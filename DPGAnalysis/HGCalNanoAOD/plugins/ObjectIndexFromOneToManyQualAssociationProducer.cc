#include "PhysicsTools/NanoAOD/interface/ObjectIndexFromOneToManyQualAssociationProducer.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef ObjectIndexFromOneToManyQualAssociationTableProducer<std::vector<reco::CaloCluster>, SimClusterCollection>
    LayerClusterToBestSimClusterIndexTableProducer;

typedef ObjectIndexFromOneToManyQualAssociationTableProducer<HGCRecHitCollection, std::vector<reco::CaloCluster>>
    HGCRecHitToBestLayerClusterIndexTableProducer;
DEFINE_FWK_MODULE(LayerClusterToBestSimClusterIndexTableProducer);
DEFINE_FWK_MODULE(HGCRecHitToBestLayerClusterIndexTableProducer);
