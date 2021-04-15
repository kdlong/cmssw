import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import Var,P3Vars
from DPGAnalysis.HGCalNanoAOD.hgcRecHits_cff import *

hgcRecHitsToSimClusters = cms.EDProducer("SimClusterRecHitAssociationProducer",
    caloRecHits = cms.VInputTag("hgcRecHits"),
    simClusters = cms.InputTag("mix:MergedCaloTruth"),
)

hgcRecHitsToMergedSimClusters = cms.EDProducer("SimClusterRecHitAssociationProducer",
    caloRecHits = cms.VInputTag("hgcRecHits"),
    simClusters = cms.InputTag("hgcSimTruth"),
)

hgcRecHitsToSimClusterTable = cms.EDProducer("CaloRecHitToSimClusterIndexTableProducer",
    cut = hgcRecHitsTable.cut,
    src = hgcRecHitsTable.src,
    objName = hgcRecHitsTable.name,
    branchName = cms.string("SimCluster"),
    objMap = cms.InputTag("hgcRecHitsToSimClusters:hgcRecHitsToSimClus"),
    docString = cms.string("All SimCluster responsible for sim energy in RecHit DetId (ordered by fraction of energy)")
)

hgcRecHitsToBestSimClusterTable = cms.EDProducer("CaloRecHitToBestSimClusterIndexTableProducer",
    cut = hgcRecHitsTable.cut,
    src = hgcRecHitsTable.src,
    objName = hgcRecHitsTable.name,
    branchName = cms.string("BestSimCluster"),
    objMap = cms.InputTag("hgcRecHitsToSimClusters:hgcRecHitsToBestSimClus"),
    docString = cms.string("SimCluster responsible for most sim energy in RecHit DetId")
)

hgcRecHitsToMergedSimClusterTable = cms.EDProducer("CaloRecHitToSimClusterIndexTableProducer",
    cut = hgcRecHitsTable.cut,
    src = hgcRecHitsTable.src,
    objName = hgcRecHitsTable.name,
    branchName = cms.string("MergedSimCluster"),
    objMap = cms.InputTag("hgcRecHitsToMergedSimClusters:hgcRecHitsToSimClus"),
    docString = cms.string("MergedSimCluster responsible for most sim energy in RecHit DetId")
)

hgcRecHitsToBestMergedSimClusterTable = cms.EDProducer("CaloRecHitToBestSimClusterIndexTableProducer",
    cut = hgcRecHitsTable.cut,
    src = hgcRecHitsTable.src,
    objName = hgcRecHitsTable.name,
    branchName = cms.string("BestMergedSimCluster"),
    objMap = cms.InputTag("hgcRecHitsToMergedSimClusters:hgcRecHitsToBestSimClus"),
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

mergedSimClusterRecEnergyTable = cms.EDProducer("SimClusterRecEnergyTableProducer",
    src = cms.InputTag("hgcSimTruth"),
    cut = cms.string(""),
    objName = cms.string("MergedSimCluster"),
    branchName = cms.string("recEnergy"),
    valueMap = cms.InputTag("hgcRecHitsToMergedSimClusters"),
    docString = cms.string("SimCluster deposited reconstructed energy associated to SimCluster")
)

hgcRecHitSimAssociationSequence = cms.Sequence(hgcRecHitsToSimClusters
                +hgcRecHitsToMergedSimClusters
                +simClusterRecEnergyTable 
                +mergedSimClusterRecEnergyTable 
                +hgcRecHitsToSimClusterTable
                +hgcRecHitsToBestSimClusterTable
                +hgcRecHitsToMergedSimClusterTable
                +hgcRecHitsToBestMergedSimClusterTable
)
