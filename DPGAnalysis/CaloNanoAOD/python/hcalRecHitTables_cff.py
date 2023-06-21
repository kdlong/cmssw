import FWCore.ParameterSet.Config as cms

from PhysicsTools.NanoAOD.common_cff import Var,P3Vars

hcalHBHERecHitTable = cms.EDProducer("SimplePFRecHitFlatTableProducer",
    src = cms.InputTag("particleFlowRecHitHBHE"),
    cut = cms.string(''),
    name = cms.string('HcalRecHitHBHE'),
    doc = cms.string('Hcal HBHE RecHit table'),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(
        detid = Var("detId()", 'int', doc = "detId"),
        energy = Var("energy", 'float', doc = "energy"),
        time = Var("time", 'float', doc = "time"),
    )
)

hcalHBHERecHitPositionTable = cms.EDProducer("PFRecHitPositionTableProducer",
    src = hcalHBHERecHitTable.src,
    doc = hcalHBHERecHitTable.doc,
    name = hcalHBHERecHitTable.name,
    cut = hcalHBHERecHitTable.cut,
)

hcalHFRecHitTable = cms.EDProducer("SimplePFRecHitFlatTableProducer",
    src = cms.InputTag("particleFlowRecHitHF"),
    cut = cms.string(''),
    name = cms.string('HcalRecHitHF'),
    doc = cms.string('Hcal HF RecHit table'),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(
        detid = Var("detId()", 'int', doc = "detId"),
        energy = Var("energy", 'float', doc = "energy"),
        time = Var("time", 'float', doc = "time"),
    )
)

hcalHFRecHitPositionTable = cms.EDProducer("PFRecHitPositionTableProducer",
    src = hcalHFRecHitTable.src,
    doc = hcalHFRecHitTable.doc,
    name = hcalHFRecHitTable.name,
    cut = hcalHFRecHitTable.cut,
)

hcalHORecHitTable = cms.EDProducer("SimplePFRecHitFlatTableProducer",
    src = cms.InputTag("particleFlowRecHitHO"),
    cut = cms.string(''),
    name = cms.string('HcalRecHitHO'),
    doc = cms.string('Hcal HO RecHit table'),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(
        detid = Var("detId()", 'int', doc = "detId"),
        energy = Var("energy", 'float', doc = "energy"),
        time = Var("time", 'float', doc = "time"),
    )
)

hcalHORecHitPositionTable = cms.EDProducer("PFRecHitPositionTableProducer",
    src = hcalHORecHitTable.src,
    doc = hcalHORecHitTable.doc,
    name = hcalHORecHitTable.name,
    cut = hcalHORecHitTable.cut,
)

hcalRecHitTables = cms.Sequence(hcalHBHERecHitTable
                                +hcalHBHERecHitPositionTable
                                +hcalHFRecHitTable
                                +hcalHFRecHitPositionTable
                                +hcalHORecHitTable
                                +hcalHORecHitPositionTable)
