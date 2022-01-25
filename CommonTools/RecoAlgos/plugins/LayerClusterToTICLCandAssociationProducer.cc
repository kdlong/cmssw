//
// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHit.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "DataFormats/HGCalReco/interface/TICLCandidate.h"

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/OneToManyWithQualityGeneric.h"
#include "DataFormats/Common/interface/RefToBase.h"


#include "FWCore/Utilities/interface/transform.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include <set>

//
// class decleration
//
typedef edm::AssociationMap<edm::OneToManyWithQualityGeneric<
    HGCRecHitCollection, TICLCandidate, float>> RecHitToTICLCands;
typedef edm::AssociationMap<edm::OneToManyWithQualityGeneric<
    reco::CaloCluster, TICLCandidate, float>> LayerClusterToTICLCands;

class LayerClusterToTICLCandidateAssociationProducer : public edm::stream::EDProducer<> {
public:
  explicit LayerClusterToTICLCandidateAssociationProducer(const edm::ParameterSet&);
  ~LayerClusterToTICLCandidateAssociationProducer() override;

private:
  void produce(edm::Event&, const edm::EventSetup&) override;

  edm::EDGetTokenT<HGCRecHitCollection> caloRechitCollectionToken_;
  edm::EDGetTokenT<edm::Association<std::vector<reco::CaloCluster>> rhToLCToken_;
  edm::EDGetTokenT<TICLCandidateCollection> ticlCandToken_;
};

LayerClusterToTICLCandidateAssociationProducer::LayerClusterToTICLCandidateAssociationProducer(const edm::ParameterSet& pset)
    : caloRechitCollectionToken_(pset.getParameter<edm::InputTag>("caloRecHits")),
      ticlCandToken_(consumes<std::vector<reco::CaloCluster>>(pset.getParameter<edm::InputTag>("layerClusters"))),
      rhToLCToken_(consumes<edm::Association<std::vector<reco::CaloCluster>>>(pset.getParameter<edm::InputTag>("recHitToLC"))),
      ticlCandToken_(consumes<TICLCandidateCollection>(pset.getParameter<edm::InputTag>("ticlTrackstersMerge"))) {
  produces<RecHitToTICLCands>();
  produces<LayerClusterToTICLCands>();
}

LayerClusterToTICLCandidateAssociationProducer::~LayerClusterToTICLCandidateAssociationProducer() {}

// ------------ method called to produce the data  ------------
void LayerClusterToTICLCandidateAssociationProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  edm::Handle<TICLCandidateCollection> ticlCands;
  iEvent.getByToken(ticlCandsToken_, ticlCands);
  edm::Handle<HGCRecHitCollection> caloRechitCollection;
  iEvent.getByToken(caloRechitCollectionToken, caloRechitCollection);
  edm::Handle<std::vector<reco::CaloCluster>> layerClusterCollection;
  iEvent.getByToken(layerClusterToken, layerClusterCollection);
  edm::Handle<edm::Asocciation<std::vector<reco::CaloCluster>>> rhToLCAssoc;
  iEvent.getByToken(rhToLCToken_, rhToLCAssoc);

  auto lcAssocMap = std::make_unique<LayerClusterToSimCluster>(layerClusterCollection, ticlCands);
  auto rhAssocMap = std::make_unique<RecHitToSimCluster>(caloRechitCollection, ticlCands);

  for (size_t t = 0; t < ticlCands->size(); t++) {
    TICLCandidateRec ticlCand(ticlCands, t);
    for (const auto trackster : ticlCand->tracksters()) {
      size_t i = 0;
      for (int lcidx : trackster.vertices()) {
          float frac = 1.f/tracster.vertex_multiplicities()[i];
          reco::CaloClusterRef lc(lcidx, layerClusters);
          lcAssocMap->insert(ticlCand, std::make_pair(lc, frac));
          // Store favorite hit match here, put into edm::Association
          // Make a vector of the indices of the matching layerclusters
      }
    }
  }


  for (size_t h = 0; h < caloRechitCollection->size(); h++) {
    HGCRecHitRef caloRh(caloRechitCollection, h);
    auto lcRef = rhToLCAssoc[caloRh]
    if (lcRef.isNonnull())
        // TODO: Check if it's inside
        auto& candMatches = lcAssocMap[lcRef];
        for (auto& match : candMatches)
            rhAssocMap->insert(caloRh, match);
        // Probably should read from the info stored above for layer clusters
        // to make an edm::Association of the best match here
  }

  auto assoc = std::make_unique<edm::Association<SimClusterCollection>>(scCollection);
  edm::Association<SimClusterCollection>::Filler filler(*assoc);
  filler.insert(caloRechitCollection, rechitIndices.begin(), rechitIndices.end());
  filler.fill();
  //iEvent.put(std::move(assoc), label + "ToBestSimClus");
  iEvent.put(std::move(assocMap));
}

// define this as a plug-in
DEFINE_FWK_MODULE(LayerClusterToTICLCandidateAssociationProducer);

