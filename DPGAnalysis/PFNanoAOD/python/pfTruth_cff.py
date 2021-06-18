import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import CandVars,Var

pfTruthParticles = cms.EDProducer("PFTruthParticleProducer",
    simClusters = cms.InputTag("mix:MergedCaloTruth"),
    trackingParticles= cms.InputTag("mix:MergedTrackTruth"),
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

pfTruth = cms.Sequence(pfTruthParticles+pfTruthTable)

