import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import Var,P3Vars
from DPGAnalysis.HGCalNanoAOD.hgcRecHits_cff import *

hgcRecHitsToSimClusters = cms.EDProducer("SimClusterRecHitAssociationProducer",
    caloRecHits = cms.VInputTag("HGCalRecHit:HGCEERecHits",
        "HGCalRecHit:HGCHEFRecHits", "HGCalRecHit:HGCHEBRecHits",
    ),
    simClusters = cms.InputTag("mix:MergedCaloTruth"),
)

hgcEERecHitsToSimClusterTable = cms.EDProducer("CaloRecHitToSimClusterIndexTableProducer",
    cut = hgcEERecHitsTable.cut,
    src = hgcEERecHitsTable.src,
    objName = hgcEERecHitsTable.name,
    branchName = cms.string("SimCluster"),
    objMap = cms.InputTag("hgcRecHitsToSimClusters:HGCEERecHitsToSimClus"),
    docString = cms.string("All SimCluster responsible for sim energy in RecHit DetId (ordered by fraction of energy)")
)

hgcEERecHitsToBestSimClusterTable = cms.EDProducer("CaloRecHitToBestSimClusterIndexTableProducer",
    cut = hgcEERecHitsTable.cut,
    src = hgcEERecHitsTable.src,
    objName = hgcEERecHitsTable.name,
    branchName = cms.string("BestSimCluster"),
    objMap = cms.InputTag("hgcRecHitsToSimClusters:HGCEERecHitsToBestSimClus"),
    docString = cms.string("SimCluster responsible for most sim energy in RecHit DetId")
)

hgcEERecHitsToBestMergedSimClusterTable = cms.EDProducer("CaloRecHitToBestSimClusterIndexTableProducer",
    cut = hgcEERecHitsTable.cut,
    src = hgcEERecHitsTable.src,
    objName = hgcEERecHitsTable.name,
    branchName = cms.string("BestMergedSimCluster"),
    objMap = cms.InputTag("hgcRecHitsToMergedSimClusters:HGCEERecHitsToBestSimClus"),
    docString = cms.string("SimCluster responsible for most sim energy in RecHit DetId")
)

simClusterRecEnergyTable = cms.EDProducer("SimClusterRecEnergyTableProducer",
    src = cms.InputTag("mix:MergedCaloTruth"),
    cut = cms.string(""),
    objName = cms.string("SimCluster"),
    branchName = cms.string("recEnergy"),
    valueMap = cms.InputTag("hgcRecHitsToSimClusters"),
    docString = cms.string("SimCluster deposited reconstructed energy associated to SimCluster")
)

hgcRecHitsToMergedSimClusters = cms.EDProducer("SimClusterRecHitAssociationProducer",
    caloRecHits = cms.VInputTag("HGCalRecHit:HGCEERecHits",
        "HGCalRecHit:HGCHEFRecHits", "HGCalRecHit:HGCHEBRecHits",
    ),
    simClusters = cms.InputTag("hgcSimTruth"),
)

hgcEERecHitsToMergedSimClusterTable = cms.EDProducer("CaloRecHitToSimClusterIndexTableProducer",
    cut = hgcEERecHitsTable.cut,
    src = hgcEERecHitsTable.src,
    objName = hgcEERecHitsTable.name,
    branchName = cms.string("MergedSimCluster"),
    objMap = cms.InputTag("hgcRecHitsToMergedSimClusters:HGCEERecHitsToSimClus"),
    docString = cms.string("MergedSimCluster responsible for most sim energy in RecHit DetId")
)

mergedSimClusterRecEnergyTable = cms.EDProducer("SimClusterRecEnergyTableProducer",
    src = cms.InputTag("hgcSimTruth"),
    cut = cms.string(""),
    objName = cms.string("MergedSimCluster"),
    branchName = cms.string("recEnergy"),
    valueMap = cms.InputTag("hgcRecHitsToMergedSimClusters"),
    docString = cms.string("SimCluster deposited reconstructed energy associated to SimCluster")
)

hgcHEfrontRecHitsToSimClusterTable = hgcEERecHitsToSimClusterTable.clone()
hgcHEfrontRecHitsToSimClusterTable.src = hgcHEfrontRecHitsTable.src
hgcHEfrontRecHitsToSimClusterTable.objName = hgcHEfrontRecHitsTable.name
hgcHEfrontRecHitsToSimClusterTable.objMap = "hgcRecHitsToSimClusters:HGCHEFRecHitsToSimClus"

hgcHEbackRecHitsToSimClusterTable = hgcEERecHitsToSimClusterTable.clone()
hgcHEbackRecHitsToSimClusterTable.src = hgcHEbackRecHitsTable.src
hgcHEbackRecHitsToSimClusterTable.objName = hgcHEbackRecHitsTable.name
hgcHEbackRecHitsToSimClusterTable.objMap = "hgcRecHitsToSimClusters:HGCHEBRecHitsToSimClus"

hgcHEfrontRecHitsToMergedSimClusterTable = hgcEERecHitsToMergedSimClusterTable.clone()
hgcHEfrontRecHitsToMergedSimClusterTable.src = hgcHEfrontRecHitsTable.src
hgcHEfrontRecHitsToMergedSimClusterTable.objName = hgcHEfrontRecHitsTable.name
hgcHEfrontRecHitsToMergedSimClusterTable.objMap = "hgcRecHitsToMergedSimClusters:HGCHEFRecHitsToSimClus"

hgcHEbackRecHitsToMergedSimClusterTable = hgcEERecHitsToMergedSimClusterTable.clone()
hgcHEbackRecHitsToMergedSimClusterTable.src = hgcHEbackRecHitsTable.src
hgcHEbackRecHitsToMergedSimClusterTable.objName = hgcHEbackRecHitsTable.name
hgcHEbackRecHitsToMergedSimClusterTable.objMap = "hgcRecHitsToMergedSimClusters:HGCHEBRecHitsToSimClus"

hgcRecHitSimAssociationSequence = cms.Sequence(hgcRecHitsToSimClusters
                +hgcRecHitsToMergedSimClusters
                +simClusterRecEnergyTable 
                +mergedSimClusterRecEnergyTable 
                +hgcEERecHitsToSimClusterTable+hgcHEfrontRecHitsToSimClusterTable+hgcHEbackRecHitsToSimClusterTable
                +hgcEERecHitsToBestSimClusterTable#+hgcHEfrontRecHitsToSimClusterTable+hgcHEbackRecHitsToSimClusterTable
                +hgcEERecHitsToMergedSimClusterTable+hgcHEfrontRecHitsToMergedSimClusterTable+hgcHEbackRecHitsToMergedSimClusterTable
                +hgcEERecHitsToBestMergedSimClusterTable#+hgcHEfrontRecHitsToMergedSimClusterTable+hgcHEbackRecHitsToMergedSimClusterTable
)
