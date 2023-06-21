#include "PhysicsTools/NanoAOD/interface/SimpleFlatTableProducer.h"

#include "SimDataFormats/PFAnalysis/interface/PFTruthParticle.h"
typedef SimpleFlatTableProducer<PFTruthParticle> SimplePFTruthParticleFlatTableProducer;

#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
typedef SimpleFlatTableProducer<reco::PFCluster> SimplePFClusterFlatTableProducer;

#include "DataFormats/ParticleFlowReco/interface/PFRecTrack.h"
typedef SimpleFlatTableProducer<reco::PFRecTrack> SimplePFRecTrackFlatTableProducer;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(SimplePFTruthParticleFlatTableProducer);
DEFINE_FWK_MODULE(SimplePFClusterFlatTableProducer);
DEFINE_FWK_MODULE(SimplePFRecTrackFlatTableProducer);
