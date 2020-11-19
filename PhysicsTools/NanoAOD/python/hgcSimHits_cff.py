import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import Var

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

hgcSimHitsSequence = cms.Sequence(hgcEESimHitsTable+hgcHEBackSimHitsTable+hgcHEFrontSimHitsTable+simTrackTable)
