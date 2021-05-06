#include <memory>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <map>
#include <sstream>
#include <utility>
#include <set>
#include <cmath>
#include <numeric>
using std::vector;
using std::map;
using std::unordered_map;
using std::pair;

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Ref.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"

#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"

#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Common/interface/View.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "DataFormats/DetId/interface/DetId.h"

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/OneToManyWithQualityGeneric.h"

#include <iterator> // For std::forward_iterator_tag
#include <cstddef>  // For std::ptrdiff_t


#define EDM_ML_DEBUG
#define PI 3.14159265358979323846

typedef edm::AssociationMap<edm::OneToManyWithQualityGeneric<
    SimClusterCollection, SimClusterCollection, float>> SimClusterToSimClusters;

/*
Yet another implementation of a 3D vector in CMSSW, but somehow
all the existing classes are missing needed functionality for the
merging algorithm.
*/
struct Vector3D{
    Vector3D() {}
    Vector3D(double x, double y, double z) : x_(x), y_(y), z_(z) {}
    Vector3D(const GlobalPoint& p) : x_(p.x()), y_(p.y()), z_(p.z()) {}
    Vector3D(const Vector3D& p) : x_(p.x_), y_(p.y_), z_(p.z_) {}
    ~Vector3D() {}
    Vector3D operator+(const Vector3D& other) const { return Vector3D(x_+other.x_, y_+other.y_, z_+other.z_); }
    Vector3D operator-(const Vector3D& other) const { return Vector3D(x_-other.x_, y_-other.y_, z_-other.z_); }
    Vector3D operator/(const double c){ return Vector3D(x_/c, y_/c, z_/c); }

    double norm(){ return std::sqrt(std::pow(x_,2) + std::pow(y_,2) + std::pow(z_,2)); }
    double dot(const Vector3D& other){ return x_ * other.x_ + y_ * other.y_ + z_ * other.z_; }

    double x_, y_, z_;
    friend std::ostream& operator<<(std::ostream& os, const Vector3D& v);
    };

std::ostream& operator<<(std::ostream& os, const Vector3D& v){
    os << "(" << v.x_ << ", " << v.y_ << ", " << v.z_ << ")";
    return os;
    }

Vector3D operator*(const double c, const Vector3D& p){ return Vector3D(c*p.x_, c*p.y_, c*p.z_); }
vector<Vector3D> operator+(const vector<Vector3D>& ps, const Vector3D& q){
    vector<Vector3D> out(ps.size());
    for (std::size_t i = 0; i < ps.size(); ++i){ 
        const Vector3D& p = ps[i];
        out[i] = p + q;
        }
    return out;
    }
vector<Vector3D> operator-(const vector<Vector3D>& ps, const Vector3D& q){
    vector<Vector3D> out(ps.size());
    for (std::size_t i = 0; i < ps.size(); ++i){ 
        const Vector3D& p = ps[i];
        out[i] = p - q;
        }
    return out;
    }

/* Needed for elementary 3D rotations */
struct RotMat3D{
    RotMat3D() {}
    ~RotMat3D() {}
    RotMat3D(
        double e11, double e12, double e13,
        double e21, double e22, double e23,
        double e31, double e32, double e33
        ) :
        e11_(e11), e12_(e12), e13_(e13),
        e21_(e21), e22_(e22), e23_(e23),
        e31_(e31), e32_(e32), e33_(e33)
        {}

    Vector3D dot(const Vector3D& p){
        return Vector3D(
            e11_*p.x_ + e12_*p.y_ + e13_*p.z_,
            e21_*p.x_ + e22_*p.y_ + e23_*p.z_,
            e31_*p.x_ + e32_*p.y_ + e33_*p.z_
            );
        }

    vector<Vector3D> dot(const vector<Vector3D>& ps){
        vector<Vector3D> out(ps.size());
        for (std::size_t i = 0; i < ps.size(); ++i){
            out[i] = dot(ps[i]);
            }
        return out;
        }

    RotMat3D dot(const RotMat3D& o){
        return RotMat3D(
            e11_*o.e11_ + e12_*o.e21_ + e13_*o.e31_, e11_*o.e12_ + e12_*o.e22_ + e13_*o.e32_, e11_*o.e13_ + e12_*o.e23_ + e13_*o.e33_,
            e21_*o.e11_ + e22_*o.e21_ + e23_*o.e31_, e21_*o.e12_ + e22_*o.e22_ + e23_*o.e32_, e21_*o.e13_ + e22_*o.e23_ + e23_*o.e33_,
            e31_*o.e11_ + e32_*o.e21_ + e33_*o.e31_, e31_*o.e12_ + e32_*o.e22_ + e33_*o.e32_, e31_*o.e13_ + e32_*o.e23_ + e33_*o.e33_
            );
        }

    RotMat3D transpose(){
        return RotMat3D(
            e11_, e21_, e31_,
            e12_, e22_, e32_,
            e13_, e23_, e33_
            );
        }

    friend std::ostream& operator<<(std::ostream& os, const RotMat3D& m);

    double e11_, e12_, e13_,
          e21_, e22_, e23_,
          e31_, e32_, e33_;
    };

std::ostream& operator<<(std::ostream& os, const RotMat3D& m){
    os  << "[" << m.e11_ << ", " << m.e12_ << ", " << m.e13_ << "]\n"
        << "[" << m.e21_ << ", " << m.e22_ << ", " << m.e23_ << "]\n"
        << "[" << m.e31_ << ", " << m.e32_ << ", " << m.e33_ << "]";
    return os;
    }


struct Hit {
    Hit(double x, double y, double z, double t, double energy, int trackid) :
        x_(x), y_(y), z_(z), t_(t), energy_(energy), trackid_(trackid) {}
    ~Hit() {}
    double x_;
    double y_;
    double z_;
    double t_;
    double energy_;
    int trackid_;
    GlobalPoint gpoint(){return GlobalPoint(x_, y_, z_);}
    Vector3D vector3d(){return Vector3D(x_, y_, z_);}
    };

/* Computes the 'average' position of a list of hits */
Vector3D hitcentroid(vector<Hit*>& hits){
    if (hits.size()==0) cms::Exception("SimMerging") << "Cannot compute hit centroid for 0 hits";
    else if (hits.size()==1) return Vector3D(hits[0]->x_, hits[0]->y_, hits[0]->z_);
    double summedEnergy = 0.;
    for(auto hit : hits) summedEnergy += hit->energy_;
    double center_x = 0., center_y = 0., center_z = 0.;
    for(auto hit : hits){
        double weight = hit->energy_/summedEnergy;
        center_x += weight * hit->x_;
        center_y += weight * hit->y_;
        center_z += weight * hit->z_;
        }
    return Vector3D(center_x, center_y, center_z);
    }

/* Cumulative sum of a vector */
template<typename T>
vector<T> cumsum(const vector<T>& input){
    T s = 0.;
    vector<T> cumulative_sum(input.size());
    for (std::size_t i = 0; i < input.size(); ++i) {
        s += input[i];
        cumulative_sum[i] = s;
        }
    return cumulative_sum;
    }

/*
Returns a permutation of a sorted vector, which is applicable on another vector
See https://stackoverflow.com/a/17074810
*/
template <typename T, typename Compare>
vector<std::size_t> argsort(
    const vector<T>& vec,
    Compare& compare)
    {
        vector<std::size_t> p(vec.size());
        std::iota(p.begin(), p.end(), 0);
        std::sort(p.begin(), p.end(),
            [&](std::size_t i, std::size_t j){ return compare(vec[i], vec[j]); });
        return p;
        }
/* Same but using a default "<" comparison */
template <typename T>
vector<std::size_t> argsort(const vector<T>& vec){
    vector<std::size_t> p(vec.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(
        p.begin(), p.end(),
        [&](std::size_t i, std::size_t j){ return vec[i] < vec[j]; }
        );
    return p;
    }
/* Function to apply the argsort returned from argsort(), returning a copy */
template <typename T>
std::vector<T> apply_argsort(
    const std::vector<T>& vec,
    const std::vector<std::size_t>& p)
{
    std::vector<T> sorted_vec(vec.size());
    std::transform(p.begin(), p.end(), sorted_vec.begin(),
        [&](std::size_t i){ return vec[i]; });
    return sorted_vec;
}
/* Function to apply the argsort returned from argsort() in place */
template <typename T>
void apply_argsort_in_place(
    std::vector<T>& vec,
    const std::vector<std::size_t>& p)
{
    std::vector<bool> done(vec.size());
    for (std::size_t i = 0; i < vec.size(); ++i)
    {
        if (done[i])
        {
            continue;
        }
        done[i] = true;
        std::size_t prev_j = i;
        std::size_t j = p[i];
        while (i != j)
        {
            std::swap(vec[prev_j], vec[j]);
            done[j] = true;
            prev_j = j;
            j = p[j];
        }
    }
}

class Node {
    public:
        Node() : 
            trackid_(0), pdgid_(0), initial_energy_(0.), parent_(nullptr)
            {}
        Node(const SimTrack& track) {
            trackid_ = track.trackId();
            initial_energy_ = track.momentum().E();
            pdgid_ = track.type();
            is_hadron_ = pdgid_ > 111;
            final_z_ = track.trackerSurfacePosition().z();
            merged_trackids_.push_back(trackid_);
            crossed_boundary_ = track.crossedBoundary();
            if (crossed_boundary_){
                boundary_momentum_ = track.getMomentumAtBoundary();
                boundary_position_ = Vector3D(
                    track.getPositionAtBoundary().x(),
                    track.getPositionAtBoundary().y(),
                    track.getPositionAtBoundary().z()
                    );
                }
            }
        ~Node() {}

        /* Number of quantities that depend on the hits */
        void calculate_shower_variables(){
            if (nhits() == 0) return;
            centroid_ = ::hitcentroid(hits_);
            axis_ = (centroid_-boundary_position_) / (centroid_-boundary_position_).norm();

            // Compute the transverse distances from hits to the shower axis
            vector<double> d_to_axis;
            vector<double> d_along_axis;
            vector<double> energies;
            double total_energy = 0.;
            for(auto hit : hits_){
                Vector3D hit_pos = hit->vector3d() - boundary_position_;
                Vector3D projection_along_axis = hit_pos.dot(axis_) * axis_;
                d_along_axis.push_back(projection_along_axis.norm());
                d_to_axis.push_back((hit_pos - projection_along_axis).norm());
                energies.push_back(hit->energy_);
                total_energy += hit->energy_;
                }

            vector<std::size_t> order = argsort(d_to_axis);
            apply_argsort_in_place(d_to_axis, order);
            vector<double> cumsum_energies_to_axis = cumsum(apply_argsort(energies, order));

            // Find the energy containment radii
            vector<double> thresholds = { .3, .75, .85 };
            for (std::size_t i = 0; i < cumsum_energies_to_axis.size()-1; ++i) {
                double cumsum_this = cumsum_energies_to_axis[i] / total_energy;
                double cumsum_next = cumsum_energies_to_axis[i+1] / total_energy;
                for(auto threshold : thresholds){
                    if (cumsum_next > threshold && cumsum_this < threshold)
                        // We just crossed a threshold, save the radius
                        energy_containment_radii_[threshold] = d_to_axis[i+1];
                    }
                }

            // Same thing, now longitudinally instead of transverse
            order = argsort(d_along_axis);
            apply_argsort_in_place(d_along_axis, order);
            vector<double> cumsum_energies_along_axis = cumsum(apply_argsort(energies, order));

            thresholds = { .1, .9 };
            // Find the longitudinal energy containment quantiles
            for (std::size_t i = 0; i < cumsum_energies_along_axis.size()-1; ++i) {
                double cumsum_this = cumsum_energies_along_axis[i] / total_energy;
                double cumsum_next = cumsum_energies_along_axis[i+1] / total_energy;
                for(auto threshold : thresholds){
                    if (cumsum_next > threshold && cumsum_this < threshold){
                        // We just crossed a threshold, save the radius
                        energy_containment_longitudinally_[threshold] = d_along_axis[i+1];
                        }
                    }
                }

            // Build rotation matrix for this shower axis
            // R.dot(v) will rotate v to a coordinate system where the z-axis
            // is aligned with the z-axis of `axis`
            // See https://en.wikipedia.org/wiki/Rotation_matrix#In_three_dimensions
            double dx = atan2(axis_.y_, axis_.z_);
            double dy = -asin(axis_.x_ / axis_.norm());
            rotation_ = RotMat3D(
                cos(dy), 0., sin(dy),
                0., 1., 0.,
                -sin(dy), 0., cos(dy)
                )
                .dot(RotMat3D(
                    1., 0., 0.,
                    0., cos(dx), -sin(dx),
                    0., sin(dx), cos(dx)
                    ));

            inv_rotation_ = rotation_.transpose();
            }

        /* Standard depth-first-search tree traversal as an iterator */
        struct Iterator {
            using iterator_category = std::forward_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = Node;
            using pointer           = Node*;  // or also value_type*
            using reference         = Node&;  // or also value_type&

            Iterator(pointer ptr, bool verbose=false) :
                m_ptr(ptr), root(ptr), depth_(0), verbose_(verbose) {}

            reference operator*() const { return *m_ptr; }
            pointer operator->() { return m_ptr; }

            Iterator& operator++() {
                if (m_ptr->hasChildren()){
                    if (verbose_) edm::LogVerbatim("SimMerging")
                        << "Track " << m_ptr->trackid_
                        << ": Going to first child " << m_ptr->children_[0]->trackid_
                        ;
                    continuation_.push(m_ptr);
                    m_ptr = m_ptr->children_[0];
                    depth_++;
                    }
                else {
                    if (verbose_) edm::LogVerbatim("SimMerging")
                        << "Track " << m_ptr->trackid_
                        << ": No children, going to next sibling"
                        ;
                    while(true){
                        if (m_ptr == root){
                            if (verbose_) edm::LogVerbatim("SimMerging") << "Back at the root of the iterator; quiting";
                            m_ptr = nullptr;
                            break;
                            }
                        else if (m_ptr->hasNextSibling()){
                            m_ptr = m_ptr->nextSibling();
                            if (verbose_) edm::LogVerbatim("SimMerging") << "Has sibling; going to " << m_ptr->trackid_;
                            break;
                            }
                        if (verbose_) edm::LogVerbatim("SimMerging") << "Has no sibling; proceed popping stack";
                        m_ptr = continuation_.top();
                        continuation_.pop();
                        depth_--;
                        if (verbose_) edm::LogVerbatim("SimMerging") << "Popped back to track " << m_ptr->trackid_;
                        }
                    }
                return *this;
                }
            // Postfix increment
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
            const int depth(){return depth_;}
            friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };  
            private:
                pointer m_ptr;
                pointer root;
                int depth_;
                bool verbose_;
                std::stack<pointer> continuation_;
            };
        Iterator begin(bool verbose=false) { return Iterator(this, verbose); }
        Iterator end() { return Iterator(nullptr); }

        /* Traverses upwards */
        struct IteratorUp {
            using iterator_category = std::forward_iterator_tag;
            using difference_type   = std::ptrdiff_t;
            using value_type        = Node;
            using pointer           = Node*;  // or also value_type*
            using reference         = Node&;  // or also value_type&
            IteratorUp(pointer ptr) : m_ptr(ptr) {}
            reference operator*() const { return *m_ptr; }
            pointer operator->() { return m_ptr; }
            IteratorUp& operator++() {
                m_ptr = (m_ptr->hasParent()) ? m_ptr->parent_ : nullptr ;
                return *this;
                }
            IteratorUp operator++(int) { IteratorUp tmp = *this; ++(*this); return tmp; }
            friend bool operator== (const IteratorUp& a, const IteratorUp& b) { return a.m_ptr == b.m_ptr; };
            friend bool operator!= (const IteratorUp& a, const IteratorUp& b) { return a.m_ptr != b.m_ptr; };  
            private: pointer m_ptr;
            };
        IteratorUp begin_up() { return IteratorUp(this); }
        IteratorUp end_up() { return IteratorUp(nullptr); }

        void setParent(Node* parent) {parent_ = parent;}
        void addChild(Node* child) {children_.push_back(child);}
        void addHit(Hit* hit) {hits_.push_back(hit);}
        int nhits(){ return hits_.size(); }
        bool hasHits(){ return nhits() > 0; }
        bool isLeaf(){ return children_.empty(); }
        bool hasChildren(){ return !(children_.empty()); }
        bool hasParent(){ return parent_ != nullptr; }

        bool hasNextSibling(){
            if (!parent_){
                // There is no parent
                return false;
                }
            else if (parent_->children_.back() == this){
                // This was the last child
                return false;
                }
            // else if ( (int)(parent_->children_.size()-1) == iChild_){
            //     // This was the last child
            //     return false;
            //     }
            return true;
            }

        Node* nextSibling(){
            if (hasNextSibling()){
                vector<Node*>::iterator sibling = std::find(
                    parent_->children_.begin(), parent_->children_.end(), this
                    );
                sibling++; // advance once
                if (sibling == parent_->children_.end())
                    // This shouldn't happen
                    return nullptr;
                return *sibling;
                }
            return nullptr;
            }

        /* Traverses tree and builds string representation */
        std::string stringrep(){
            std::stringstream ss;
            int nTracks = 0;
            int nHits = 0;
            for (Node::Iterator it = begin(); it != end(); it++){
                Node& node = (*it);
                for (int i = 0; i < it.depth(); ++i){
                    ss << "--";
                    }
                ss
                    << "Track " << node.trackid_
                    << " (" << node.nhits() << " hits)"
                    << "\n";
                nTracks++;
                nHits += node.nhits();
                }
            ss << "In total " << nTracks << " tracks with " << nHits << " hits";
            return ss.str();
            }

        /* A node is a 'leaf parent' if it has children, and all those children are leafs  */
        bool isLeafParent(){
            // A leaf itself is not a leaf parent
            if (isLeaf()) return false;
            for(auto child : children_){
                if (child->hasChildren()) return false;
                }
            return true;
            }

        /*
        Flips all basic z quantities by -1
        Does not flip the shower properties; calculate_shower_variables must be rerun
        */
        void flipz(){
            final_z_ *= -1.;
            if(crossed_boundary_) boundary_position_.z_ *= -1.;
            for(auto hit : hits_) hit->z_ *= -1.;
            }

        bool crossed_boundary_, is_hadron_;
        int trackid_, pdgid_;
        double initial_energy_, final_z_;
        math::XYZTLorentzVectorF boundary_momentum_;

        Vector3D centroid_, axis_, boundary_position_;
        map<double, double> energy_containment_radii_, energy_containment_longitudinally_;
        RotMat3D rotation_, inv_rotation_;

        Node * parent_;
        vector<Node*> children_;
        vector<int> merged_trackids_;
        vector<Hit*> hits_;
    };

/* Finds a track by trackid in a tree */
Node* find_in_tree(Node* root, int trackid){
    for(auto& track : *root){
        if (track.trackid_ == trackid){
            return &track;
            }
        }
    throw cms::Exception("Unknown")
        << "Track id " << trackid << " is not in the tree";
    }

/* Returns a circle of Vector3D's in the xy plane */
vector<Vector3D> get_circle(double r, int N=30) {
    vector<Vector3D> circle(N);
    for (int i = 0; i < N; ++i) {
        double angle = 2.*PI * double(i)/double(N-1);
        circle[i] = Vector3D(r*cos(angle), r*sin(angle), 0.);
        }
    return circle;
    }

/*
First rotates the 10% and 90% longitudinal energy quantile vectors
to the reference frame of the most energetic shower (should be t1)
Then calculates delta z between the end point of the first and 
the beginning of the second track:

b1------e1
                b2----------e2
          ----->


b1------e1
     b2----------e2
     <----


b1-----------------e1
     b2-----e2
     <---------------
*/
double longitudinal_distance(Node& t1, Node& t2){
    // Vectors of the 10% and 90% longitudinal energy quantiles
    Vector3D v1_10 = t1.boundary_position_ + t1.energy_containment_longitudinally_[.1] * t1.axis_;
    Vector3D v1_90 = t1.boundary_position_ + t1.energy_containment_longitudinally_[.9] * t1.axis_;
    Vector3D v2_10 = t2.boundary_position_ + t2.energy_containment_longitudinally_[.1] * t2.axis_;
    Vector3D v2_90 = t2.boundary_position_ + t2.energy_containment_longitudinally_[.9] * t2.axis_;
    // Rotate them
    Vector3D origin = t1.boundary_position_;
    Vector3D rb1 = t1.rotation_.dot(v1_10 - origin);
    Vector3D re1 = t1.rotation_.dot(v1_90 - origin);
    Vector3D rb2 = t1.rotation_.dot(v2_10 - origin);
    Vector3D re2 = t1.rotation_.dot(v2_90 - origin);
    // Fix 1 to be the lowest in z after rotating
    if (rb2.z_ < rb1.z_){ std::swap(rb1, rb2); std::swap(re1, re2); }
    return rb2.z_ - re1.z_;
    }

/*
Winding number calculation to determine if a point is inside a 2D polygon
(https://en.wikipedia.org/wiki/Nonzero-rule)
point: (x, y)
polygon: [ (x1, y1), ..., (xn, yn) ]
*/
bool is_inside(const vector<double>& px, const vector<double>& py, double x, double y){
    int winding_number = 0;
    int n_edges = px.size();
    for (int i_edge = 0; i_edge < n_edges; ++i_edge){
        int i_edge_next = (i_edge+1) % n_edges; // Loop back for last edge
        double x1 = px[i_edge], x2 = px[i_edge_next], y1 = py[i_edge], y2 = py[i_edge_next];
        // Skip any edges completely to the left of x
        if (x1 < x && x2 < x) continue;
        // Skip any edges that don't cross the line at y=y
        if ((y1 < y && y2 < y) || (y1 > y && y2 > y)) continue;
        if (x1==x2){
            // Vertical edge - no need to calculate crossing point, slope is also infinite
            if (x1>x) winding_number++;
            }
        else {
            // Edges that cross the line y=y and are not vertical;
            // need to calculate crossing point
            if ( (y - y1) / ((y2-y1)/(x2-x1)) + x1 > x ) winding_number++;
            }
        }
    return winding_number % 2 == 1; // false if even (out), and true if odd (in)
    }

/* Evaluates the overlap of two 2D polygons numerically */
double polygon_overlap(
    const vector<double>& x1, const vector<double>& y1,
    const vector<double>& x2, const vector<double>& y2,
    int nbins=50
    ){
    // First determine extrema
    auto x1_minmax = std::minmax_element(x1.begin(), x1.end());
    auto x2_minmax = std::minmax_element(x2.begin(), x2.end());
    double xmin = std::min(*(x1_minmax.first), *(x2_minmax.first));
    double xmax = std::max(*(x1_minmax.second), *(x2_minmax.second));
    auto y1_minmax = std::minmax_element(y1.begin(), y1.end());
    auto y2_minmax = std::minmax_element(y2.begin(), y2.end());
    double ymin = std::min(*(y1_minmax.first), *(y2_minmax.first));
    double ymax = std::max(*(y1_minmax.second), *(y2_minmax.second));
    double x_binwidth = (xmax-xmin) / double(nbins);
    double y_binwidth = (ymax-ymin) / double(nbins);

    // Count how many cells in a grid are inside polygon 2 and in both polygons
    int is_inside_2 = 0, is_inside_1_and_2 = 0;
    for (int i = 0; i < nbins; ++i){
        for (int j = 0; j < nbins; ++j){
            double x = xmin + i*x_binwidth + .5*x_binwidth;
            double y = ymin + j*y_binwidth + .5*y_binwidth;
            if (is_inside(x2, y2, x, y)){
                is_inside_2++;
                if (is_inside(x1, y1, x, y)) is_inside_1_and_2++;
                }
            }
        }
    if (is_inside_1_and_2 == 0) return 0.;
    return double(is_inside_1_and_2) / double(is_inside_2);
    }
/* Reimplementation of function above with different input */
double polygon_overlap(vector<Vector3D>& polygon1, vector<Vector3D>& polygon2, int nbins=30){
    // Convert to vector<double>; Would have been better to not have a copy here,
    // but the other algos get hard to write then
    std::size_t n(polygon1.size());
    vector<double> polygon1_x(n), polygon1_y(n), polygon2_x(n), polygon2_y(n);
    for (std::size_t i=0; i<n; ++i){
        polygon1_x[i] = polygon1[i].x_;
        polygon1_y[i] = polygon1[i].y_;
        polygon2_x[i] = polygon2[i].x_;
        polygon2_y[i] = polygon2[i].y_;
        }
    return polygon_overlap(polygon1_x, polygon1_y, polygon2_x, polygon2_y, nbins);
    }

pair<double, double> calculate_shower_overlap(Node& t1, Node& t2){
    if (t1.boundary_momentum_.E() < t2.boundary_momentum_.E()) std::swap(t1,t2);
    double f_radius;
    if(t1.is_hadron_ != t2.is_hadron_) f_radius = .3;
    else if (t1.is_hadron_ && t2.is_hadron_) f_radius = .75;
    else f_radius = .85;
    double t1_r = std::max(t1.energy_containment_radii_[f_radius], 1.0);
    double t2_r = std::max(t2.energy_containment_radii_[f_radius], 1.0);

    Vector3D t1_b = t1.boundary_position_;
    Vector3D t1_e = t1.centroid_;
    Vector3D t2_b = t2.boundary_position_;
    Vector3D t2_e = t2.centroid_;

    Vector3D t1_re = t1.rotation_.dot(t1_e-t1_b);
    // Vector3D t2_rb = t1.rotation_.dot(t2_b-t1_b);
    // Vector3D t2_re = t1.rotation_.dot(t2_e-t1_b);

    vector<Vector3D> t1_rcircle = t1.inv_rotation_.dot(get_circle(t1_r)) + t1_re;
    vector<Vector3D> t2_rcircle = t1.rotation_.dot(
        t2.inv_rotation_.dot(get_circle(t2_r)) + t2_e - t1_b
        );
    double rcircle_overlap = polygon_overlap(t1_rcircle, t2_rcircle);
    double deltaz = longitudinal_distance(t1, t2);
    return std::make_pair(rcircle_overlap, deltaz);
    }


class CachedOverlapFn {
    public:
        CachedOverlapFn() {}
        ~CachedOverlapFn() {}

        pair<double, double> get(Node* t1, Node* t2){
            if (t1->boundary_momentum_.E() < t2->boundary_momentum_.E()) std::swap(t1,t2);
            pair<int,int> p = std::make_pair(t1->trackid_, t2->trackid_);
            if (cache_.count(p) == 0)
                cache_[p] = calculate_shower_overlap(*t1, *t2);
            return cache_[p];
            }

        /* Clears the cache for two tracks in one go */
        void clear(Node* t1, Node* t2){
            auto it = cache_.begin();
            while(it != cache_.end()){
                auto key = (*it).first;
                if (
                    key.first == t1->trackid_ || key.second == t1->trackid_ ||
                    key.first == t2->trackid_ || key.second == t2->trackid_
                    )
                    it = cache_.erase(it);
                else
                    it++;
                }
            }

        void insert(int i, int j, double a, double b){
            cache_[std::make_pair(i,j)] = std::make_pair(a,b);
            }

        std::string print() const {
            std::stringstream ss;
            for(auto it: cache_){
                ss  << "(" << it.first.first << "," << it.first.second << ") = "
                    << it.second.first << ", " << it.second.second << "\n"
                    ;
                }
            return ss.str();
            }

    private:
        map<pair<int,int>, pair<double,double>> cache_;  
    };



/* Remove a node from its parent's children vector */
void break_from_parent(Node* node){
    if (!(node->hasParent())) cms::Exception("SimMerging") << "Cannot remove root node";
    vector<Node*>& parents_children = node->parent_->children_;
    // erase-remove idiom: https://en.wikipedia.org/wiki/Erase%E2%80%93remove_idiom#Example
    parents_children.erase(
        std::remove(parents_children.begin(), parents_children.end(), node),
        parents_children.end()
        );
    }

/* Breaks node from parent but moves its children to the children of the parent */
void remove_intermediate_node(Node* node){
    Node* parent = node->parent_;
    break_from_parent(node);
    // Move children of the now-removed node to its parent
    for (auto child : node->children_){
        parent->addChild(child);
        child->setParent(parent);
        }
    }

// _______________________________________________
// Some functions for traversal by recursion
// These first build the whole traversal in a vector
// (as pointers, so memory usage is not too bad)

/* Does depth first search traversal by using recursion, but not as an iterator */
void _dfs_recursion(
        Node* node,
        vector<pair<Node*, int>>& returnable,
        int depth
        )
    {
    returnable.push_back(std::make_pair(node, depth));
    for (auto child : node->children_){
        _dfs_recursion(child, returnable, depth+1);
        }
    }

/*
Wrapper around the recursive version that only takes a node as input.
Returns a vector of pair<node, depth>.
Useful if you want to keep the whole traversal in memory; usually you
will want to use the iterator version of the Node class.
*/
vector<pair<Node*, int>> dfs(Node* root){
    vector<pair<Node*, int>> returnable;
    _dfs_recursion(root, returnable, 0);
    return returnable;
    }

/* String representation of dfs traversal (keeps whole traversal in memory) */
std::string dfs_stringrep(Node* root){
    std::stringstream ss;
    for (auto node_depth_pair : dfs(root)){
        for (int i = 0; i < node_depth_pair.second; ++i){
            ss << "--";
            }
        ss
            << "Track " << node_depth_pair.first->trackid_
            << " (" << node_depth_pair.first->nhits() << " hits)"
            << "\n";
        }
    return ss.str();
    }

/* Remove single-child no-hit tracks (i.e. intermediate tracks) */
void trim_tree(Node* root){
    // Traverse once and note all tracks that should be kept:
    // Either a track that has hits, or an ancestor thereof
    std::set<int> trackids_with_hits_or_parents_thereof;
    for (auto& node : *root){
        if (!(node.hasHits())) continue;
        // Iterate upwards and save in the set
        for (auto it=node.begin_up(); it!=node.end_up(); it++){
            trackids_with_hits_or_parents_thereof.insert(it->trackid_);
            }
        }
    // Now remove all nodes not in the set
    // We'll be modifying parent-child relationships mid-loop, so we have to be a little
    // careful
    auto it=root->begin();
    while(it!=root->end()){
        Node& node = (*it);
        if (!(trackids_with_hits_or_parents_thereof.count(node.trackid_))){
            // First remove children so the iterator will go to the next sibling
            node.children_.clear();
            // Advance to next sibling (or further up the chain)
            it++;
            // Then break from parent (if doing this before advancing the order gets messed up)
            break_from_parent(&node);
            }
        else{
            it++;
            }
        }
    // Second trimming step: Remove 'intermediate' tracks
    // (i.e. tracks with no hits, 1 child, and 1 parent)
    // In this case it's easier to put the whole traversal in memory first,
    // and avoid modifying relationships mid-loop
    for (auto node_depth_pair : dfs(root)){
        Node* node = node_depth_pair.first;
        if (node->hasParent() && (node->children_.size()==1) && !(node->hasHits())){
            remove_intermediate_node(node);
            }
        }
    }


/* Compute a distance measure between two nodes: now simply distance between the hit centroids */
double distance(Node* left, Node* right){
    Vector3D p1 = left->centroid_, p2 = right->centroid_;
    return std::sqrt(
        std::pow(p1.x_-p2.x_,2) + std::pow(p1.y_-p2.y_,2) + std::pow(p1.z_-p2.z_,2)
        );
    }

bool merge_leafparent(Node* leafparent, double min_overlap, CachedOverlapFn& overlapfn){
    edm::LogVerbatim("SimMerging") << "  Merging leafparent " << leafparent->trackid_;
    bool didUpdate = false;
    // Copy list of potentially mergeable nodes
    vector<Node*> mergeable = leafparent->children_;
    leafparent->children_.clear();
    // Parent itself can be mergeable, if it has hits and is not a root
    if (leafparent->hasParent() && leafparent->hasHits()) mergeable.push_back(leafparent);
    // Start merging
    while(true){
        bool break_early = false;
        bool didUpdateThisIteration = false;
        double current_max_overlap = min_overlap;
        pair<Node*,Node*> pairToMerge;
        // Compute all distances between clusters
        int nMergeable = mergeable.size();
        for (int i = 0; i < nMergeable; ++i){
            Node* left = mergeable[i];
            for (int j = i+1; j < nMergeable; ++j){
                Node* right = mergeable[j];
                pair<double, double> p = overlapfn.get(left, right);
                double overlap = p.first;
                double deltaz = p.second;
                if (overlap > current_max_overlap && deltaz < 10.){
                    current_max_overlap = overlap;
                    pairToMerge = (left->initial_energy_ > right->initial_energy_) ?
                        std::make_pair(left, right) : std::make_pair(right, left);
                    didUpdate = true;
                    didUpdateThisIteration = true;
                    if (overlap == 1.){
                        // It can't get higher than 1 anyway
                        break_early = true;
                        break;
                        }
                    }
                // double r = distance(left, right);
                // if (r < minr){
                //     minr = r;
                //     pairToMerge = (left->initial_energy_ > right->initial_energy_) ?
                //         std::make_pair(left, right) : std::make_pair(right, left);
                //     didUpdate = true;
                //     didUpdateThisIteration = true;
                //     }
                }
            if (break_early) break;
            }
        if (!didUpdateThisIteration) break; // Nothing to merge this iteration
        // Now do the merging
        edm::LogVerbatim("SimMerging")
            << "    Merging " << pairToMerge.second->trackid_
            << " into " << pairToMerge.first->trackid_
            ;
        // Bookkeep that the track (and any previously merged tracks) is merged in
        for (auto trackid : pairToMerge.second->merged_trackids_){
            pairToMerge.first->merged_trackids_.push_back(trackid);
            }
        // Move children
        for(auto child : pairToMerge.second->children_){
            pairToMerge.first->addChild(child);
            child->setParent(pairToMerge.first);
            }
        pairToMerge.second->children_.clear();
        // Move hits
        for(auto hit : pairToMerge.second->hits_) pairToMerge.first->addHit(hit);
        pairToMerge.second->hits_.clear();
        // Delete the merged-away node
        break_from_parent(pairToMerge.second);
        mergeable.erase(
            std::remove(mergeable.begin(), mergeable.end(), pairToMerge.second),
            mergeable.end()
            );
        // Recompute the shower variables for newly merged node, now that it has more hits
        pairToMerge.first->calculate_shower_variables();
        overlapfn.clear(pairToMerge.first, pairToMerge.second);
        }
    // Make a string representation of the mergeable nodes for debugging
    std::string mergeableStr = "";
    if(mergeable.size()){
        std::stringstream ss;
        for(auto child : mergeable) ss << child->trackid_ << ", ";
        mergeableStr = ss.str();
        mergeableStr.pop_back(); mergeableStr.pop_back(); // Remove trailing comma
        }
    // All possible merging now done;
    // Next steps depend on whether the passed node was a root
    if (!(leafparent->hasParent())){
        leafparent->children_ = mergeable;
        if(didUpdate) {
            // Simply overwrite with the merged nodes
            edm::LogVerbatim("SimMerging")
                << "    Root " << leafparent->trackid_
                << " is set to have the following children: "
                << mergeableStr;
            }
        else{
            edm::LogVerbatim("SimMerging")
                << "    Root " << leafparent->trackid_
                << ": no further merging possible";
            }
        return didUpdate;
        }
    else {
        // Special case: If the leafparent had no hits (and was thus not included as
        // a mergeable node), AND all nodes were merged into one cluster, assign the 
        // pdgid of the leafparent to the remaining node
        if(
            !(leafparent->hasHits())
            && mergeable.size()==1
            && mergeable[0]->pdgid_!=leafparent->pdgid_
            ){
            edm::LogVerbatim("SimMerging")
                << "    Using leafparent pdgid " << leafparent->pdgid_
                << " for track " << mergeable[0]->trackid_
                << " (rather than " << mergeable[0]->pdgid_
                << ") since all nodes were merged into one";
            mergeable[0]->pdgid_ = leafparent->pdgid_;
            }
        // Replace the node in the parent's children list with all merged nodes
        Node* parent = leafparent->parent_;
        edm::LogVerbatim("SimMerging")
            << "    Adding the following children to parent " << parent->trackid_
            << ": " << mergeableStr;
        break_from_parent(leafparent);
        for(auto child : mergeable){
            parent->addChild(child);
            child->setParent(parent);
            }
        return true;
        }
    }

void merging_algo(Node* root){
    CachedOverlapFn overlapfn;
    int iIteration = -1;
    bool didUpdate = true;
    while(didUpdate){
        iIteration++;
        edm::LogVerbatim("SimMerging") << "Iteration " << iIteration;
        // Build list of leaf parents in memory
        vector<Node*> leafparents;
        for (auto& node : *root){
            if (!(node.isLeafParent())) continue;
            leafparents.push_back(&node);
            }
        for (auto node : leafparents){
            didUpdate = merge_leafparent(node, .3, overlapfn);
            }
        }
    edm::LogVerbatim("SimMerging") << "Done after iteration " << iIteration;
    }

// _______________________________________________


class simmerger : public edm::stream::EDProducer<> {
    public:
        explicit simmerger(const edm::ParameterSet&);
        ~simmerger() {}
        SimCluster mergedSimClusterFromTrackIds(std::vector<int>& trackIds, 
            const edm::Association<SimClusterCollection>& simTrackToSimCluster);
    private:
        virtual void produce(edm::Event&, const edm::EventSetup&) override;
        void beginRun(const edm::Run&, const edm::EventSetup&) override {}
        hgcal::RecHitTools hgcalRecHitToolInstance_ ;
        edm::EDGetTokenT<edm::View<PCaloHit>> hgcalEEHitsToken_;
        edm::EDGetTokenT<edm::View<PCaloHit>> hgcalHEfrontHitsToken_;
        edm::EDGetTokenT<edm::View<PCaloHit>> hgcalHEbackHitsToken_;
        edm::EDGetTokenT<edm::SimTrackContainer> tokenSimTracks;
        edm::EDGetTokenT<edm::SimVertexContainer> tokenSimVertices;
        edm::EDGetTokenT<SimClusterCollection> simClustersToken_;
        edm::EDGetTokenT<edm::Association<SimClusterCollection>> simTrackToSimClusterToken_;
        unordered_map<unsigned int, SimTrackRef> trackIdToTrackRef_;
    };


simmerger::simmerger(const edm::ParameterSet& iConfig) :
    hgcalEEHitsToken_(consumes<edm::View<PCaloHit>>(edm::InputTag("g4SimHits", "HGCHitsEE"))),
    hgcalHEfrontHitsToken_(consumes<edm::View<PCaloHit>>(edm::InputTag("g4SimHits", "HGCHitsHEfront"))),
    hgcalHEbackHitsToken_(consumes<edm::View<PCaloHit>>(edm::InputTag("g4SimHits", "HGCHitsHEback"))),
    tokenSimTracks(consumes<edm::SimTrackContainer>(edm::InputTag("g4SimHits"))),
    tokenSimVertices(consumes<edm::SimVertexContainer>(edm::InputTag("g4SimHits"))),
    simClustersToken_(consumes<SimClusterCollection>(edm::InputTag("mix:MergedCaloTruth"))),
    simTrackToSimClusterToken_(consumes<edm::Association<SimClusterCollection>>(edm::InputTag("mix:simTrackToSimCluster")))
    {
    produces<SimClusterCollection>();
    produces<edm::Association<SimClusterCollection>>();
    produces<SimClusterToSimClusters>();
    }

SimCluster simmerger::mergedSimClusterFromTrackIds(std::vector<int>& trackIds, 
    const edm::Association<SimClusterCollection>& simTrackToSimCluster) {
    SimCluster sc; 
    for (auto tid : trackIds) {
        if (trackIdToTrackRef_.find(tid) == trackIdToTrackRef_.end())
            throw cms::Exception("SimClusterTreeMerger") << "Failed to find a trackId in the TrackMap.";
        sc += *simTrackToSimCluster[trackIdToTrackRef_[tid]];
    }
    return sc;
}

void simmerger::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {  
    edm::ESHandle<CaloGeometry> geom;
    iSetup.get<CaloGeometryRecord>().get(geom);
    hgcalRecHitToolInstance_.setGeometry(*geom);
    trackIdToTrackRef_.clear();

    auto output = std::make_unique<SimClusterCollection>();

    // Create Hit instances
    vector<Hit> hits;
    vector<edm::EDGetTokenT<edm::View<PCaloHit>>> tokens = {
        hgcalEEHitsToken_,
        hgcalHEfrontHitsToken_,
        hgcalHEbackHitsToken_
        };
    std::set<int> trackids_with_hits;
    for (edm::EDGetTokenT<edm::View<PCaloHit>> token : tokens ) {
        edm::Handle< edm::View<PCaloHit> > handle;
        iEvent.getByToken(token, handle);
        for (auto const & hit : handle->ptrs() ) {
            DetId id = hit->id();
            GlobalPoint position = hgcalRecHitToolInstance_.getPosition(id);
            hits.push_back(Hit(
                position.x(), position.y(), position.z(),
                hit->time(), hit->energy(), hit->geantTrackId()
                ));
            trackids_with_hits.insert(hit->geantTrackId());
            }
        }

    // Build the tree
    edm::Handle<edm::SimTrackContainer> handleSimTracks;
    iEvent.getByLabel("g4SimHits", handleSimTracks);
    edm::Handle<edm::SimVertexContainer> handleSimVertices;
    iEvent.getByLabel("g4SimHits", handleSimVertices);

    edm::LogVerbatim("SimMerging") << "Building map";
    unordered_map<int, Node> trackid_to_node;
    for(size_t i = 0; i < handleSimTracks->size(); i++){
        SimTrackRef track(handleSimTracks, i);
        trackid_to_node.emplace(track->trackId(), Node(*track));
        trackIdToTrackRef_[track->trackId()] = track;
	}

    edm::LogVerbatim("SimMerging") << "Adding hits to nodes";
    for (auto& hit : hits){
        trackid_to_node[hit.trackid_].addHit(&hit);
        }

    edm::LogVerbatim("SimMerging") << "Building tree";
    Node* root(new Node());
    for(const auto& track : *handleSimTracks){
        int trackid = track.trackId();
        Node* node = &(trackid_to_node[trackid]);
        // Have to get parent info via the SimVertex
        SimVertex vertex = handleSimVertices.product()->at(track.vertIndex());
        bool hasParent = !(vertex.noParent());
        // Build the tree
        if (hasParent){
            int parentid = vertex.parentIndex();
            edm::LogVerbatim("SimMerging")
                << "Setting parent->child relationship: "
                << parentid << " -> " << trackid
                ;
            auto it = trackid_to_node.find(parentid);
            if (it != trackid_to_node.end()){
                Node* parent = &(it->second);
                node->setParent(parent);
                parent->addChild(node);
                }
            else{
                throw cms::Exception("Unknown")
                    << "Track id " << parentid
                    << " is not in the map"
                    ;
                }
            }
        else{
            edm::LogVerbatim("SimMerging") << "Found parentless particle: " << node->trackid_;
            root->addChild(node);
            node->setParent(root);
            }
        }

#ifdef EDM_ML_DEBUG
    edm::LogVerbatim("SimMerging") << "Printing root " << root->trackid_;
    edm::LogVerbatim("SimMerging") << root->stringrep() << "\n";
    edm::LogVerbatim("SimMerging") << "Trimming tree...";
#endif

    trim_tree(root);

#ifdef EDM_ML_DEBUG
    edm::LogVerbatim("SimMerging") << "Printing root " << root->trackid_ << " after trimming";
    edm::LogVerbatim("SimMerging") << root->stringrep() << "\n";
#endif

#ifdef EDM_ML_DEBUG
    edm::LogVerbatim("SimMerging") << "Splitting into positive and negative endcap";
#endif
    Node* pos(new Node());
    Node* neg(new Node());
    for(auto child: root->children_){
        if(child->final_z_ < 0.){
            child->setParent(neg);
            neg->addChild(child);
            }
        else{
            child->setParent(pos);
            pos->addChild(child);
            }
        }

#ifdef EDM_ML_DEBUG
    edm::LogVerbatim("SimMerging") << "Flipping all z's in negative endcap for merging algorithm";
#endif
    for (auto& node : *neg){
        node.flipz();
        node.calculate_shower_variables();
        }

    // Testing code for development
    // Node* t1 = find_in_tree(neg, 411358);
    // Node* t2 = find_in_tree(neg, 419854);
    // edm::LogVerbatim("SimMerging") << "Found track " << t1->trackid_;
    // edm::LogVerbatim("SimMerging") << "Found track " << t2->trackid_;
    // t1->calculate_shower_variables();
    // t2->calculate_shower_variables();

    // CachedOverlapFn overlapfn;
    // overlapfn.get(t1, t2);;
    // pair<double,double> overlap = calculate_shower_overlap(*t1, *t2);
    // edm::LogVerbatim("SimMerging")
    //     << "Overlap: " << overlap.first
    //     << ", dz: " << overlap.second
    //     ;

#ifdef EDM_ML_DEBUG
    edm::LogVerbatim("SimMerging") << "Running merging algo...";
#endif
    merging_algo(neg);

#ifdef EDM_ML_DEBUG
    edm::LogVerbatim("SimMerging") << "Printing neg " << neg->trackid_ << " after merging_algo";
    edm::LogVerbatim("SimMerging") << neg->stringrep() << "\n";
#endif

    for (auto& node : *pos) node.calculate_shower_variables();
    merging_algo(pos);

    // Fill the output; the clusters are the remaining nodes (except the root)
    edm::Handle<edm::Association<SimClusterCollection>> simTrackToSimClusterHandle;
    iEvent.getByToken(simTrackToSimClusterToken_, simTrackToSimClusterHandle);

    edm::Handle<SimClusterCollection> simClusterHandle;
    iEvent.getByToken(simClustersToken_, simClusterHandle);

	auto allChildren = pos->children_;
	allChildren.insert(allChildren.begin(), neg->children_.begin(), neg->children_.end());

    size_t i = 0;
    std::vector<int> mergedIndices(simClusterHandle->size(), 0);
    std::vector<std::vector<SimClusterRef>> groups;
    for(auto cluster : allChildren) {
        SimCluster sc; 
        std::vector<SimClusterRef> group;
        for (auto tid : cluster->merged_trackids_) {
            if (trackIdToTrackRef_.find(tid) == trackIdToTrackRef_.end())
                throw cms::Exception("SimClusterTreeMerger") << "Failed to find a trackId in the TrackMap.";
            const auto& unmerged = (*simTrackToSimClusterHandle)[trackIdToTrackRef_[tid]];
            mergedIndices.at(unmerged.key()) = i;
            sc += *unmerged;
            group.push_back(unmerged);
        }
        std::sort(std::begin(group), std::end(group), 
            [](SimClusterRef a, SimClusterRef b) { return a->impactMomentum().energy() > b->impactMomentum().energy(); });
        i++;
		sc.setPdgId(cluster->pdgid_);
        output->push_back(sc);
        groups.push_back(group);
    }

    const auto& mergedSCHandle = iEvent.put(std::move(output));

    auto assocMap = std::make_unique<SimClusterToSimClusters>(mergedSCHandle, simClusterHandle);

    for (size_t i = 0; i < groups.size(); i++) {
        SimClusterRef msc(mergedSCHandle, i);
        float mergedE = msc->impactMomentum().energy();
        for (auto sc : groups.at(i)) {
            float scE = sc->impactMomentum().energy();
            assocMap->insert(msc, std::make_pair(sc, mergedE/scE));
        }
    }
    iEvent.put(std::move(assocMap));

    auto assoc = std::make_unique<edm::Association<SimClusterCollection>>(mergedSCHandle);
    edm::Association<SimClusterCollection>::Filler filler(*assoc);
    filler.insert(simClusterHandle, mergedIndices.begin(), mergedIndices.end());
    filler.fill();
    iEvent.put(std::move(assoc));

    delete root;
    delete pos;
    delete neg;
    }

DEFINE_FWK_MODULE(simmerger);
