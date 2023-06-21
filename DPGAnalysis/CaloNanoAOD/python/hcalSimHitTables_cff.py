import FWCore.ParameterSet.Config as cms

from PhysicsTools.NanoAOD.common_cff import Var,P3Vars

hcalSimHitTable = cms.EDProducer("SimplePCaloHitFlatTableProducer",
    src = cms.InputTag("g4SimHits","HcalHits"),
    cut = cms.string(''),
    name = cms.string('HcalSimHit'),
    doc = cms.string('Hcal SimHit table'),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(
        detid = Var("id()", 'int', doc = "detId"),
        energy = Var("energy", 'float', doc = "energy"),
        time = Var("time", 'float', doc = "time"),
    )
)

hcalSimHitPositionTable = cms.EDProducer("HcalSimHitPositionTableProducer",
    src = hcalSimHitTable.src,
    doc = hcalSimHitTable.doc,
    name = hcalSimHitTable.name,
    cut = hcalSimHitTable.cut,
)

hcalSimHitTables = cms.Sequence(hcalSimHitTable
                               +hcalSimHitPositionTable)
