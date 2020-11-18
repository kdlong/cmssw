/*
 * CMSSW module to create the trees used for the HH2bbtautau analysis.
 * Disclainer: This prototype is not meant for creating an official PR. Major refactoring ahead.
 */

#include <memory>

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ProducerBase.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalDetId/interface/HcalTestNumbering.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"

#include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticleFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "SimDataFormats/CaloTest/interface/HGCalTestNumbering.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"

#include "SimGeneral/MixingModule/interface/DigiAccumulatorMixMod.h"
#include "SimGeneral/MixingModule/interface/DigiAccumulatorMixModFactory.h"
#include "SimGeneral/MixingModule/interface/PileUpEventPrincipal.h"
#include "SimGeneral/TrackingAnalysis/interface/EncodedTruthId.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/HcalCommonData/interface/HcalHitRelabeller.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "RecoHGCal/GraphReco/interface/HGCalParticlePropagator.h"



class CaloParticleSimClusterMerger : public edm::stream::EDProducer<> {
public:
  static void fillDescriptions(edm::ConfigurationDescriptions&);

  explicit CaloParticleSimClusterMerger(const edm::ParameterSet&);
  ~CaloParticleSimClusterMerger();

  virtual void beginStream(edm::StreamID) override;
  virtual void endStream() override;
  virtual void produce(edm::Event&, const edm::EventSetup&) override;



private:

  edm::EDGetTokenT<std::vector<CaloParticle>> caloParticleToken_;
  edm::EDGetTokenT<std::vector<SimVertex>> simVertexToken_;
  edm::EDGetTokenT<std::vector<SimCluster>> simClusterToken_;

  //just for debugging
  std::vector<edm::EDGetTokenT<HGCRecHitCollection>> recHitTokens_;

  HGCalParticlePropagator prop_;

};

void CaloParticleSimClusterMerger::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<edm::InputTag>("caloParticleCollection", edm::InputTag("mix", "MergedCaloTruth"));
  desc.add<edm::InputTag>("simVertexCollection", edm::InputTag("g4SimHits"));
  desc.add<edm::InputTag>("simClusterCollection", edm::InputTag("mix", "MergedCaloTruth"));

  desc.add<std::vector<edm::InputTag> >("recHitCollections",
                                         {
                                             edm::InputTag("HGCalRecHit", "HGCEERecHits"),
                                             edm::InputTag("HGCalRecHit", "HGCHEFRecHits"),
                                             edm::InputTag("HGCalRecHit", "HGCHEBRecHits"),
                                         });

  descriptions.add("CaloParticleSimClusterMerger", desc);
}

CaloParticleSimClusterMerger::CaloParticleSimClusterMerger(const edm::ParameterSet& params)
    :
      caloParticleToken_(
          consumes<std::vector<CaloParticle>>(params.getParameter<edm::InputTag>("caloParticleCollection"))),
          simVertexToken_(consumes<std::vector<SimVertex>>(params.getParameter<edm::InputTag>("simVertexCollection"))),
          simClusterToken_(consumes<std::vector<SimCluster>>(params.getParameter<edm::InputTag>("simClusterCollection")))
       {

    for (auto& recHitCollection : params.getParameter<std::vector<edm::InputTag>>("recHitCollections")) {
        recHitTokens_.push_back(consumes<HGCRecHitCollection>(recHitCollection));
    }

  produces<std::vector<SimCluster>>();                // SimClusters
}

CaloParticleSimClusterMerger::~CaloParticleSimClusterMerger() {}

void CaloParticleSimClusterMerger::beginStream(edm::StreamID) {}

void CaloParticleSimClusterMerger::endStream() {}


double getSimClusterEnergy(const SimCluster& sc, const std::vector<const HGCRecHit*>& allrechits,
        const std::unordered_map<DetId, size_t>& detid_to_rh_index){

    double totalscen=0;
    auto haf = sc.hits_and_fractions();
    for(const auto hf:haf){
        auto pos = detid_to_rh_index.find(hf.first);
        if(pos == detid_to_rh_index.end()) //edges or not included in layer clusters
            continue;
        size_t idx = pos->second;
        totalscen += hf.second * allrechits.at(idx)->energy();
    }
    return totalscen;
}

void CaloParticleSimClusterMerger::produce(edm::Event& event, const edm::EventSetup& setup) {


  std::cout << "CaloParticleSimClusterMerger::produce" << std::endl;

  prop_.setEventSetup(setup);

  // create unique pointers for output products
  std::unique_ptr<std::vector<SimCluster>> outSimClusters = std::make_unique<std::vector<SimCluster>>();


  edm::Handle<std::vector<CaloParticle>> caloParticleHandle;
  event.getByToken(caloParticleToken_, caloParticleHandle);
  CaloParticleCollection caloParticles(*caloParticleHandle);

  edm::Handle<std::vector<SimVertex>> simVertexHandle;
  event.getByToken(simVertexToken_, simVertexHandle);
  std::vector<SimVertex> simVertices(*simVertexHandle);

  edm::Handle<std::vector<SimCluster>> simClusterHandle;
  event.getByToken(simClusterToken_, simClusterHandle);
  std::vector<SimCluster> simClusters(*simClusterHandle);

  size_t ntotal=0;

  for(const auto& cp: caloParticles){


      for(const auto& g4t: cp.g4Tracks()){
          std::cout << "g4t " << g4t.genpartIndex() << " " << g4t.type()  <<std::endl;
      }

      auto firstSimTrack = cp.g4Tracks().front();
      int vertID = firstSimTrack.vertIndex();

      auto vertex = simVertices.at(vertID);

      math::XYZTLorentzVectorF momentum (firstSimTrack.momentum().x(),
              firstSimTrack.momentum().y(),firstSimTrack.momentum().z(),
              firstSimTrack.momentum().t());

      math::XYZTLorentzVectorF impact(vertex.position().x(),
              vertex.position().y(),vertex.position().z(),
              vertex.position().t());

      prop_.propagate(impact,momentum,firstSimTrack.charge());

      SimCluster newsc(firstSimTrack);

      newsc.setImpactMomentum(momentum);
      newsc.setImpactPoint(impact);


      //for(const auto& haf: cp.hits_and_fractions()){
      //    //not working
      //}
      for(const auto& sc_ref: cp.simClusters()){
          ntotal++;
          auto haf = sc_ref->hits_and_fractions();
          if(newsc.hits_and_fractions().size()<1){
              for(const auto& hf: haf)
                  newsc.addRecHitAndFraction(hf.first,hf.second);
          }
          else{
              for(const auto& hf: haf)
                  newsc.addDuplicateRecHitAndFraction(hf.first,hf.second);
          }
      }

      outSimClusters->emplace_back(newsc);

  }


  std::cout << "total simclusters: " << simClusters.size() << " associated to calo particles " << ntotal<< std::endl;

  event.put(std::move(outSimClusters));


  /////DEBUG
  std::vector<const HGCRecHit*> allrechits;
  std::unordered_map<DetId, size_t> detid_to_rh_index;
  size_t rhindex = 0;
  for (auto& token : recHitTokens_) {
      for (const auto& rh : event.get(token)) {
          detid_to_rh_index[rh.detid()] = rhindex;
          rhindex++;
          allrechits.push_back(&rh);
      }
  }

  double totalen=0;
  for(const auto& sc: simClusters)
      totalen+=getSimClusterEnergy(sc,allrechits,detid_to_rh_index);

  double totalcaloassoen=0;
  for(const auto& cp: caloParticles){
      for(const auto& sc: cp.simClusters()){
          totalcaloassoen+=getSimClusterEnergy(*sc,allrechits,detid_to_rh_index);
      }
  }

  std::cout << "total associated energy "<< totalcaloassoen << " total SC energy " << totalen
          << " fraction associated " << totalcaloassoen/totalen * 100. << std::endl;





}


DEFINE_FWK_MODULE(CaloParticleSimClusterMerger);
