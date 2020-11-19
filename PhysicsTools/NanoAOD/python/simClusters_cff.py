import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import CandVars

simClusterTable = cms.EDProducer("SimpleSimClusterFlatTableProducer",
    src = cms.InputTag("mix:MergedCaloTruth"),
    cut = cms.string(""),
    name = cms.string("SimCluster"),
    doc  = cms.string("SimCluster information"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the muons
    variables = cms.PSet(CandVars,
        impactPointX = Var('impactPoint().x()', 'float', precision=14, doc='x position'),
        impactPointY = Var('impactPoint().y()', 'float', precision=14, doc='y position'),
        impactPointZ = Var('impactPoint().z()', 'float', precision=14, doc='z position'),
        nSimHits = Var('numberOfSimHits', 'int', precision=-1, doc='total energy of simhits'),
        simEnergy = Var('simEnergy', 'float', precision=14, doc='total energy of simhits'),
        trackId = Var('g4Tracks().at(0).trackId()', 'int', precision=10, doc='Geant track id'),
    )
)

