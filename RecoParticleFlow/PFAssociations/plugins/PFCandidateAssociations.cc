#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/CaloRecHit/interface/CaloRecHit.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFraction.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/OneToManyWithQualityGeneric.h"

#include "FWCore/Utilities/interface/transform.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include <set>

//
// class decleration
//
// TODO: Does this work with an edm::View ?
typedef edm::AssociationMap<edm::OneToManyWithQualityGeneric<
    edm::View<CaloRecHit>, reco::PFCandidateCollection, float>> CaloRecHitToPFCandidate;
typedef edm::AssociationMap<edm::OneToManyWithQualityGeneric<
    edm::View<CaloRecHit>, reco::PFClusterCollection, float>> CaloRecHitToPFCluster;

class PFCandAssociationsProducer : public edm::stream::EDProducer<> {
public:
  explicit PFCandAssociationsProducer(const edm::ParameterSet&);
  ~PFCandAssociationsProducer() override;

private:
  void produce(edm::Event&, const edm::EventSetup&) override;

  std::vector<edm::InputTag> caloRechitTags_;
  std::vector<edm::EDGetTokenT<edm::View<CaloRecHit>>> caloRechitTokens_;
  std::vector<edm::InputTag> pfClusterTags_;
  std::vector<edm::EDGetTokenT<reco::PFClusterCollection>> pfClusterTokens_;
  std::vector<edm::InputTag> trackTags_;
  std::vector<edm::EDGetTokenT<edm::View<reco::Track>>> trackTokens_;
  edm::EDGetTokenT<reco::PFCandidateCollection> pfCandCollectionToken_;
  // TODO: read from configuration
  const std::unordered_map<std::string, std::string> rechitNameToPFClusName_ = {
	{"hbhereco", "particleFlowClusterHCAL"},
	{"hfreco", "particleFlowClusterHF"},
  };
};

PFCandAssociationsProducer::PFCandAssociationsProducer(const edm::ParameterSet& pset)
    : caloRechitTags_(pset.getParameter<std::vector<edm::InputTag>>("caloRecHits")),
      caloRechitTokens_(edm::vector_transform(
          caloRechitTags_, [this](const edm::InputTag& tag) { return consumes<edm::View<CaloRecHit>>(tag); })),
      pfClusterTags_(pset.getParameter<std::vector<edm::InputTag>>("pfClusters")),
      pfClusterTokens_(edm::vector_transform(
          pfClusterTags_, [this](const edm::InputTag& tag) { return consumes<reco::PFClusterCollection>(tag); })),
      trackTags_(pset.getParameter<std::vector<edm::InputTag>>("tracks")),
      trackTokens_(edm::vector_transform(
          trackTags_, [this](const edm::InputTag& tag) { return consumes<edm::View<reco::Track>>(tag); })),
      pfCandCollectionToken_(consumes<reco::PFCandidateCollection>(pset.getParameter<edm::InputTag>("pfCands"))) {
  for (auto& tag : pfClusterTags_) {
    const std::string& label = !tag.instance().empty() ? tag.instance() : tag.label();
    produces<edm::Association<reco::PFCandidateCollection>>(label + "ToPFCand");
  }
  // TODO: Make OneToMany associations for rechits
  for (auto& tag : caloRechitTags_) {
     const std::string& label = !tag.instance().empty() ? tag.instance() : tag.label();
     produces<CaloRecHitToPFCandidate>(label+"ToPFCand");
     // TODO: this is kind of a pain because you need to match rechits to the right PFCluster collections
	 if (rechitNameToPFClusName_.find(label) != std::end(rechitNameToPFClusName_))
       produces<CaloRecHitToPFCluster>(label+"To"+rechitNameToPFClusName_.at(label));
  }
  for (auto& tag : trackTags_) {
    const std::string& label = !tag.instance().empty() ? tag.instance() : tag.label();
    produces<edm::Association<reco::PFCandidateCollection>>(label + "ToPFCand");
  }
}

PFCandAssociationsProducer::~PFCandAssociationsProducer() {}

//
// member functions
//

// ------------ method called to produce the data  ------------
void PFCandAssociationsProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  edm::Handle<reco::PFCandidateCollection> pfCandCollection;
  iEvent.getByToken(pfCandCollectionToken_, pfCandCollection);

  std::unordered_map<std::string, size_t> clusterNameToID;
  std::unordered_map<size_t, std::vector<int>> clusterIDToCandMatchIdx;
  std::vector<edm::Handle<reco::PFClusterCollection>> clusterHands;
  for (size_t i = 0; i < pfClusterTokens_.size(); i++) {
    clusterHands.push_back({});
    auto& hand = clusterHands[i];
    iEvent.getByToken(pfClusterTokens_[i], hand);
    size_t prodId = hand.id().id();
    auto& tag = pfClusterTags_[i];
    const std::string& label = !tag.instance().empty() ? tag.instance() : tag.label();
    clusterNameToID[label] = prodId;
    clusterIDToCandMatchIdx[prodId] = std::vector<int>(hand->size(), -1);
  }

  std::map<size_t, std::vector<int>> trackToPFCandIndices;
  std::vector<edm::Handle<edm::View<reco::Track>>> trackHands;
  for (size_t i = 0; i < trackTokens_.size(); i++) {
	trackHands.push_back({});
	auto& hand = trackHands.back();
	iEvent.getByToken(trackTokens_[i], hand);	
    size_t prodId = hand.id().id();
	trackToPFCandIndices[prodId] = std::vector<int>(hand->size(), -1);
  }

  std::unordered_map<size_t, edm::RefToBase<CaloRecHit>> detIdToRecHit;
  std::unordered_map<size_t, std::unique_ptr<CaloRecHitToPFCandidate>> recHitColToCandAssoc;
  std::unordered_map<size_t, std::unique_ptr<CaloRecHitToPFCluster>> recHitColToClusterAssoc;
  std::vector<size_t> rhColIds;
  for (size_t i = 0; i < caloRechitTokens_.size(); i++) {
    edm::Handle<edm::View<CaloRecHit>> rhHand;
    iEvent.getByToken(caloRechitTokens_[i], rhHand);
	rhColIds.push_back(rhHand.id().id());
    recHitColToCandAssoc[rhColIds.back()] = std::make_unique<CaloRecHitToPFCandidate>(rhHand, pfCandCollection);
    for (size_t j = 0; j < rhHand->size(); j++) {
      edm::RefToBase<CaloRecHit> ch(rhHand, j);
      detIdToRecHit[ch->detid().rawId()] = ch;
    }
    auto& tag = caloRechitTags_[i];
    const std::string& label = !tag.instance().empty() ? tag.instance() : tag.label();
	if (rechitNameToPFClusName_.find(label) != std::end(rechitNameToPFClusName_)) {
	  size_t clusterId = clusterNameToID[rechitNameToPFClusName_.at(label)];
	  auto clusterHandIt = std::find_if(std::begin(clusterHands), std::end(clusterHands), 
	    [clusterId](auto& h) { return h.id().id() == clusterId; });
	  if (clusterHandIt != std::end(clusterHands)) {
	    recHitColToClusterAssoc[rhColIds.back()] = std::make_unique<CaloRecHitToPFCluster>(rhHand, *clusterHandIt);
	  }
    }
  }

  for (size_t j = 0; j < pfCandCollection->size(); j++) {
    reco::PFCandidateRef pfCandRef(pfCandCollection, j);
    const reco::PFCandidate::ElementsInBlocks& elements = pfCandRef->elementsInBlocks();
    for (auto& element : elements) {
      // ElementInBlock, first is the ref, second is the index (of blocks in the cand)
      const reco::PFBlockRef blockRef = element.first;
      if (!blockRef.isNonnull())
          continue;
	  // TODO: Add tracks
      for (const auto& block : blockRef->elements()) {
        const reco::TrackRef trackRef = block.trackRef();
        if (trackRef.isNonnull()) {
			size_t prodId = trackRef.id().id();
			if (trackToPFCandIndices.find(prodId) != std::end(trackToPFCandIndices)) {
				trackToPFCandIndices[prodId][trackRef.key()] = j;
			}
        }
        const reco::PFClusterRef cluster = block.clusterRef();
        if (cluster.isNonnull()) {
          size_t prodId = cluster.id().id();
          if (clusterIDToCandMatchIdx.find(prodId) != std::end(clusterIDToCandMatchIdx)) {
            auto& idxvec = clusterIDToCandMatchIdx[prodId];
            // Length of vector matches length of cluster collection, ith element should point to index of
            // containing PFCand, i.e., j here
            idxvec[cluster.key()] = j;
          }

          // TODO: Make one-to-many association of hit to cluster and cand. Need to handle all the rechit
          // products
          const std::vector<reco::PFRecHitFraction>& rhf = cluster->recHitFractions();
          for (const auto& hf : rhf) {
            auto pfRecHit = hf.recHitRef();
            auto rechit = detIdToRecHit[pfRecHit->detId()];
            size_t prodId = rechit.id().id();
            if (recHitColToCandAssoc.find(prodId) != std::end(recHitColToCandAssoc)) {
              recHitColToCandAssoc[prodId]->insert(rechit, std::make_pair(pfCandRef, hf.fraction()));
            }
            if (recHitColToClusterAssoc.find(prodId) != std::end(recHitColToClusterAssoc)) {
              recHitColToClusterAssoc[prodId]->insert(rechit, std::make_pair(cluster, hf.fraction()));
			}	
          }
        }
      }
    }
  }

  for (size_t i = 0; i < caloRechitTags_.size(); i++) {
    auto& tag = caloRechitTags_[i];
    const std::string& label = !tag.instance().empty() ? tag.instance() : tag.label();
	size_t rhId = rhColIds[i];
	auto pfCandAssocEntry = recHitColToCandAssoc.find(rhId);
	if (pfCandAssocEntry != std::end(recHitColToCandAssoc)) {
      iEvent.put(std::move(pfCandAssocEntry->second), label+"ToPFCand");
	}

	auto pfClusAssocEntry = recHitColToClusterAssoc.find(rhId);
    if (pfClusAssocEntry != std::end(recHitColToClusterAssoc)) {
      iEvent.put(std::move(pfClusAssocEntry->second), label+"To"+rechitNameToPFClusName_.at(label));
    }
  }

  for (size_t i = 0; i < pfClusterTags_.size(); i++) {
    auto clusToCand = std::make_unique<edm::Association<reco::PFCandidateCollection>>(pfCandCollection);
    edm::Association<reco::PFCandidateCollection>::Filler filler(*clusToCand);
    auto& tag = pfClusterTags_[i];
    const std::string& label = !tag.instance().empty() ? tag.instance() : tag.label();
    auto& indices = clusterIDToCandMatchIdx[clusterNameToID[label]];
    filler.insert(clusterHands[i], indices.begin(), indices.end());
    filler.fill();
    iEvent.put(std::move(clusToCand), label+"ToPFCand");
  }

  for (size_t i = 0; i < trackTags_.size(); i++) {
	auto& hand = trackHands[i];
	auto& indices = trackToPFCandIndices[hand.id().id()];
    auto trackToCand = std::make_unique<edm::Association<reco::PFCandidateCollection>>(pfCandCollection);
    edm::Association<reco::PFCandidateCollection>::Filler filler(*trackToCand);
    auto& tag = trackTags_[i];
    const std::string& label = !tag.instance().empty() ? tag.instance() : tag.label();
    filler.insert(trackHands[i], indices.begin(), indices.end());
    filler.fill();
    iEvent.put(std::move(trackToCand), label+"ToPFCand");
  }

}

// define this as a plug-in
DEFINE_FWK_MODULE(PFCandAssociationsProducer);
