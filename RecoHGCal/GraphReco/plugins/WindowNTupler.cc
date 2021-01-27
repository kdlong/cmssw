// -*- C++ -*-
//
// Package:    RecoHGCal/WindowNTupler
// Class:      WindowNTupler
//
/**\class WindowNTupler WindowNTupler.cc RecoHGCal/GraphReco/plugins/WindowNTupler.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Jan Kieseler
//         Created:  Tue, 27 Aug 2019 17:25:47 GMT
//
//



// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "../interface/NTupleWindow.h"
#include "DataFormats/ParticleFlowReco/interface/HGCalMultiCluster.h"
#include "HGCSimTruth/HGCSimTruth/interface/SimClusterTools.h"

#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"

#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "DataFormats/Common/interface/OneToMany.h"
#include "DataFormats/Common/interface/AssociationMap.h"

#include <algorithm>
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


using reco::TrackCollection;
typedef edm::AssociationMap<edm::OneToMany<
    TrackingParticleCollection, SimClusterCollection>> TrackingParticleToSimCluster;

class WindowNTupler : public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources>  {
   public:
      explicit WindowNTupler(const edm::ParameterSet&);
      ~WindowNTupler();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

      virtual void beginRun(edm::Run const &iEvent, edm::EventSetup const &) override;
      virtual void endRun(edm::Run const &iEvent, edm::EventSetup const &) override;

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;



      Tracksterwithpos_and_energy assignPositionAndEnergy(const ticl::Trackster& ,
              const reco::CaloClusterCollection& )const;

      // ----------member data ---------------------------
      edm::EDGetTokenT<std::vector<ticl::Trackster>> tracksters_token_;
      edm::EDGetTokenT<edm::View<reco::Track>> tracksToken_;
      edm::EDGetTokenT<reco::CaloClusterCollection> layerClustersToken_;
      edm::EDGetTokenT<std::vector<SimCluster>> simClusterToken_;
      std::vector<edm::EDGetTokenT<HGCRecHitCollection> > rechitsTokens_;
      //edm::EDGetTokenT<TrackingParticleCollection> trackingPartToken_;
      edm::EDGetTokenT<reco::RecoToSimCollection> tracksToTrackingPartToken_;
      edm::EDGetTokenT<TrackingParticleToSimCluster> trackingPartToSimClusToken_;

      std::vector<NTupleWindow> windows_;
      hgcal::RecHitTools recHitTools_;
      SimClusterTools sctools_;
      HGCalTrackPropagator trackprop_;
      edm::Service<TFileService> fs_;
      TTree * outTree_;


};


Tracksterwithpos_and_energy WindowNTupler::assignPositionAndEnergy(const ticl::Trackster& trackster,
        const reco::CaloClusterCollection& caloclusters)const{

    std::array<float, 3> baricenter{{0., 0., 0.}};
    float total_weight = 0.;
    std::vector<size_t> assohits;
    if (!trackster.vertices().empty()) {

        int counter = 0;
        std::for_each(std::begin(trackster.vertices()), std::end(trackster.vertices()), [&](unsigned int idx) {
            auto fraction = 1.f / trackster.vertex_multiplicity(counter++);
            auto weight = caloclusters.at(idx).energy() * fraction;
            total_weight += weight;
            baricenter[0] += caloclusters.at(idx).x() * weight;
            baricenter[1] += caloclusters.at(idx).y() * weight;
            baricenter[2] += caloclusters.at(idx).z() * weight;

            auto haf = caloclusters.at(idx).hitsAndFractions();
            for(const auto& hf:haf){
                assohits.push_back(hf.first);
            }

        });
        std::transform(
                std::begin(baricenter), std::end(baricenter), std::begin(baricenter), [&total_weight](double val) -> double {
            return val / total_weight;
        });
    }
//Tracksterwithpos r=
    auto pos = GlobalPoint(baricenter[0], baricenter[1], baricenter[2]);
    return {&trackster,pos ,total_weight,assohits};

}

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
WindowNTupler::WindowNTupler(const edm::ParameterSet& config)
 :
  tracksters_token_(consumes<std::vector<ticl::Trackster>>(config.getParameter<edm::InputTag>("tracksters"))),
  tracksToken_(consumes<edm::View<reco::Track>>(config.getParameter<edm::InputTag>("tracks"))),
  layerClustersToken_(consumes<reco::CaloClusterCollection>(config.getParameter<edm::InputTag>("layerClusters"))),
  simClusterToken_(consumes<std::vector<SimCluster>>(config.getParameter<edm::InputTag>("simClusters"))),
  //trackingPartToken_(consumes<TrackingParticleCollection>(config.getParameter<edm::InputTag>("trackingParticles"))),
  tracksToTrackingPartToken_(consumes<reco::RecoToSimCollection>(config.getParameter<edm::InputTag>("tracksToTrackingParticles"))),
  trackingPartToSimClusToken_(consumes<TrackingParticleToSimCluster>(config.getParameter<edm::InputTag>("trackingParticleSimCluster"))),
  outTree_(nullptr)

/* ... */

{
    for (edm::InputTag& recHitCollection : config.getParameter<
            std::vector<edm::InputTag> >("recHitCollections")) {
        rechitsTokens_.push_back(
                consumes<HGCRecHitCollection>(recHitCollection));
    }

    //DEBUG INFO: has checks built in
    windows_ = NTupleWindow::createWindows(
            (size_t) config.getParameter<uint32_t>("nPhiSegments"),
            (size_t) config.getParameter<uint32_t>("nEtaSegments"),
            config.getParameter<double>("minEta"),
            config.getParameter<double>("maxEta"),
            config.getParameter<double>("etaFrameWidth"),
            config.getParameter<double>("phiFrameWidth"));

    for(auto& w: windows_){
        w.setMode(WindowBase::useRechits);//FIXME make configurable
        w.setRemoveFrameSimclusters(true);
    }
        //w.setMode(WindowBase::useLayerClusters);//FIXME make configurable

}


WindowNTupler::~WindowNTupler()
{
}


//
// member functions
//

// ------------ method called for each event  ------------
void
WindowNTupler::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   auto inlayerclusters = iEvent.get(layerClustersToken_);
   auto insimclusters = iEvent.get(simClusterToken_);

   std::vector<Tracksterwithpos_and_energy> tracksters;
   auto intracksters = iEvent.get(tracksters_token_);
   for(const auto& t: intracksters){
       auto twpe = assignPositionAndEnergy(t, inlayerclusters);
       tracksters.push_back(twpe);
   }



   reco::RecoToSimCollection tracksToTrackingParticles = iEvent.get(tracksToTrackingPartToken_);
   TrackingParticleToSimCluster trackingPartToSimClus = iEvent.get(trackingPartToSimClusToken_);
   
   //get and propagate tracks
   std::vector<TrackWithHGCalPos> proptracks;
   edm::Handle<edm::View<reco::Track>> tracksHandle;
   iEvent.getByToken(tracksToken_, tracksHandle);
   std::vector<int> trackTruthIdx(tracksHandle->size(), -2);
   for(size_t i = 0; i < tracksHandle->size(); i++){
       edm::RefToBase<reco::Track> track(tracksHandle, i);
       if(fabs(track->eta())< 1.5 || fabs(track->eta())>3.0) 
           continue; //just use potentially interesting ones
       proptracks.push_back(trackprop_.propagateObject(*track));
       // I know try/catch is a bit ugly, but I don't know if there is another
       // mechanism to see if the track has matched tracking particle. This is 
       // what they do in https://github.com/cms-sw/cmssw/blob/master/SimTracker/TrackAssociation/test/testTrackAssociator.cc#L64-L78
       std::vector<std::pair<TrackingParticleRef, double>> trackingPartsWithQual;
       try {
            trackingPartsWithQual = tracksToTrackingParticles[track]; 
       }
       catch (edm::Exception const &) {
           // No trackingParticle match, this is a fake
           trackTruthIdx.at(i) = -1;
           continue;
       } 
       for (auto& tpWithQual : trackingPartsWithQual) {
           TrackingParticleRef trackingPart = tpWithQual.first;
           SimClusterRefVector assocSimClusters;
           try  {
               assocSimClusters = trackingPartToSimClus[trackingPart];
           }
           catch (edm::Exception const &) { 
               // Give a unique ID, not matching any simClusters
               trackTruthIdx.at(i) = insimclusters.size()+i;
               continue;
           }
           
           // This should be a loop, but need to figure out how to assign to SC.
           // Maybe also need to group multiple SCs together if they share a track
           SimClusterRef simClusRef = assocSimClusters.at(0); 
           trackTruthIdx.at(i) = simClusRef.key();
       }
   }
   std::cout << "The size of the in simclusters is " << insimclusters.size() << std::endl;
   for (auto i : trackTruthIdx)
       std::cout << "Track truth index is " << i << std::endl;

   //get rechits, get positions and merge collections
   std::vector<HGCRecHitWithPos> allrechits;
   for (auto & token : rechitsTokens_) {
       for (const auto& rh : iEvent.get(token)) {
           HGCRecHitWithPos rhp = { const_cast<HGCRecHit*>(&rh), recHitTools_.getPosition(rh.detid()) };
           allrechits.push_back(rhp);
       }
   }
   std::sort(allrechits.begin(), allrechits.end(), 
        [](const HGCRecHitWithPos& rh1, const HGCRecHitWithPos& rh2) 
            { return rh1.hit->energy() > rh2.hit->energy();});




   std::vector<size_t> filledtracksters(tracksters.size(),0);
   std::vector<size_t> filledtrack(proptracks.size(),0);
   std::vector<size_t> filledrechits(allrechits.size(),0);
   std::vector<size_t> filledlayercluster(inlayerclusters.size(),0);

   //rechits
   //filledrechits vector etc

   for(auto& window : windows_){
       //attach tracks, rehits etc to windows
       for(size_t it=0;it<proptracks.size();it++) {
           if(filledtrack.at(it)>3) continue;
           if(window.maybeAddTrack(proptracks.at(it)))
               filledtrack.at(it)++;
       }
       for(size_t it=0;it<allrechits.size();it++) {
           if(filledrechits.at(it)>3) continue;
           if(window.maybeAddRecHit(allrechits.at(it)))
               filledrechits.at(it)++;
       }
       if(window.getMode() == WindowBase::useLayerClusters){
           for(size_t it=0;it<inlayerclusters.size();it++) {
               if(filledlayercluster.at(it)>3) continue;
               if(window.maybeAddLayerCluster(inlayerclusters.at(it)))
                   filledlayercluster.at(it)++;
           }
       }
       for(size_t it=0;it<tracksters.size();it++){
           if(filledtracksters.at(it)>3) continue;
           if(window.maybeAddTrackster(tracksters.at(it)))
               filledtracksters.at(it)++;
       }

       for(size_t it=0;it<insimclusters.size();it++) {
           //not here to catch some strays if(filledsimclusters.at(it)>3) continue;
           window.maybeAddSimCluster(insimclusters.at(it),sctools_.isHGCal(insimclusters.at(it)));

       }

       //the follwing will not work yet before everything is filled
       window.fillFeatureArrays();
       window.flattenRechitFeatures();
       window.fillTiclAssignment();


       window.fillTruthArrays();

       window.assignTreePointers();

       outTree_->Fill();
       window.clear(); //free memory
   }



}


// ------------ method called once each job just before starting event loop  ------------
void WindowNTupler::beginJob() {

    if (!fs_)
        throw edm::Exception(edm::errors::Configuration,
                "TFile Service is not registered in cfg file");

    outTree_ = fs_->make<TTree>("tree", "tree");
    NTupleWindow::createTreeBranches(outTree_);

}

// ------------ method called once each job just after ending the event loop  ------------
void
WindowNTupler::endJob()
{
}

void WindowNTupler::beginRun(edm::Run const &iEvent, edm::EventSetup const &es) {
  trackprop_ .getEventSetup(es);
  sctools_.setRechitTools(recHitTools_);
}
void WindowNTupler::endRun(edm::Run const &iEvent, edm::EventSetup const &es) {

}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
WindowNTupler::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(WindowNTupler);
