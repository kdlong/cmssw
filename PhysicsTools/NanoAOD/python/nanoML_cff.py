from __future__ import print_function
import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *

nanoMetadata = cms.EDProducer("UniqueStringProducer",
    strings = cms.PSet(
        tag = cms.string("untagged"),
    )
)

simclusterTable = cms.EDProducer("SimpleSimClusterFlatTableProducer",
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

simTrackTable = cms.EDProducer("SimpleSimTrackFlatTableProducer",
    src = cms.InputTag("g4SimHits"),
    cut = cms.string("abs(momentum().eta) > 1.52 || abs(getMomentumAtBoundary().eta()) > 1.52"), 
    name = cms.string("SimTrack"),
    doc  = cms.string("Geant4 SimHits in HGCAL Electromagnetic endcap"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the muons
    variables = cms.PSet(
        crossedBoundary = Var('crossedBoundary', 'bool', doc='track crossed boundary'),
        pdgId = Var('type', 'int', doc='pdgId (track type)'),
        charge = Var('charge', 'int', doc='ID'),
        trackId = Var('trackId', 'int', precision=-1, doc='ID'),
        pt = Var('momentum().pt()', 'float', precision=14, doc='pt'),
        eta = Var('momentum().eta()', 'float', precision=14, doc='eta'),
        phi = Var('momentum().phi()', 'float', precision=14, doc='phi'),
        boundaryPos_x = Var('getPositionAtBoundary().x()', 'float', precision=14, doc='x position at HGCAL boundary'),
        boundaryPos_y = Var('getPositionAtBoundary().y()', 'float', precision=14, doc='y position at HGCAL boundary'),
        boundaryPos_z = Var('getPositionAtBoundary().z()', 'float', precision=14, doc='z position at HGCAL boundary'),
        boundaryPos_pt = Var('getMomentumAtBoundary().pt()', 'float', precision=14, doc='pt at HGCAL boundary'),
        boundaryPos_eta = Var('getMomentumAtBoundary().eta()', 'float', precision=14, doc='eta position at HGCAL boundary'),
        boundaryPos_phi = Var('getMomentumAtBoundary().phi()', 'float', precision=14, doc='phi position at HGCAL boundary'),
    )
)

hgcEESimHitsTable = cms.EDProducer("SimplePCaloHitFlatTableProducer",
    src = cms.InputTag("g4SimHits:HGCHitsEE"),
    cut = cms.string(""), 
    name = cms.string("SimHitHGCEE"),
    doc  = cms.string("Geant4 SimHits in HGCAL Electromagnetic endcap"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the muons
    variables = cms.PSet(
        detId = Var('id', 'float', precision=-1, doc='detId'),
        energy = Var('energy', 'float', precision=14, doc='energy'),
        geantTrackId = Var('geantTrackId', 'int', precision=-1, doc='Geant4 track ID'),
        geantFineTrackId = Var('geantFineTrackId', 'int', precision=-1, doc='granular Geant4 track ID'),
    )
)

hgcHEFrontSimHitsTable = hgcEESimHitsTable.clone()
hgcHEFrontSimHitsTable.src = "g4SimHits:HGCHitsHEfront"
hgcHEFrontSimHitsTable.name = "SimHitHGCHEFront"
hgcHEBackSimHitsTable = hgcEESimHitsTable.clone()
hgcHEBackSimHitsTable.src = "g4SimHits:HGCHitsHEback"
hgcHEBackSimHitsTable.name = "SimHitHGCHEBack"

nanoMLSequence = cms.Sequence(nanoMetadata+simclusterTable+hgcEESimHitsTable+hgcHEBackSimHitsTable+hgcHEFrontSimHitsTable+simTrackTable)
