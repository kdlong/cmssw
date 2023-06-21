import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import CandVars,Var,P3Vars

pfCandTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
    src = cms.InputTag("particleFlow"),
    cut = cms.string(""), 
    name = cms.string("PFCand"),
    doc  = cms.string("ParticleFlow candidates"),
    singleton = cms.bool(False),
    extension = cms.bool(False), 
    variables = cms.PSet(CandVars,
        Vtx_x = Var('vertex().x()', 'float', precision=14, doc='vertex x pos'),
        Vtx_y = Var('vertex().y()', 'float', precision=14, doc='vertex y pos'),
        Vtx_z = Var('vertex().z()', 'float', precision=14, doc='vertex z pos'),
        TrackIdx = Var("? trackRef().isNonnull() ? trackRef().key() : -1", "int", precision=10, doc="Matching track index"),
    )
)

pfCandTables = cms.Sequence(
        pfCandTable
)
