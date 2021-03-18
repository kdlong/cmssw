import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import P3Vars,Var

layerClusterTable = cms.EDProducer("SimpleCaloClusterFlatTableProducer",
    src = cms.InputTag("hgcalLayerClusters"),
    cut = cms.string(""),
    name = cms.string("LayerCluster"),
    doc  = cms.string("LayerCluster information"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the muons
    variables = cms.PSet(
        eta  = Var("eta",  float,precision=12),
        phi = Var("phi", float, precision=12),
        energy = Var("energy", float, precision=14),
        x = Var('position().x()', 'float', precision=14, doc='x position'),
        y = Var('position.y()', 'float', precision=14, doc='y position'),
        z = Var('position.z()', 'float', precision=14, doc='z position'),
        nHits = Var('size', 'int', precision=14, doc='number of rechits'),
        seedDetId = Var('seed().rawId()', 'int', precision=-1, doc='detId of seed hit'),
    )
)

layerClusterToSimClusterTable = cms.EDProducer("LayerClusterToSimClusterIndexTableProducer",
    cut = layerClusterTable.cut,
    src = layerClusterTable.src,
    objName = layerClusterTable.name,
    branchName = cms.string("SimCluster"),
    objMap = cms.InputTag("layerClusterCaloParticleAssociationProducer"),
    docString = cms.string("Index of SimCluster matched to LayerCluster")
)

layerClusterTables = cms.Sequence(layerClusterTable+layerClusterToSimClusterTable)

