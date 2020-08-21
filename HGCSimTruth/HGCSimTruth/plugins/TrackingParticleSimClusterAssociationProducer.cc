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

#include "FWCore/Utilities/interface/EDGetToken.h"

//
// class decleration
//

typedef edm::AssociationMap<edm::OneToManyWithQuality<
    TrackingParticleCollection, SimClusterCollection, float>> TrackingParticleToSimCluster;

class TrackingParticleSimClusterAssociationProducer : public edm::global::EDProducer<> {
public:
  explicit TrackingParticleSimClusterAssociationProducer(const edm::ParameterSet &);
  ~TrackingParticleSimClusterAssociationProducer() override;

private:
  void produce(edm::StreamID, edm::Event &, const edm::EventSetup &) const override;

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

// ------------ method called to produce the data  ------------
void TrackingParticleSimClusterAssociationProducer::produce(edm::StreamID, edm::Event &iEvent, const edm::EventSetup &iSetup) const {

  edm::Handle<TrackingParticleCollection> tpCollection;
  iEvent.getByToken(tpCollectionToken_, tpCollection);

  edm::Handle<SimClusterCollection> simclusCollection;
  iEvent.getByToken(scCollectionToken_, simclusCollection);

  auto out = std::make_unique<TrackingParticleToSimCluster>();
  std::cout << "Trying to make associations\n";

  iEvent.put(std::move(out));
}

// define this as a plug-in
DEFINE_FWK_MODULE(TrackingParticleSimClusterAssociationProducer);
