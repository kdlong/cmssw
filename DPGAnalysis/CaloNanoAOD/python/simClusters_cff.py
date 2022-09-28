import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import CandVars,Var

simClusterTable = cms.EDProducer("SimpleSimClusterFlatTableProducer",
    src = cms.InputTag("mix:MergedCaloTruth"),
    cut = cms.string(""),
    name = cms.string("SimCluster"),
    doc  = cms.string("SimCluster information"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the muons
    variables = cms.PSet(CandVars,
        eventId = Var('eventId().event()', 'int', precision=-1, doc='Event ID (can be used to identify pileup'),
        bunchCrossing = Var('eventId().bunchCrossing()', 'int', precision=-1, doc='bunch crossing (can be used to identify pileup'),
        rawEventId = Var('eventId().rawId()', 'int', precision=-1, doc='Raw event ID (can be used to identify pileup'),
        lastPos_x = Var('g4Tracks.at(0).trackerSurfacePosition().x()', 'float', precision=14, doc='track x final position'),
        lastPos_y = Var('g4Tracks.at(0).trackerSurfacePosition().y()', 'float', precision=14, doc='track y final position'),
        lastPos_z = Var('g4Tracks.at(0).trackerSurfacePosition().z()', 'float', precision=14, doc='track z final position'),
        # For reasons lost on me, the nsimhits_ variable is uninitialized, and hits_ (which are really simhits)
        # are often referred to as rechits in the SimCluster class
        nHits = Var('numberOfRecHits', 'int', precision=-1, doc='number of simhits'),
        sumHitEnergy = Var('energy', 'float', precision=14, doc='total energy of simhits'),
        trackId = Var('g4Tracks().at(0).trackId()', 'int', precision=-1, doc='Geant track id'),
        crossedBoundary = Var('g4Tracks().at(0).crossedBoundary()', 'bool', doc='Crossed the HGCAL boundary')
    )
)

simClusterToCaloPart = cms.EDProducer("SimClusterToCaloParticleAssociationProducer",
    caloParticles = cms.InputTag("mix:MergedCaloTruth"),
    simClusters = cms.InputTag("mix:MergedCaloTruth"),
)

simClusterToCaloPartTable = cms.EDProducer("SimClusterToCaloParticleIndexTableProducer",
    cut = simClusterTable.cut,
    src = simClusterTable.src,
    objName = simClusterTable.name,
    branchName = cms.string("CaloPart"),
    objMap = cms.InputTag("simClusterToCaloPart"),
    docString = cms.string("Index of CaloPart containing SimCluster")
)

simClusterTables = cms.Sequence(simClusterTable+simClusterToCaloPart+simClusterToCaloPartTable)
