#include "PhysicsTools/NanoAOD/interface/SimpleFlatTableProducer.h"

#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
typedef SimpleFlatTableProducer<reco::CaloCluster> SimpleCaloClusterFlatTableProducer;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(SimpleCaloClusterFlatTableProducer);
