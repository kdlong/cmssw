
#include "../interface/SimClusterMerger.h"


math::XYZTLorentzVectorF SimClusterMergeWrapper::calcBoundaryPos(const SimCluster* sc)const{
    auto pos  = sc_->g4Tracks().at(0).getPositionAtBoundary();//DEBUG FIXME check with below
    pos.SetE(0.);//remove time
    return pos;

    math::XYZTLorentzVectorF p = sc_->impactPoint();
    if(p.mag2()==0){//not filled
        p = sc_->g4Tracks().at(0).getPositionAtBoundary();
    }
    return p;
}



SCGausMergeVertex::SCGausMergeVertex(const SimClusterMergeWrapper* t,
        float isHighEfracThreshold, float connectThreshold):MergeVertex<SimClusterMergeWrapper>(t),
        isHighEfracThreshold_(isHighEfracThreshold),connectThreshold_(connectThreshold){
    updateIsLow();
}

bool SCGausMergeVertex::shouldConnect(const SCGausMergeVertex* rhs)const{
    if(this == rhs)
        return false;
    float s = calcScore(rhs);
    return s > connectThreshold_ && MergeVertex<SimClusterMergeWrapper>::shouldConnect(rhs);
}

void SCGausMergeVertex::updateIsLow(){
    isLow_ = true;
    for(const auto& oi:obj_){
        float eval=0;
        for(const auto& oj:obj_){
            eval += oj->gaus(oi->pos());
        }
        float frac = eval / (oi->etot()+1e-6);
        if(frac > isHighEfracThreshold_){
            isLow_=false;
            break;
        }
        if(!isLow_)
            break;
    }
}

float SCGausMergeVertex::energy(const math::XYZTLorentzVectorF& pos)const{
    float val=0;
    for(const auto sc: obj_){
        val += sc->gaus(pos);
    }
    return val;
}

float SCGausMergeVertex::energy(float x, float y)const{
    float val=0;
    for(const auto sc: obj_){
        val += sc->gaus(x,y);
    }
    return val;
}

float SCGausMergeVertex::calcScore(const SCGausMergeVertex* rhs)const{

    //evaluate gaus overlap on all sc positions
    float highgestoverlap=0;
    for(const auto sc_rhs: rhs->obj_){
        float thissog  = energy(sc_rhs->pos());
        float rhssog = rhs->energy(sc_rhs->pos());
        float contained = thissog/rhssog;
        if(highgestoverlap < contained)
            highgestoverlap=contained;
    }
    return highgestoverlap;
}

float OldRadiusMergeVertex::energy(const math::XYZTLorentzVectorF& pos)const{
    float val=0;
    for(const auto sc: obj_){
        val += sc->gaus(pos);
    }
    return val;
}

float OldRadiusMergeVertex::energy(float x, float y)const{
    float val=0;
    for(const auto sc: obj_){
        val += sc->gaus(x,y);
    }
    return val;
}

float OldRadiusMergeVertex::calcScore(const OldRadiusMergeVertex* rhs)const{
    float mindist=1e9;
    for(const auto sc: obj_){
        for(const auto sc_rhs: rhs->obj_){
            auto dpos = sc->pos()-sc_rhs->pos();
            dpos.SetE(0.);
            float dist = dpos.mag()/(sc->r() + sc_rhs->r());
            if(mindist>dist)
                mindist=dist;
        }
    }
    return 1/(mindist+1e-9);
}


bool OldRadiusMergeVertex::shouldConnect(const OldRadiusMergeVertex* rhs)const{
    if(this == rhs)
        return false;
    float s = calcScore(rhs);
    return s > connectThreshold_;
}


float RadiusMergeVertex::calcScore(const RadiusMergeVertex* rhs)const{
    float mindist=1e9;
    for(const auto sc: obj_){
        for(const auto sc_rhs: rhs->obj_){
            auto dpos = sc->pos()-sc_rhs->pos();
            dpos.SetE(0.);
            float dist = dpos.mag()/(sc->r() + sc_rhs->r());
            if(mindist>dist)
                mindist=dist;
        }
    }
    return 1/(mindist+1e-9);
}

bool RadiusMergeVertex::shouldConnect(const RadiusMergeVertex* rhs)const{
    if(this == rhs)
        return false;
    float s = calcScore(rhs);
    return s > connectThreshold_  && MergeVertex<SimClusterMergeWrapper>::shouldConnect(rhs);
}



float HitMergeVertex::connectThreshold=0.2;
float HitMergeVertex::singleClusterEfrac=0.9;

HitMergeVertex::HitMergeVertex(const SimClusterMergeWrapper* t,
        const std::vector<RecHit> & allhits,
        const std::vector<std::pair<uint32_t,float> >& hafs
):MergeVertex<SimClusterMergeWrapper>(t){

    for(size_t i=0;i<hafs.size();i++){//TBI DEBUG CMSSW adapt
        auto id = hafs.at(i).first;
        for(const auto& h: allhits){//TBI performance
            if(h.id() == id){
                hafs_.push_back(Haf(&h,hafs.at(i).second));
                break;
            }
        }
         //TBI emplace_back
    }
    //build Hafs
    std::sort(hafs_.begin(),hafs_.end());
    updateEnergies();
    //call last
    updateIsLow();
    cached_score_.first=0;
}

bool HitMergeVertex::shouldConnect(const HitMergeVertex* rhs)const{
    if(this == rhs)
        return false;
    float s = calcScore(rhs);
    return s > connectThreshold && MergeVertex<SimClusterMergeWrapper>::shouldConnect(rhs);
}

void HitMergeVertex::updateIsLow(){
    //tbi
    //use energy weighted fraction average
    float fracsum=0, ensum=0;
    for(const auto& h:hafs_){
        fracsum += h.frac * (h.frac*h.rechit->energy());
        ensum += h.frac*h.rechit->energy();
    }
    float efrac = fracsum/(ensum+1e-6);

    isLow_ = efrac < singleClusterEfrac;//DEBUG HARDCODED
}

void HitMergeVertex::absorb(HitMergeVertex* rhs){
    absorbHafs(rhs->hafs_);
    rhs->hafs_.clear();

    //re-calc total energy etc
    updateEnergies();
    rhs->updateEnergies();

    MergeVertex<SimClusterMergeWrapper>::absorb(rhs);
}


//DEBUG only
float HitMergeVertex::energy(float x, float y)const{
    float val=0;
    for(const auto sc: obj_){
        val += sc->gaus(x,y);
    }
    return val;
}

/*
 * how much if the rhs energy is contained in intersecting hits
 * union
 */
float HitMergeVertex::calcScore(const HitMergeVertex* rhs)const{
    if(!rhs->valid())
        return 0;
    if(cached_score_.first == rhs){
        cached_score_.first=0;
        return cached_score_.second;
    }
    //this can be improved using simple disance measures, but keep it for now

    std::vector<size_t>  int_a, int_b;
    intersectSorted(hafs_,rhs->hafs_,int_a,int_b);
    //use an intersection over sorted vectors here
    float score=0;
    //out of the hits that intersect with *this,
    for(size_t i=0;i<int_a.size();i++){
        const auto& h = hafs_.at(int_a.at(i));
        const auto& h_rhs = rhs->hafs_.at(int_b.at(i));
        float uE = h.rechit->energy() * (h.frac + h_rhs.frac);
        score += uE;
    }
    score /= rhs->etot_ + 1e-6; //how does rhs energy compare to the combined intersection energy

    cached_score_.first=rhs;
    cached_score_.second=score;
    //     std::cout << score << std::endl;
    return score;
}
void HitMergeVertex::updateEnergies(){
    etot_=0;
    eallhits_=0;
    for(const auto& h:hafs_){
        eallhits_ += h.rechit->energy();
        etot_+=h.rechit->energy() * h.frac;
    }
}

void HitMergeVertex::absorbHafs(const std::vector<Haf>& h_rhs){
    std::vector<size_t> sidx, rhs_sidx, _uidx, rhs_uidx;

    intersectSorted(hafs_,h_rhs, sidx, rhs_sidx, _uidx, rhs_uidx);

    //add overlapping fractions
    for(size_t i=0;i<sidx.size();i++){
        hafs_.at(sidx.at(i)).frac += h_rhs.at(rhs_sidx.at(i)).frac;
    }
    //get unique to rhs
    std::vector<Haf> rhsu;
    for(size_t i=0;i<rhs_uidx.size();i++){
        rhsu.push_back(h_rhs.at(rhs_uidx.at(i)));
    }
    //FIXME TBI use insertSorted, debug config here
    hafs_.insert(hafs_.end(),rhsu.begin(),rhsu.end());
    std::sort(hafs_.begin(),hafs_.end());

    return;
    //both are sorted FIXME TBI (to check if it's actually faster)
    insertSortedInSorted(rhsu,hafs_);
}




