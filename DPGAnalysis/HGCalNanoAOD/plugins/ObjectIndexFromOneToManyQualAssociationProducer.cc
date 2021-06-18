#include "PhysicsTools/NanoAOD/interface/ObjectIndexFromOneToManyQualAssociationProducer.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef ObjectIndexFromOneToManyQualAssociationTableProducer<std::vector<reco::CaloCluster>, SimClusterCollection>
    LayerClusterToSimClusterIndexTableProducer;
typedef ObjectIndexFromOneToManyQualAssociationTableProducer<HGCRecHitCollection, SimClusterCollection>
    CaloRecHitToSimClusterIndexTableProducer;
typedef ObjectIndexFromOneToManyQualAssociationTableProducer<HGCRecHitCollection, std::vector<reco::CaloCluster>>
    HGCRecHitToLayerClusterIndexTableProducer;
typedef ObjectIndexFromOneToManyQualAssociationTableProducer<SimClusterCollection, SimClusterCollection>
    SimClusterToSimClustersIndexTableProducer;

DEFINE_FWK_MODULE(LayerClusterToSimClusterIndexTableProducer);
DEFINE_FWK_MODULE(HGCRecHitToLayerClusterIndexTableProducer);
DEFINE_FWK_MODULE(CaloRecHitToSimClusterIndexTableProducer);
DEFINE_FWK_MODULE(SimClusterToSimClustersIndexTableProducer);
