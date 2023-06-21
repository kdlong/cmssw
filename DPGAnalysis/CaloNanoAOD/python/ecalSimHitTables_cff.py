import FWCore.ParameterSet.Config as cms

from PhysicsTools.NanoAOD.common_cff import Var,P3Vars

ecalSimHitEBTable = cms.EDProducer("SimplePCaloHitFlatTableProducer",
    src = cms.InputTag("g4SimHits","EcalHitsEB"),
    cut = cms.string(''),
    name = cms.string('EcalSimHitEB'),
    doc = cms.string('Ecal EB SimHit table'),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(
        detid = Var("id()", 'int', doc = "detId"),
        energy = Var("energy", 'float', doc = "energy"),
        time = Var("time", 'float', doc = "time"),
    )
)

ecalSimHitEBPositionTable = cms.EDProducer("EcalSimHitPositionTableProducer",
    src = ecalSimHitEBTable.src,
    doc = ecalSimHitEBTable.doc,
    name = ecalSimHitEBTable.name,
    cut = ecalSimHitEBTable.cut,
)

ecalSimHitEETable = cms.EDProducer("SimplePCaloHitFlatTableProducer",
    src = cms.InputTag("g4SimHits","EcalHitsEE"),
    cut = cms.string(''),
    name = cms.string('EcalSimHitEE'),
    doc = cms.string('Ecal EE SimHit table'),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(
        detid = Var("id()", 'int', doc = "detId"),
        energy = Var("energy", 'float', doc = "energy"),
        time = Var("time", 'float', doc = "time"),
    )
)

ecalSimHitEEPositionTable = cms.EDProducer("EcalSimHitPositionTableProducer",
    src = ecalSimHitEETable.src,
    doc = ecalSimHitEETable.doc,
    name = ecalSimHitEETable.name,
    cut = ecalSimHitEETable.cut,
)

ecalSimHitESTable = cms.EDProducer("SimplePCaloHitFlatTableProducer",
    src = cms.InputTag("g4SimHits","EcalHitsES"),
    cut = cms.string(''),
    name = cms.string('EcalSimHitES'),
    doc = cms.string('Ecal ES SimHit table'),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(
        detid = Var("id()", 'int', doc = "detId"),
        energy = Var("energy", 'float', doc = "energy"),
        time = Var("time", 'float', doc = "time"),
    )
)

ecalSimHitESPositionTable = cms.EDProducer("EcalSimHitPositionTableProducer",
    src = ecalSimHitESTable.src,
    doc = ecalSimHitESTable.doc,
    name = ecalSimHitESTable.name,
    cut = ecalSimHitESTable.cut,
)

ecalSimHitTables = cms.Sequence(ecalSimHitESTable
                                +ecalSimHitESPositionTable
                                +ecalSimHitEETable
                                +ecalSimHitEEPositionTable
                                +ecalSimHitEBTable
                                +ecalSimHitEBPositionTable)
