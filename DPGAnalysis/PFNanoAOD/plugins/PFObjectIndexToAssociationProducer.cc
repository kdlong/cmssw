#include "PhysicsTools/NanoAOD/interface/ObjectIndexFromAssociationProducer.h"
#include "PhysicsTools/NanoAOD/interface/ObjectIndexFromOneToManyQualAssociationProducer.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/PFAnalysis/interface/PFTruthParticleFwd.h"
#include "SimDataFormats/PFAnalysis/interface/PFTruthParticle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"

#include "FWCore/Framework/interface/MakerMacros.h"

typedef ObjectIndexFromAssociationTableProducer<SimClusterCollection, PFTruthParticleCollection>
    SimClusterToPFTruthParticleIndexTableProducer;
typedef ObjectIndexFromAssociationTableProducer<TrackingParticleCollection, PFTruthParticleCollection>
    TrackingParticleToPFTruthParticleIndexTableProducer;
typedef ObjectIndexFromAssociationTableProducer<edm::View<reco::Track>, PFTruthParticleCollection>
    TrackToPFTruthParticleIndexTableProducer;
typedef ObjectIndexFromAssociationTableProducer<edm::View<reco::Track>, reco::PFCandidateCollection>
    TrackToPFCandIndexTableProducer;
typedef ObjectIndexFromAssociationTableProducer<reco::PFClusterCollection, reco::PFCandidateCollection>
    PFClusterToPFCandIndexTableProducer;
typedef ObjectIndexFromOneToManyQualAssociationTableProducer<edm::View<CaloRecHit>, reco::PFCandidateCollection, float>
    CaloRecHitToPFCandIndexTableProducer;
typedef ObjectIndexFromOneToManyQualAssociationTableProducer<edm::View<CaloRecHit>, reco::PFClusterCollection, float>
    CaloRecHitToPFClusterIndexTableProducer;
DEFINE_FWK_MODULE(TrackingParticleToPFTruthParticleIndexTableProducer);
DEFINE_FWK_MODULE(TrackToPFTruthParticleIndexTableProducer);
DEFINE_FWK_MODULE(SimClusterToPFTruthParticleIndexTableProducer);
DEFINE_FWK_MODULE(PFClusterToPFCandIndexTableProducer);
DEFINE_FWK_MODULE(CaloRecHitToPFCandIndexTableProducer);
DEFINE_FWK_MODULE(CaloRecHitToPFClusterIndexTableProducer);
DEFINE_FWK_MODULE(TrackToPFCandIndexTableProducer);

