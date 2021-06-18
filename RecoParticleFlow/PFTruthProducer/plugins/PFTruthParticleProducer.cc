// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/Associations/interface/TrackToTrackingParticleAssociator.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/PFAnalysis/interface/PFTruthParticle.h"
#include "SimDataFormats/PFAnalysis/interface/PFTruthParticleFwd.h"

#include "FWCore/Utilities/interface/EDGetToken.h"
#include <set>

//
// class decleration
//

typedef edm::AssociationMap<edm::OneToMany<
    TrackingParticleCollection, SimClusterCollection>> TrackingParticleToSimCluster;

class PFTruthParticleProducer : public edm::global::EDProducer<> {
public:
  explicit PFTruthParticleProducer(const edm::ParameterSet &);
  ~PFTruthParticleProducer() override;

private:
  void produce(edm::StreamID, edm::Event &, const edm::EventSetup &) const override;
  std::set<TrackingParticleRef> findTrackingParticleMatch(
        std::unordered_map<unsigned int, TrackingParticleRef>& trackIdToTPRef, SimClusterRef scRef) const;

  edm::EDGetTokenT<TrackingParticleCollection> tpCollectionToken_;
  edm::EDGetTokenT<SimClusterCollection> scCollectionToken_;
};

PFTruthParticleProducer::PFTruthParticleProducer(const edm::ParameterSet &pset)
    : tpCollectionToken_(consumes<TrackingParticleCollection>(pset.getParameter<edm::InputTag>("trackingParticles"))),
        scCollectionToken_(consumes<SimClusterCollection>(pset.getParameter<edm::InputTag>("simClusters")))
{
  produces<PFTruthParticleCollection>();
}

PFTruthParticleProducer::~PFTruthParticleProducer() {}

//
// member functions
//

std::set<TrackingParticleRef> PFTruthParticleProducer::findTrackingParticleMatch(
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
void PFTruthParticleProducer::produce(edm::StreamID, edm::Event &iEvent, const edm::EventSetup &iSetup) const {

  edm::Handle<TrackingParticleCollection> tpCollection;
  iEvent.getByToken(tpCollectionToken_, tpCollection);

  edm::Handle<SimClusterCollection> scCollection;
  iEvent.getByToken(scCollectionToken_, scCollection);

  auto out = std::make_unique<PFTruthParticleCollection>();
  // Not used right now, but useful for the trackID lookup later
  std::unordered_map<unsigned int, TrackingParticleRef> trackIdToTPRef;

  for (size_t i = 0; i < tpCollection->size(); i++) {
      TrackingParticleRef trackingParticle(tpCollection, i);
      PFTruthParticle particle;
      particle.addTrackingParticle(trackingParticle);
      particle.setCharge(trackingParticle->charge());
      particle.setPdgId(trackingParticle->pdgId());
      particle.setP4(trackingParticle->p4());
      out->push_back(particle);
      for (auto& track : trackingParticle->g4Tracks()) {
          unsigned int trackId = track.trackId();
          trackIdToTPRef[trackId] = trackingParticle;
      }
  }

  for (size_t i = 0; i < scCollection->size(); i++) {
      SimClusterRef simclus(scCollection, i);
      // Doesn't really do much anyway
      //findTrackingParticleMatch(trackIdToTPRef, simclus))
      // For now just randomly assign the SCs as a dummy
      int tpIdx = i % out->size();
      auto& pf = out->at(tpIdx);
      pf.addSimCluster(simclus);
  }

  iEvent.put(std::move(out));
}

// define this as a plug-in
DEFINE_FWK_MODULE(PFTruthParticleProducer);

