/*
 * WindowBase.cpp
 *
 *  Created on: 26 Sep 2019
 *      Author: jkiesele
 */




#include "FWCore/Utilities/interface/Exception.h"
#include "../interface/WindowBase.h"



WindowBase::WindowBase(float centerEta, float centerPhi, float outerRegionDEta, float outerRegionDPhi,
        float innerRegionDEta, float innerRegionDPhi) :

        mode_(useLayerClusters),
        centerEta_(centerEta),centerPhi_(centerPhi),
        outerRegionDEta_(outerRegionDEta),outerRegionDPhi_(outerRegionDPhi),
        innerRegionDEta_(innerRegionDEta), innerRegionDPhi_(
                innerRegionDPhi) {

    //sanity checks FIXME: add more
    if (innerRegionDEta_ <= 0 || innerRegionDPhi_ <= 0) {
        throw cms::Exception("IncorrectWindowParameters")
                << "innerRegionDEta,innerRegionDPhi  must be > 0";
    }
    if (innerRegionDEta_ > outerRegionDEta_ || innerRegionDPhi_ > outerRegionDPhi_) {
        throw cms::Exception("IncorrectWindowParameters")
                << "innerRegionDEta,innerRegionDPhi  must be <= outerRegionDEta, outerRegionDPhi";
    }
    if(nTrackFeatures_ != nRechitFeatures_ || nTrackFeatures_ != nLayerClusterFeatures_){
        throw cms::Exception("IncorrectWindowParameters")
                        << "number of track, rechit and layer cluster features must be the same";
    }

}


WindowBase::~WindowBase() {
    clear();
}


void WindowBase::clear() {
    // this class does not own anything
    tracks_.clear();
    recHits.clear();
    layerClusters_.clear();
    simClusters_.clear();
    badSimClusters_.clear();
    ticltracksters_.clear();
    hitFeatures_.clear();
}



//// private ////

/*
 * recHitEnergy,
   recHitEta   ,
   recHitID, #indicator if it is track or not
   recHitTheta ,
   recHitR   ,
   recHitX     ,
   recHitY     ,
   recHitZ     ,
   recHitTime
 */


const size_t WindowBase::nTrackFeatures_=9;
void WindowBase::fillTrackFeatures(float*& data, const TrackWithHGCalPos * t) const {
    *(data++) = t->obj->p();
    *(data++) = t->pos.eta();
    *(data++) = -1.; //"detid"
    *(data++) = t->pos.theta();
    *(data++) = t->pos.mag();
    *(data++) = t->pos.x();
    *(data++) = t->pos.y();
    *(data++) = t->pos.z();
    *(data++) = t->obj->chi2();
}

/*
 * recHitEnergy,
   recHitEta   ,
   recHitID, #indicator if it is track or not
   recHitTheta ,
   recHitR   ,
   recHitX     ,
   recHitY     ,
   recHitZ     ,
   recHitTime
 */

const size_t WindowBase::nRechitFeatures_=9;
void WindowBase::fillRecHitFeatures(float*& data, const HGCRecHitWithPos * recHit) const {
    *(data++) = recHit->hit->energy();
    *(data++) = recHit->pos.eta();
    *(data++) = 0; //ID track or not
    *(data++) = recHit->pos.theta();
    *(data++) = recHit->pos.mag();
    *(data++) = recHit->pos.x();
    *(data++) = recHit->pos.y();
    *(data++) = recHit->pos.z();
    *(data++) = recHit->hit->time();
}


const size_t WindowBase::nLayerClusterFeatures_=9;
void WindowBase::fillLayerClusterFeatures(float*& data, const reco::CaloCluster * cl) const {
    throw std::runtime_error("WindowBase::fillLayerClusterFeatures: LC not supported anymore");
}

void WindowBase::fillFeatureArrays(){
    //NO CUTS HERE!

    hitFeatures_.clear();
    if(getMode() == useRechits){
        for(const auto& rh:recHits){
            std::vector<float> feats(nRechitFeatures_);
            auto data = &feats.at(0);
            fillRecHitFeatures(data,rh);
            hitFeatures_.push_back(feats);
        }
    }
    else{
        for(const auto& lc: layerClusters_){
            std::vector<float> feats(nLayerClusterFeatures_);
            auto data = &feats.at(0);
            fillLayerClusterFeatures(data,lc);
            hitFeatures_.push_back(feats);
        }
    }

    //return;
    //add tracks LAST!
    for(const auto& tr:tracks_){
        std::vector<float> feats(nTrackFeatures_);
        auto data = &feats.at(0);
        fillTrackFeatures(data,tr);
        hitFeatures_.push_back(feats);
    }



}

WindowBase::particle_type WindowBase::pdgToParticleType(int pdgid)const{

    /*
     * enum particle_type{
        type_ambiguous,
        type_electron,
        type_photon,
        type_mip,
        type_charged_hadron,
        type_neutral_hadron,
        n_particle_types
    };
     */
    if(pdgid == 0)
        return type_ambiguous;
    if(pdgid == 13 || pdgid == -13)
        return type_electron;
    if(pdgid == 22)
        return type_photon;
    if(pdgid == 13 || pdgid == -13)
        return type_mip;
    if (pdgid == 211 || pdgid == -211 || pdgid == 321 || pdgid == -321) //to be extended
        return type_charged_hadron;

    return type_neutral_hadron;

}

std::vector<int> WindowBase::particleTypeToOneHot(particle_type ptype) const{
    std::vector<int> out((int)n_particle_types,0);
    out.at((int)ptype) = 1.;
    return out;
}

WindowBase::particle_type WindowBase::oneHotToParticleType(const std::vector<int>& v) const{
    int idx=-1;
    for(int i=0;i<(int)v.size();i++){
        if(v.at(i)){
            idx=i;
            break;
        }
    }
    if(idx >= (int)n_particle_types)
        throw std::out_of_range("WindowBase::oneHotToParticleType: input vector has wrong size");
    return (particle_type)idx;
}

WindowBase::particle_type WindowBase::predictionToParticleType(const float * pred) const{
    int maxidx=-1;
    float maxval=-1;
    for(int i=0;i<(int)n_particle_types;i++){
        if(maxval<pred[i]){
            maxval=pred[i];
            maxidx=i;
        }
    }
    return (particle_type)maxidx;
}


std::vector<int> WindowBase::pdgToOneHot(int pdgid) const{
    return particleTypeToOneHot(pdgToParticleType(pdgid) );
}



void WindowBase::printDebug()const{
     DEBUGPRINT(centerPhi_);
     DEBUGPRINT(centerEta_);
     DEBUGPRINT(outerRegionDEta_);
     DEBUGPRINT(outerRegionDPhi_);
     DEBUGPRINT(innerRegionDEta_);
     DEBUGPRINT(innerRegionDPhi_);
     std::cout << "coverage phi " << centerPhi_-outerRegionDPhi_ << "| " << centerPhi_-innerRegionDPhi_ << "[  :" <<
             centerPhi_ << ":  ]" << centerPhi_+innerRegionDPhi_ << " |" << centerPhi_ + outerRegionDPhi_ << std::endl;
     std::cout << "coverage eta " << centerEta_-outerRegionDEta_ << "| " << centerEta_-innerRegionDEta_ << "[  :" <<
             centerEta_ << ":  ]" << centerEta_+innerRegionDEta_ << " |" << centerEta_+outerRegionDEta_ << std::endl;
}



/// static




