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
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/PFAnalysis/interface/PFParticleFwd.h"
#include "SimDataFormats/PFAnalysis/interface/PFParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"

#include "FWCore/Utilities/interface/EDGetToken.h"
#include <set>

#include "DataFormats/RecoCandidate/interface/TrackAssociation.h"
#include "DataFormats/Common/interface/OneToMany.h"
#include "DataFormats/Common/interface/AssociationMap.h"

//
// class decleration
//

typedef edm::AssociationMap<edm::OneToMany<
    TrackingParticleCollection, SimClusterCollection>> TrackingParticleToSimCluster;

class PFParticleProducer : public edm::stream::EDProducer<> {
public:
  explicit PFParticleProducer(const edm::ParameterSet &);
  ~PFParticleProducer() override;

private:
  virtual void produce(edm::Event&, const edm::EventSetup&) override;

  edm::EDGetTokenT<TrackingParticleCollection> trackingPartToken_;
  edm::EDGetTokenT<TrackingParticleToSimCluster> trackingPartToSimClusToken_;
};

PFParticleProducer::PFParticleProducer(const edm::ParameterSet &config)
    : trackingPartToken_(consumes<TrackingParticleCollection>(config.getParameter<edm::InputTag>("trackingParticles"))),
        trackingPartToSimClusToken_(consumes<TrackingParticleToSimCluster>(config.getParameter<edm::InputTag>("trackingParticleSimClusterAssoc"))) {
  produces<PFParticleCollection>();
}

PFParticleProducer::~PFParticleProducer() {}

//
// member functions
//

// ------------ method called to produce the data  ------------
void PFParticleProducer::produce(edm::Event &iEvent, const edm::EventSetup &iSetup) {

  edm::Handle<TrackingParticleCollection> tpCollection;
  iEvent.getByToken(trackingPartToken_, tpCollection);

  TrackingParticleToSimCluster trackingPartToSimClus = iEvent.get(trackingPartToSimClusToken_);

  auto out = std::make_unique<PFParticleCollection>();
  for (size_t i = 0; i < tpCollection->size(); i++) {
    PFParticle pf;
    TrackingParticleRef tp(tpCollection, i);
    pf.addTrackingParticle(tp);

    try  {
        SimClusterRefVector assocSimClusters = trackingPartToSimClus[tp];
        pf.setSimClusters(assocSimClusters);
    }
    catch (edm::Exception const &) { 
    }
  }

  iEvent.put(std::move(out));
}

DEFINE_FWK_MODULE(PFParticleProducer);

