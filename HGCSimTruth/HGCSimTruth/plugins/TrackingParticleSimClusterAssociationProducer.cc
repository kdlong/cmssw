// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/Associations/interface/TrackToTrackingParticleAssociator.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"

#include "FWCore/Utilities/interface/EDGetToken.h"
#include <set>

//
// class decleration
//

typedef edm::AssociationMap<edm::OneToMany<
    TrackingParticleCollection, SimClusterCollection>> TrackingParticleToSimCluster;

class TrackingParticleSimClusterAssociationProducer : public edm::stream::EDProducer<> {
public:
  explicit TrackingParticleSimClusterAssociationProducer(const edm::ParameterSet &);
  ~TrackingParticleSimClusterAssociationProducer() override;

private:
  virtual void produce(edm::Event&, const edm::EventSetup&) override;
  std::set<TrackingParticleRef> findTrackingParticleMatch(
        std::unordered_map<unsigned int, TrackingParticleRef>& trackIdToTPRef, SimClusterRef scRef) const;

  edm::EDGetTokenT<TrackingParticleCollection> tpCollectionToken_;
  edm::EDGetTokenT<SimClusterCollection> scCollectionToken_;
};

TrackingParticleSimClusterAssociationProducer::TrackingParticleSimClusterAssociationProducer(const edm::ParameterSet &pset)
    : tpCollectionToken_(consumes<TrackingParticleCollection>(pset.getParameter<edm::InputTag>("trackingParticles"))),
        scCollectionToken_(consumes<SimClusterCollection>(pset.getParameter<edm::InputTag>("simClusters")))
{
  produces<TrackingParticleToSimCluster>();
}

TrackingParticleSimClusterAssociationProducer::~TrackingParticleSimClusterAssociationProducer() {}

//
// member functions
//

std::set<TrackingParticleRef> TrackingParticleSimClusterAssociationProducer::findTrackingParticleMatch(
        std::unordered_map<unsigned int, TrackingParticleRef>& trackIdToTPRef, SimClusterRef scRef) const {
    std::set<TrackingParticleRef> trackingParticles;
    for (auto& track : scRef->g4Tracks()) {
        unsigned int trackId = track.trackId();
        if (trackIdToTPRef.find(trackId) != trackIdToTPRef.end())
            trackingParticles.insert(trackIdToTPRef[trackId]); 
    }
    return trackingParticles;
}

// ------------ method called to produce the data  ------------
void TrackingParticleSimClusterAssociationProducer::produce(edm::Event &iEvent, const edm::EventSetup &iSetup) {

  edm::Handle<TrackingParticleCollection> tpCollection;
  iEvent.getByToken(tpCollectionToken_, tpCollection);

  edm::Handle<SimClusterCollection> scCollection;
  iEvent.getByToken(scCollectionToken_, scCollection);

  auto out = std::make_unique<TrackingParticleToSimCluster>(tpCollection, scCollection);
  std::unordered_map<unsigned int, TrackingParticleRef> trackIdToTPRef;

  for (size_t i = 0; i < tpCollection->size(); i++) {
      TrackingParticleRef trackingParticle(tpCollection, i);
      for (auto& track : trackingParticle->g4Tracks()) {
          unsigned int trackId = track.trackId();
          trackIdToTPRef[trackId] = trackingParticle;
      }
  }

  // NOTE: not every trackingparticle will be in the association.
  // could add empty SCs in this case, but that might be worse...
  for (size_t i = 0; i < scCollection->size(); i++) {
      SimClusterRef simclus(scCollection, i);
      for (auto tp : findTrackingParticleMatch(trackIdToTPRef, simclus))
          out->insert(tp, simclus);
  }

  iEvent.put(std::move(out));
}

// define this as a plug-in
DEFINE_FWK_MODULE(TrackingParticleSimClusterAssociationProducer);
