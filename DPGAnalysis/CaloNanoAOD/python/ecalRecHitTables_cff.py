import FWCore.ParameterSet.Config as cms

from PhysicsTools.NanoAOD.common_cff import Var,P3Vars

ecalRecHitEBEETable = cms.EDProducer("SimplePFRecHitFlatTableProducer",
    src = cms.InputTag("particleFlowRecHitECAL"),
    cut = cms.string(''),
    name = cms.string('EcalRecHitEBEE'),
    doc = cms.string('Ecal EB/EE RecHit table'),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(
        detid = Var("detId()", 'int', doc = "detId"),
        energy = Var("energy", 'float', doc = "energy"),
        time = Var("time", 'float', doc = "time"),
    )
)

ecalRecHitEBEEPositionTable = cms.EDProducer("PFRecHitPositionTableProducer",
    src = ecalRecHitEBEETable.src,
    doc = ecalRecHitEBEETable.doc,
    name = ecalRecHitEBEETable.name,
    cut = ecalRecHitEBEETable.cut,
)

ecalRecHitESTable = cms.EDProducer("SimplePFRecHitFlatTableProducer",
    src = cms.InputTag("particleFlowRecHitPS"),
    cut = cms.string(''),
    name = cms.string('EcalRecHitES'),
    doc = cms.string('Ecal ES RecHit table'),
    singleton = cms.bool(False),
    extension = cms.bool(False),
    variables = cms.PSet(
        detid = Var("detId()", 'int', doc = "detId"),
        energy = Var("energy", 'float', doc = "energy"),
        time = Var("time", 'float', doc = "time"),
    )
)

ecalRecHitESPositionTable = cms.EDProducer("PFRecHitPositionTableProducer",
    src = ecalRecHitESTable.src,
    doc = ecalRecHitESTable.doc,
    name = ecalRecHitESTable.name,
    cut = ecalRecHitESTable.cut,
)

ecalRecHitTables = cms.Sequence(ecalRecHitESTable
                                +ecalRecHitESPositionTable
                                +ecalRecHitEBEETable
                                +ecalRecHitEBEEPositionTable)
