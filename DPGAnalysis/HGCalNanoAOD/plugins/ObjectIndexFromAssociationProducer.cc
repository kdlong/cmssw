#include "PhysicsTools/NanoAOD/interface/ObjectIndexFromAssociationProducer.h"
#include "DataFormats/CaloRecHit/interface/CaloRecHit.h"
#include "DataFormats/HGCalReco/interface/TICLCandidate.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef ObjectIndexFromAssociationTableProducer<edm::View<CaloRecHit>, std::vector<TICLCandidate>>
    RecHitToTICLCandidateIndexTableProducer;

DEFINE_FWK_MODULE(RecHitToTICLCandidateIndexTableProducer);
