/*
 * NTupleWindow.cpp
 *
 *  Created on: 26 Sep 2019
 *      Author: jkiesele
 */



#include "../interface/NTupleWindow.h"
#include "DataFormats/Math/interface/deltaR.h"

std::vector<std::vector<float>>* NTupleWindow::sp_hitFeatures_=0;
std::vector<float>* NTupleWindow::sp_recHitEnergy_;
std::vector<float>* NTupleWindow::sp_recHitEta_;
std::vector<float>* NTupleWindow::sp_recHitPhi_;
std::vector<float>* NTupleWindow::sp_recHitTheta_;
std::vector<float>* NTupleWindow::sp_recHitR_;
std::vector<float>* NTupleWindow::sp_recHitX_;
std::vector<float>* NTupleWindow::sp_recHitY_;
std::vector<float>* NTupleWindow::sp_recHitZ_;
std::vector<float>* NTupleWindow::sp_recHitDetID_;
std::vector<float>* NTupleWindow::sp_recHitTime_;
std::vector<float>* NTupleWindow::sp_recHitID_;
std::vector<float>* NTupleWindow::sp_recHitPad_;
//std::vector<float>* NTupleWindow::sp_trackFeatures_=0;

std::vector<std::vector<float> > * NTupleWindow::sp_truthHitFractions_=0;
std::vector<int>                 * NTupleWindow::sp_truthHitAssignementIdx_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedEnergies_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedX_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedY_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedZ_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedEta_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedPhi_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedT_=0;
std::vector<std::vector<int> >   * NTupleWindow::sp_truthHitAssignedPIDs_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedInner_=0;

std::vector<float>               * NTupleWindow::sp_truthHitAssignedDirX_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedDirY_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedDirZ_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedDirEta_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedDepEnergies_=0;
std::vector<float>               * NTupleWindow::sp_truthHitAssignedDirR_=0;

std::vector<int>    * NTupleWindow::sp_truthSimclusterIdx_=0;
std::vector<std::vector<int> >   * NTupleWindow::sp_truthSimclusterPIDs_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterEnergies_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterX_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterY_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterZ_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterEta_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterPhi_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterR_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterInnerWindow_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterT_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterDirX_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterDirY_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterDirZ_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterDirEta_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterDepEnergies_=0;
std::vector<float>  * NTupleWindow::sp_truthSimclusterDirR_=0;

std::vector<int>   * NTupleWindow::sp_ticlHitAssignementIdx_=0;
std::vector<float> * NTupleWindow::sp_ticlHitAssignedEnergies_=0;

float * NTupleWindow::sp_windowEta_=0;
float * NTupleWindow::sp_windowPhi_=0;

//static
std::vector<NTupleWindow> NTupleWindow::createWindows(size_t nSegmentsPhi,
            size_t nSegmentsEta, double minEta, double maxEta, double frameWidthEta,
            double frameWidthPhi){
    return WindowBase::createWindows<NTupleWindow>(nSegmentsPhi,nSegmentsEta,
            minEta,maxEta,frameWidthEta,frameWidthPhi);
}



void NTupleWindow::createTreeBranches(TTree* t){

   // NTupleWindow dummy;
   // dummy.assignTreePointers(); //so that the pointers are not null, maybe not needed? FIXME

    if (flattenRechitFeatures_) {
        t->Branch("recHitEnergy", &sp_recHitEnergy_);
        t->Branch("recHitEta", &sp_recHitEta_);
        t->Branch("recHitPhi", &sp_recHitPhi_);
        t->Branch("recHitTheta", &sp_recHitTheta_);
        t->Branch("recHitR", &sp_recHitR_);
        t->Branch("recHitX", &sp_recHitX_);
        t->Branch("recHitY", &sp_recHitY_);
        t->Branch("recHitZ", &sp_recHitZ_);
        t->Branch("recHitDetID", &sp_recHitDetID_);
        t->Branch("recHitTime", &sp_recHitTime_);
        t->Branch("recHitID", &sp_recHitID_);
        t->Branch("recHitPad", &sp_recHitPad_);
    }
    else
        t->Branch("recHitFeatures", &sp_hitFeatures_);
    //t->Branch("trackFeatures", &sp_trackFeatures_);

    t->Branch("truthHitFractions", &sp_truthHitFractions_);
    t->Branch("truthHitAssignementIdx", &sp_truthHitAssignementIdx_);
    t->Branch("truthHitAssignedEnergies", &sp_truthHitAssignedEnergies_);
    t->Branch("truthHitAssignedX", &sp_truthHitAssignedX_);
    t->Branch("truthHitAssignedY", &sp_truthHitAssignedY_);
    t->Branch("truthHitAssignedZ", &sp_truthHitAssignedZ_);
    t->Branch("truthHitAssignedEta", &sp_truthHitAssignedEta_);
    t->Branch("truthHitAssignedPhi", &sp_truthHitAssignedPhi_);
    t->Branch("truthHitAssignedT", &sp_truthHitAssignedT_);
    t->Branch("truthHitAssignedPIDs", &sp_truthHitAssignedPIDs_);
    t->Branch("truthHitAssignedInner", &sp_truthHitAssignedInner_);
    t->Branch("truthHitAssignedDirX", &sp_truthHitAssignedDirX_);
    t->Branch("truthHitAssignedDirY", &sp_truthHitAssignedDirY_);
    t->Branch("truthHitAssignedDirZ", &sp_truthHitAssignedDirZ_);
    t->Branch("truthHitAssignedDirEta", &sp_truthHitAssignedDirEta_);
    t->Branch("truthHitAssignedDepEnergies", &sp_truthHitAssignedDepEnergies_);
    t->Branch("truthHitAssignedDirR", &sp_truthHitAssignedDirR_);

    t->Branch("truthSimclusterIdx",&sp_truthSimclusterIdx_);
    t->Branch("truthSimclusterPIDs",&sp_truthSimclusterPIDs_);
    t->Branch("truthSimclusterEnergies",&sp_truthSimclusterEnergies_);
    t->Branch("truthSimclusterX",&sp_truthSimclusterX_);
    t->Branch("truthSimclusterY",&sp_truthSimclusterY_);
    t->Branch("truthSimclusterZ",&sp_truthSimclusterZ_);
    t->Branch("truthSimclusterInnerWindow",&sp_truthSimclusterInnerWindow_);
    t->Branch("truthSimclusterT",&sp_truthSimclusterT_);

    t->Branch("truthSimclusterDirX",&sp_truthSimclusterDirX_);
    t->Branch("truthSimclusterDirY",&sp_truthSimclusterDirY_);
    t->Branch("truthSimclusterDirZ",&sp_truthSimclusterDirZ_);
    t->Branch("truthSimclusterDirEta",&sp_truthSimclusterDirEta_);
    t->Branch("truthSimclusterDepEnergies",&sp_truthSimclusterDepEnergies_);
    t->Branch("truthSimclusterDirR",&sp_truthSimclusterDirR_);

    t->Branch("ticlHitAssignementIdx",  &sp_ticlHitAssignementIdx_  );
    t->Branch("ticlHitAssignedEnergies",&sp_ticlHitAssignedEnergies_);

    t->Branch("windowEta",sp_windowEta_);
    t->Branch("windowPhi",sp_windowPhi_);

}

NTupleWindow::NTupleWindow(float centerEta, float centerPhi,
        float outerRegionDEta, float outerRegionDPhi, float innerRegionDEta,
        float innerRegionDPhi) :
        WindowBase(centerEta, centerPhi, outerRegionDEta, outerRegionDPhi,
                innerRegionDEta, innerRegionDPhi) {

    windowEta_ = centerEta;
    windowPhi_ = centerPhi;
    removeFrameSimcluster_=true;
}


//static

// This is not a very clear/efficient way to do this, but it's the simplest way given
// the structure we already have
void NTupleWindow::flattenRechitFeatures() {
    for (size_t i = 0; i < hitFeatures_.size(); i++) {
        recHitEnergy_.push_back(hitFeatures_[i][kEnergy]);
        recHitEta_.push_back(hitFeatures_[i][kEta]);
        recHitTheta_.push_back(hitFeatures_[i][kTheta]);
        recHitR_.push_back(hitFeatures_[i][kR]);
        recHitX_.push_back(hitFeatures_[i][kx]);
        recHitY_.push_back(hitFeatures_[i][ky]);
        recHitZ_.push_back(hitFeatures_[i][kz]);
        recHitTime_.push_back(hitFeatures_[i][kTime]);
        recHitID_.push_back(hitFeatures_[i][kId]);

        math::XYZTLorentzVectorF v(hitFeatures_[i][kx],hitFeatures_[i][ky],hitFeatures_[i][kz],0);

        recHitPad_.push_back(0);
        recHitPhi_.push_back(v.phi());
        recHitDetID_.push_back(0);
    }
}


void NTupleWindow::assignTreePointers()  {

    sp_hitFeatures_ = &hitFeatures_;
    //sp_trackFeatures_ = &hitFeatures_;
    if (flattenRechitFeatures_) {
        sp_recHitEnergy_  = &recHitEnergy_;
        sp_recHitEta_  = &recHitEta_;
        sp_recHitPhi_  = &recHitPhi_;
        sp_recHitTheta_  = &recHitTheta_;
        sp_recHitR_  = &recHitR_;
        sp_recHitX_  = &recHitX_;
        sp_recHitY_  = &recHitY_;
        sp_recHitZ_  = &recHitZ_;
        sp_recHitDetID_  = &recHitDetID_;
        sp_recHitTime_  = &recHitTime_;
        sp_recHitID_  = &recHitID_;
        sp_recHitPad_  = &recHitPad_;
    }

    sp_truthHitFractions_ = &truthHitFractions_;
    sp_truthHitAssignementIdx_ = &truthHitAssignementIdx_;
    sp_truthHitAssignedEnergies_ = &truthHitAssignedEnergies_;
    sp_truthHitAssignedX_ = &truthHitAssignedX_;
    sp_truthHitAssignedY_ = &truthHitAssignedY_;
    sp_truthHitAssignedZ_ = &truthHitAssignedZ_;
    sp_truthHitAssignedEta_ = &truthHitAssignedEta_;
    sp_truthHitAssignedPhi_ = &truthHitAssignedPhi_;
    sp_truthHitAssignedT_ = &truthHitAssignedT_;
    sp_truthHitAssignedPIDs_ = &truthHitAssignedPIDs_;
    sp_truthHitAssignedInner_ = &truthHitAssignedInner_;
    sp_truthHitAssignedDirX_ = &truthHitAssignedDirX_;
    sp_truthHitAssignedDirY_ = &truthHitAssignedDirY_;
    sp_truthHitAssignedDirZ_ = &truthHitAssignedDirZ_;
    sp_truthHitAssignedDirEta_ = &truthHitAssignedDirEta_;
    sp_truthHitAssignedDepEnergies_ = &truthHitAssignedDepEnergies_;
    sp_truthHitAssignedDirR_ = &truthHitAssignedDirR_;

    sp_truthSimclusterIdx_ = &truthSimclusterIdx_;
    sp_truthSimclusterPIDs_ = &truthSimclusterPIDs_;
    sp_truthSimclusterEnergies_ = &truthSimclusterEnergies_;
    sp_truthSimclusterX_ = &truthSimclusterX_;
    sp_truthSimclusterY_ = &truthSimclusterY_;
    sp_truthSimclusterZ_ = &truthSimclusterZ_;
    sp_truthSimclusterEta_ = &truthSimclusterEta_;
    sp_truthSimclusterPhi_ = &truthSimclusterPhi_;
    sp_truthSimclusterR_ = &truthSimclusterR_;
    sp_truthSimclusterInnerWindow_ = &truthSimclusterInnerWindow_;
    sp_truthSimclusterT_ = &truthSimclusterT_;

    sp_truthSimclusterDirX_=&truthSimclusterDirY_;
    sp_truthSimclusterDirY_=&truthSimclusterDirX_;
    sp_truthSimclusterDirZ_=&truthSimclusterDirZ_;
    sp_truthSimclusterDirEta_=&truthSimclusterDirEta_;
    sp_truthSimclusterDepEnergies_=&truthSimclusterDepEnergies_;
    sp_truthSimclusterDirR_=&truthSimclusterDirR_;


    sp_ticlHitAssignementIdx_  = &ticlHitAssignementIdx_;
    sp_ticlHitAssignedEnergies_= &ticlHitAssignedEnergies_;

    sp_windowEta_ = &windowEta_;
    sp_windowPhi_ = &windowPhi_;

}



void NTupleWindow::clear(){
    WindowBase::clear(); //clears rechits etc

    detIDHitAsso_.clear();

    hitFeatures_.clear();
    recHitEnergy_.clear();
    recHitEta_.clear();
    recHitPhi_.clear();
    recHitTheta_.clear();
    recHitR_.clear();
    recHitX_.clear();
    recHitY_.clear();
    recHitZ_.clear();
    recHitDetID_.clear();
    recHitTime_.clear();
    recHitID_.clear();
    recHitPad_.clear();

    truthHitFractions_.clear();
    truthHitAssignementIdx_.clear();
    truthHitAssignedEnergies_.clear();
    truthHitAssignedX_.clear();
    truthHitAssignedY_.clear();
    truthHitAssignedZ_.clear();
    truthHitAssignedEta_.clear();
    truthHitAssignedPhi_.clear();
    truthHitAssignedT_.clear();
    truthHitAssignedPIDs_.clear();
    truthHitAssignedInner_.clear();
    truthHitAssignedDirX_.clear();
    truthHitAssignedDirY_.clear();
    truthHitAssignedDirZ_.clear();
    truthHitAssignedDirEta_.clear();
    truthHitAssignedDepEnergies_.clear();
    truthHitAssignedDirR_.clear();

    truthSimclusterIdx_.clear();
    truthSimclusterPIDs_.clear();
    truthSimclusterEnergies_.clear();
    truthSimclusterX_.clear();
    truthSimclusterY_.clear();
    truthSimclusterZ_.clear();
    truthSimclusterEta_.clear();
    truthSimclusterPhi_.clear();
    truthSimclusterR_.clear();
    truthSimclusterInnerWindow_.clear();
    truthSimclusterT_.clear();

    truthSimclusterDirY_.clear();
    truthSimclusterDirX_.clear();
    truthSimclusterDirZ_.clear();
    truthSimclusterDirEta_.clear();
    truthSimclusterDepEnergies_.clear();
    truthSimclusterDirR_.clear();


    ticlHitAssignementIdx_.clear();
    ticlHitAssignedEnergies_.clear();

}

void NTupleWindow::fillFeatureArrays(){
    //NO CUTS HERE!
    WindowBase::fillFeatureArrays();
    createDetIDHitAssociation();

}

void NTupleWindow::fillTruthArrays(){

    if(ticlHitAssignementIdx_.size() < 1 || recHitEnergy_.size() < 1)
        throw std::runtime_error("NTupleWindow::fillTruthArrays: first fill all reco quantities including ticl");

    auto hits_to_keep = cleanSimclusters();
    calculateSimclusterFeatures();
    calculateTruthFractions();
    fillTruthAssignment();

    bool keep_all=true;
    for(const auto k:hits_to_keep){
        if(!k)
            keep_all=false;
        if(!keep_all)
            break;
    }
    if(keep_all)
        return;
    //clean up after all assignments have been done
    //hitFeatures_=keepIndices(hitFeatures_,hits_to_keep);         //
    recHitEnergy_=keepIndices(recHitEnergy_,hits_to_keep);       //recHitEnergy_.clear();
    recHitEta_=keepIndices(recHitEta_,hits_to_keep);             //recHitEta_.clear();
    recHitPhi_=keepIndices(recHitPhi_,hits_to_keep);             //recHitPhi_.clear();
    recHitTheta_=keepIndices(recHitTheta_,hits_to_keep);         //recHitTheta_.clear();
    recHitR_=keepIndices(recHitR_,hits_to_keep);                 //recHitR_.clear();
    recHitX_=keepIndices(recHitX_,hits_to_keep);                 //recHitX_.clear();
    recHitY_=keepIndices(recHitY_,hits_to_keep);                 //recHitY_.clear();
    recHitZ_=keepIndices(recHitZ_,hits_to_keep);                 //recHitZ_.clear();
    recHitDetID_=keepIndices(recHitDetID_,hits_to_keep);         //recHitDetID_.clear();
    recHitTime_=keepIndices(recHitTime_,hits_to_keep);           //recHitTime_.clear();
    recHitID_=keepIndices(recHitID_,hits_to_keep);               //recHitID_.clear();
    recHitPad_=keepIndices(recHitPad_,hits_to_keep);             //recHitPad_.clear();


    truthHitFractions_=keepIndices(truthHitFractions_,hits_to_keep);                       //truthHitFractions_.clear();
    truthHitAssignementIdx_=keepIndices(truthHitAssignementIdx_,hits_to_keep);             //truthHitAssignementIdx_.clear();
    truthHitAssignedEnergies_=keepIndices(truthHitAssignedEnergies_,hits_to_keep);         //truthHitAssignedEnergies_.clear();
    truthHitAssignedX_=keepIndices(truthHitAssignedX_,hits_to_keep);                       //truthHitAssignedX_.clear();
    truthHitAssignedY_=keepIndices(truthHitAssignedY_,hits_to_keep);                       //truthHitAssignedY_.clear();
    truthHitAssignedZ_=keepIndices(truthHitAssignedZ_,hits_to_keep);                       //truthHitAssignedZ_.clear();
    truthHitAssignedEta_=keepIndices(truthHitAssignedEta_,hits_to_keep);                   //truthHitAssignedEta_.clear();
    truthHitAssignedPhi_=keepIndices(truthHitAssignedPhi_,hits_to_keep);                   //truthHitAssignedPhi_.clear();
    truthHitAssignedT_=keepIndices(truthHitAssignedT_,hits_to_keep);                       //truthHitAssignedT_.clear();
    truthHitAssignedPIDs_=keepIndices(truthHitAssignedPIDs_,hits_to_keep);                 //truthHitAssignedPIDs_.clear();
    truthHitAssignedInner_=keepIndices(truthHitAssignedInner_,hits_to_keep);               //truthHitAssignedInner_.clear();
    truthHitAssignedDirX_=keepIndices(truthHitAssignedDirX_,hits_to_keep);                 //truthHitAssignedDirX_.clear();
    truthHitAssignedDirY_=keepIndices(truthHitAssignedDirY_,hits_to_keep);                 //truthHitAssignedDirY_.clear();
    truthHitAssignedDirZ_=keepIndices(truthHitAssignedDirZ_,hits_to_keep);                 //truthHitAssignedDirZ_.clear();
    truthHitAssignedDirEta_=keepIndices(truthHitAssignedDirEta_,hits_to_keep);             //truthHitAssignedDirEta_.clear();
    truthHitAssignedDepEnergies_=keepIndices(truthHitAssignedDepEnergies_,hits_to_keep);   //truthHitAssignedDepEnergies_.clear();
    truthHitAssignedDirR_=keepIndices(truthHitAssignedDirR_,hits_to_keep);                 //truthHitAssignedDirR_.clear();

    ticlHitAssignementIdx_=keepIndices(ticlHitAssignementIdx_,hits_to_keep);
    ticlHitAssignedEnergies_=keepIndices(ticlHitAssignedEnergies_,hits_to_keep);
}

void NTupleWindow::createDetIDHitAssociation(){
    detIDHitAsso_.clear();

    for(size_t i=0;i<recHits.size();i++){
        detIDHitAsso_[recHits.at(i)->hit->detid()]={i,1.};
    }
    for(size_t i=0;i<tracks_.size();i++){
        detIDHitAsso_[-1]={i+recHits.size(),1.};
    }
}

void NTupleWindow::calculateSimclusterFeatures(){

    truthSimclusterIdx_.clear();
    truthSimclusterPIDs_.clear();
    truthSimclusterEnergies_.clear();
    truthSimclusterX_.clear();
    truthSimclusterY_.clear();
    truthSimclusterZ_.clear();
    truthSimclusterEta_.clear();
    truthSimclusterPhi_.clear();
    truthSimclusterR_.clear();
    truthSimclusterInnerWindow_.clear();
    truthSimclusterT_.clear();
    truthSimclusterDirX_.clear();
    truthSimclusterDirY_.clear();
    truthSimclusterDirZ_.clear();
    truthSimclusterDirEta_.clear();
    truthSimclusterDepEnergies_.clear();
    truthSimclusterDirR_.clear();



    for(size_t i=0;i<simClusters_.size();i++){
        truthSimclusterIdx_.push_back(i);
        truthSimclusterPIDs_.push_back(pdgToOneHot(simClusters_.at(i)->pdgId()));
        const math::XYZTLorentzVectorF& scimpactpoint = simClusters_.at(i)->impactPoint();
        truthSimclusterX_.push_back(scimpactpoint.X());
        truthSimclusterY_.push_back(scimpactpoint.Y());
        truthSimclusterZ_.push_back(scimpactpoint.Z());
        truthSimclusterEta_.push_back(scimpactpoint.Eta());
        truthSimclusterPhi_.push_back(scimpactpoint.Phi());
        truthSimclusterR_.push_back(scimpactpoint.R());

        truthSimclusterInnerWindow_.push_back(simClustersInnerWindow_.at(i) ? 1 : 0);

        truthSimclusterT_.push_back(simClusters_.at(i)->impactPoint().T());
        const math::XYZTLorentzVectorF& scimpactmom = simClusters_.at(i)->impactMomentum();
        truthSimclusterEnergies_.push_back(simClusters_.at(i)->p4().E());
        truthSimclusterDirX_.push_back(scimpactmom.X());
        truthSimclusterDirY_.push_back(scimpactmom.Y());
        truthSimclusterDirZ_.push_back(scimpactmom.Z());
        truthSimclusterDirEta_.push_back(scimpactmom.Eta());
        truthSimclusterDirR_.push_back(scimpactmom.R());

        double dep_energy=0;
        const auto& hitsandfracs = simClusters_.at(i)->hits_and_fractions();
        for(const auto& haf: hitsandfracs){
            auto pos = detIDHitAsso_.find(haf.first);
            if(pos == detIDHitAsso_.end()) //edges or not included in layer clusters
                continue;
            int idx = pos->second.first;
            float totalfrac = pos->second.second * haf.second;
            dep_energy += recHitEnergy_.at(idx) * totalfrac;
        }
        truthSimclusterDepEnergies_.push_back(dep_energy);

    }
}

//needs function to match truth tracks

void NTupleWindow::calculateTruthFractions(){

    truthHitFractions_.clear();
    truthHitFractions_.resize(hitFeatures_.size(),
            std::vector<float>(simClusters_.size(), 0)); //includes tracks

    for (size_t i_sc = 0; i_sc < simClusters_.size(); i_sc++) {
        const auto& hitsandfracs = simClusters_.at(i_sc)->hits_and_fractions();
        for(const auto& haf: hitsandfracs){
            auto pos = detIDHitAsso_.find(haf.first);
            if(pos == detIDHitAsso_.end()) //edges or not included in layer clusters
                continue;
            size_t idx = pos->second.first;
            float totalfrac = pos->second.second * haf.second;
            truthHitFractions_.at(idx).at(i_sc) += totalfrac; //can be more than 1-1 for layer clusters
        }
    }

    //associate the tracks here, such that they look like hits, simple matching
    size_t trackStartIterator = recHits.size();


    ////match, will be improved by direct truth matching in new simclusters on longer term
    ////assumption: for every track there is a simcluster, given the below-MIP thresholds in the HGCal
    ///*
    // *
    // * this is just a temporary solution until a proper simcluster-simtrack integration exists
    // *
    // */
    //std::vector<size_t> usedSimclusters;

    //return ;

    std::vector<bool> hassc(tracks_.size(),false);

    const double momentumscaler = 0;//as long as it's unreliable
    std::vector<bool> scused(simClusters_.size(),false);
    for(float minDistance=0.005;minDistance<=0.02;minDistance+=0.005){//rather strict
        for(size_t i_t=0;i_t<tracks_.size();i_t++){

            //double minDistance=0.03;

            size_t matchedSCIdx=simClusters_.size();

            for(size_t i_sc=0;i_sc<simClusters_.size();i_sc++){
                //needs improvement
                if(scused.at(i_sc))
                    continue;

                double scEnergy = simClusters_.at(i_sc)->impactMomentum().E();
                double trackMomentum = tracks_.at(i_t)->obj->p();
                double distance = reco::deltaR(simClusters_.at(i_sc)->impactPoint().Eta(),
                        simClusters_.at(i_sc)->impactPoint().Phi(),
                        (float)tracks_.at(i_t)->pos.eta(),
                        (float)tracks_.at(i_t)->pos.phi()) +
                                momentumscaler*std::abs(scEnergy - trackMomentum)/(scEnergy);

                if(distance<minDistance){
                    matchedSCIdx=i_sc;
                    minDistance=distance;
                }
            }
            if(matchedSCIdx<simClusters_.size()){
                hassc.at(i_t)=true;
                truthHitFractions_.at(i_t+trackStartIterator).at(matchedSCIdx) = 1.;
                scused.at(matchedSCIdx)=true;
            }
        }
    }
    int debug_ntrackwithSC=0;
    for(const auto& hsc:hassc)
        if(hsc)
            debug_ntrackwithSC++;
    DEBUGPRINT(debug_ntrackwithSC);
    DEBUGPRINT(debug_ntrackwithSC/(float)tracks_.size());
}


void NTupleWindow::fillTruthAssignment(){

    truthHitAssignementIdx_.resize(truthHitFractions_.size());
    truthHitAssignedEnergies_.resize(truthHitFractions_.size());
    truthHitAssignedX_.resize(truthHitFractions_.size());
    truthHitAssignedY_.resize(truthHitFractions_.size());
    truthHitAssignedZ_.resize(truthHitFractions_.size());
    truthHitAssignedEta_.resize(truthHitFractions_.size());
    truthHitAssignedPhi_.resize(truthHitFractions_.size());
    truthHitAssignedT_.resize(truthHitFractions_.size());

    std::vector<int> pids(n_particle_types,0);
    pids.at((int)type_ambiguous) = 1;//default
    truthHitAssignedPIDs_.resize(truthHitFractions_.size(), pids);

    truthHitAssignedInner_.resize(truthHitFractions_.size());
    truthHitAssignedDirX_.resize(truthHitFractions_.size());
    truthHitAssignedDirY_.resize(truthHitFractions_.size());
    truthHitAssignedDirZ_.resize(truthHitFractions_.size());
    truthHitAssignedDirEta_.resize(truthHitFractions_.size());
    truthHitAssignedDepEnergies_.resize(truthHitFractions_.size());
    truthHitAssignedDirR_.resize(truthHitFractions_.size());


    bool nosim = simClusters_.size() < 1;

    for (size_t i_hit = 0; i_hit < truthHitFractions_.size(); i_hit++) {

        bool allzero = std::all_of(truthHitFractions_.at(i_hit).begin(),
                truthHitFractions_.at(i_hit).end(), [](float i) {return i==0;});

        if(allzero || nosim){
            truthHitAssignementIdx_.at(i_hit) = -1;
            continue;
        }
        size_t maxfrac_idx = std::max_element(
                truthHitFractions_.at(i_hit).begin(),
                truthHitFractions_.at(i_hit).end())
        - truthHitFractions_.at(i_hit).begin();

        truthHitAssignementIdx_.at(i_hit) = maxfrac_idx;
        truthHitAssignedEnergies_.at(i_hit) = truthSimclusterEnergies_.at(
                maxfrac_idx);
        truthHitAssignedX_.at(i_hit) = truthSimclusterX_.at(maxfrac_idx);
        truthHitAssignedY_.at(i_hit) = truthSimclusterY_.at(maxfrac_idx);
        truthHitAssignedZ_.at(i_hit) = truthSimclusterZ_.at(maxfrac_idx);
        truthHitAssignedEta_.at(i_hit) = truthSimclusterEta_.at(maxfrac_idx);
        truthHitAssignedPhi_.at(i_hit) = truthSimclusterPhi_.at(maxfrac_idx);
        truthHitAssignedT_.at(i_hit) = truthSimclusterT_.at(maxfrac_idx);
        truthHitAssignedPIDs_.at(i_hit) = truthSimclusterPIDs_.at(maxfrac_idx);
        truthHitAssignedDirX_.at(i_hit) = truthSimclusterDirX_.at(maxfrac_idx);
        truthHitAssignedDirY_.at(i_hit) = truthSimclusterDirY_.at(maxfrac_idx);
        truthHitAssignedDirZ_.at(i_hit) = truthSimclusterDirZ_.at(maxfrac_idx);
        truthHitAssignedDirEta_.at(i_hit) = truthSimclusterDirEta_.at(maxfrac_idx);
        truthHitAssignedDepEnergies_.at(i_hit) = truthSimclusterDepEnergies_.at(maxfrac_idx);
        truthHitAssignedDirR_.at(i_hit) = truthSimclusterDirR_.at(maxfrac_idx);
        truthHitAssignedInner_.at(i_hit) = truthSimclusterInnerWindow_.at(maxfrac_idx);

    }
}


std::vector<bool> NTupleWindow::cleanSimclusters(){

    auto rh_energycopy = recHitEnergy_;


    //clean everything that was NOT assigned (for whatever reason) to this window
    for(const auto& sc: badSimClusters_){
        float energy=0;
        size_t nhits=0;
        const auto& hitsandfracs = sc->hits_and_fractions();
        for(const auto& haf: hitsandfracs){
            auto pos = detIDHitAsso_.find(haf.first);
            if(pos == detIDHitAsso_.end()) //edges or not included in layer clusters
                continue;
            size_t idx = pos->second.first;
            double thisenergy = rh_energycopy.at(idx) * pos->second.second * haf.second;
            recHitEnergy_.at(idx) -= thisenergy;
            energy += thisenergy;
            nhits++;
            if(recHitEnergy_.at(idx)<=1e-9){
                recHitEnergy_.at(idx)=0;
            }
        }
        std::cout << "removed bad sc at eta "  <<  sc->impactPoint().Eta() << ", phi "<< sc->impactPoint().Phi()
                << ", depo energy " << energy <<" in "<<nhits<<" hits"<<", impact en "<< sc->impactMomentum().E()<< std::endl;
    }

    std::cout << "removing hits..." << std::endl;

    std::vector<bool> hits_to_keep;

    for(size_t i=0;i<recHitEnergy_.size();i++){
        if(recHitEnergy_.at(i)>0)
            hits_to_keep.push_back(true);
        else
            hits_to_keep.push_back(false);
    }

    return hits_to_keep;

    // do not remove bare rechits! they are needed with original index later
//    recHits = keepIndices(recHits,hits_to_keep);




    //simclusters are gone and also their energy is gone
}

void NTupleWindow::fillTiclAssignment(){//last


    ticlHitAssignementIdx_.resize(recHitEnergy_.size(), -1);
    ticlHitAssignedEnergies_.resize(recHitEnergy_.size(), -1);


    for (size_t it=0;it<ticltracksters_.size();it++) {

        for(const auto& ID: ticltracksters_.at(it)->assohits){
            auto pos = detIDHitAsso_.find(ID);
            if(pos == detIDHitAsso_.end()) //not in window
                continue;
            size_t idx = pos->second.first;
            ticlHitAssignementIdx_.at(idx)=it;
            ticlHitAssignedEnergies_.at(idx)=ticltracksters_.at(it)->energy;
        }
    }

}

