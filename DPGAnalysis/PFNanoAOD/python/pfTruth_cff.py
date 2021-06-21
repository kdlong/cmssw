import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import CandVars,Var
from DPGAnalysis.HGCalNanoAOD.simClusters_cff import simClusterTable
from DPGAnalysis.TrackNanoAOD.trackingParticles_cff import trackingParticleTable

pfTruthParticles = cms.EDProducer("PFTruthParticleProducer",
    trackingParticles= cms.InputTag("mix:MergedTrackTruth"),
    caloParticles= cms.InputTag("mix:MergedCaloTruth"),
    simVertices= cms.InputTag("SimVertices"),
    simTracks= cms.InputTag("SimTracks"),
)

pfTruthTable = cms.EDProducer("SimplePFTruthParticleFlatTableProducer",
    src = cms.InputTag("pfTruthParticles"),
    cut = cms.string(""), 
    name = cms.string("PFTruthPart"),
    doc  = cms.string("PFTruthParticles"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the muons
    variables = cms.PSet(CandVars,
        nSimCluster = Var("nSimCluster", 'int', precision=-1, doc='Number of associated SimClusters'),
        nTrackingPart = Var("nTrackingParticle", 'int', precision=-1, doc='Number of associated SimClusters'),
    )
)

simClusterToPFTruthTable = cms.EDProducer("SimClusterToPFTruthParticleIndexTableProducer",
    cut = cms.string(""),
    src = cms.InputTag("mix:MergedCaloTruth"),
    objName = simClusterTable.name,
    branchName = pfTruthTable.name,
    objMap = cms.InputTag("pfTruthParticles:simClusToPFTruth"),
    docString = cms.string("PFTruth particle to which the SimCluster is associated")
)

trackingPartToPFTruthTable = cms.EDProducer("TrackingParticleToPFTruthParticleIndexTableProducer",
    cut = cms.string(""),
    src = cms.InputTag("mix:MergedTrackTruth"),
    objName = trackingParticleTable.name,
    branchName = pfTruthTable.name,
    objMap = cms.InputTag("pfTruthParticles:trackingPartToPFTruth"),
    docString = cms.string("PFTruth particle to which the TrackingPart is associated")
)

pfTruth = cms.Sequence(pfTruthParticles+pfTruthTable+simClusterToPFTruthTable+trackingPartToPFTruthTable)

