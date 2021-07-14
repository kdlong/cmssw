#include "PhysicsTools/NanoAOD/interface/ObjectIndexFromAssociationProducer.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/PFAnalysis/interface/PFTruthParticleFwd.h"
#include "SimDataFormats/PFAnalysis/interface/PFTruthParticle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef ObjectIndexFromAssociationTableProducer<SimClusterCollection, PFTruthParticleCollection>
    SimClusterToPFTruthParticleIndexTableProducer;
typedef ObjectIndexFromAssociationTableProducer<TrackingParticleCollection, PFTruthParticleCollection>
    TrackingParticleToPFTruthParticleIndexTableProducer;
DEFINE_FWK_MODULE(TrackingParticleToPFTruthParticleIndexTableProducer);
DEFINE_FWK_MODULE(SimClusterToPFTruthParticleIndexTableProducer);
