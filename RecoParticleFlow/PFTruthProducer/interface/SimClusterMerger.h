
#ifndef RECOPARTICLEFLOW_PFTRUTHPRODUCER_INTERFACE_SIMCLUSTERMERGER_H_
#define RECOPARTICLEFLOW_PFTRUTHPRODUCER_INTERFACE_SIMCLUSTERMERGER_H_

#include "SCMergeTools.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHit.h"

/*
 * Contains differernt incarnations for simcluster merging, based on SCMergeTools and SimHistory
 */


class SimClusterMergeWrapper{
public:
    SimClusterMergeWrapper(const SimCluster* sc,
            float e, float etot, float r,size_t idx):sc_(sc),r_(r), e_(e), etot_(etot),idx_(idx){
        pos_ = calcBoundaryPos(sc);
    };

    // TBI
    inline float gaus(const math::XYZTLorentzVectorF& rhs)const{
        float distsq = LVDistSq(pos_,rhs);
        return e_ * exp(-distsq/(2.*r_*r_));
    }

    //for validation mostly
    inline float gaus(float x, float y)const{
        return gaus(math::XYZTLorentzVectorF(x,y,pos_.z(),0));
    }

    const math::XYZTLorentzVectorF& pos()const{return pos_;}
    float x()const{return pos_.x();}
    float y()const{return pos_.y();}
    float z()const{return pos_.z();}

    const SimCluster* SC()const{return sc_;}

    const float& r()const{return r_;}
    const float& e()const{return e_;}
    const float& etot()const{return etot_;}
    const size_t& idx()const{return idx_;}

    void setEtot(float etot){etot_=etot;}//needs to be adapted later

private:
    //helper until simtrack<->sc boundary treatment is non ambiguous
    // TBI
    math::XYZTLorentzVectorF calcBoundaryPos(const SimCluster* sc)const;

    //helper to avoid minkowski metric for .Mag()
    inline float LVDistSq(const math::XYZTLorentzVectorF& a, const math::XYZTLorentzVectorF& b) const {
      return (a.x() - b.x()) * (a.x() - b.x()) +
             (a.y() - b.y()) * (a.y() - b.y()) +
             (a.z() - b.z()) * (a.z() - b.z()) +
             (a.t() - b.t()) * (a.t() - b.t());
    }

    const SimCluster * sc_;
    //additional information for merging
    float r_,e_,etot_;
    size_t idx_;
    math::XYZTLorentzVectorF pos_;//cached for simplicity
};



class SCGausMergeVertex: public MergeVertex<SimClusterMergeWrapper>{
public:

    SCGausMergeVertex(const SimClusterMergeWrapper* t,
            float isHighEfracThreshold, float connectThreshold);

    bool shouldConnect(const SCGausMergeVertex* rhs)const;

    void updateIsLow();

    float energy(const math::XYZTLorentzVectorF& pos)const;
    float energy(float x, float y)const;

    inline float z()const{
        if(valid())
            return objects().at(0)->z();//DEBUG
        return 0;
    }

    float calcScore(const SCGausMergeVertex* rhs)const;

protected:
    float isHighEfracThreshold_;
    float connectThreshold_;

};



class OldRadiusMergeVertex: public MergeVertex<SimClusterMergeWrapper>{
public:
    OldRadiusMergeVertex(const SimClusterMergeWrapper* t,
            float isHighEfracThreshold, float connectThreshold):MergeVertex<SimClusterMergeWrapper>(t),connectThreshold_(connectThreshold){
        updateIsLow();
    }

    void updateIsLow(){
        isLow_ = true;
    }
    float energy(const math::XYZTLorentzVectorF& pos)const;
    float energy(float x, float y)const;

    float calcScore(const OldRadiusMergeVertex* rhs)const;

    bool shouldConnect(const OldRadiusMergeVertex* rhs)const;

private:
    float connectThreshold_;
};

class RadiusMergeVertex: public SCGausMergeVertex{
public:
    RadiusMergeVertex(const SimClusterMergeWrapper* t,
            float isHighEfracThreshold, float connectThreshold):SCGausMergeVertex(t,isHighEfracThreshold,connectThreshold){
        updateIsLow();
    }

    float calcScore(const RadiusMergeVertex* rhs)const;

    bool shouldConnect(const RadiusMergeVertex* rhs)const;

};


class HitMergeVertex: public MergeVertex<SimClusterMergeWrapper>{
public:

    using RecHit = HGCRecHit;

    HitMergeVertex(const SimClusterMergeWrapper* t,
            const std::vector<RecHit> & allhits,
            const std::vector<std::pair<uint32_t,float> >& hafs
            );


    bool shouldConnect(const HitMergeVertex* rhs)const;

    void updateIsLow();

    void absorb(HitMergeVertex* rhs);


    inline float z()const{
        if(valid())
            return objects().at(0)->z();//DEBUG
        return 0;
    }

    //DEBUG only
    float energy(float x, float y)const;

    /*
     * how much if the rhs energy is contained in intersecting hits
     * union
     */
    float calcScore(const HitMergeVertex* rhs)const;

    static float connectThreshold;
    static float singleClusterEfrac;

private:

    void updateEnergies();

    class Haf{
    public:
        Haf(const RecHit * h, float f):rechit(h),frac(f){}

        const RecHit * rechit;
        float frac;

        //for sorting by ID (just performance)
        bool operator == (const Haf& rhs)const{return rechit->id() == rhs.rechit->id();}
        bool operator < (const Haf& b) const{
            return rechit->id()<b.rechit->id();
        }
    };

    std::vector<Haf> hafs_;

    mutable std::pair<const HitMergeVertex* , float> cached_score_;

    //total energy of this vertex, using fractions
    float etot_;

    //total energy of all hits assigned to this vertex, not including fractions
    float eallhits_;

    void absorbHafs(const std::vector<Haf>& h_rhs);
};


#endif
