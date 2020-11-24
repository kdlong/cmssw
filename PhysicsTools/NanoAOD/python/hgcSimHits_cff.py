import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import Var,P3Vars

hgcEESimHitsTable = cms.EDProducer("SimplePCaloHitFlatTableProducer",
    src = cms.InputTag("g4SimHits:HGCHitsEE"),
    cut = cms.string(""), 
    name = cms.string("SimHitHGCEE"),
    doc  = cms.string("Geant4 SimHits in HGCAL Electromagnetic endcap"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the muons
    variables = cms.PSet(
        detId = Var('id', 'int', precision=-1, doc='detId'),
        energy = Var('energy', 'float', precision=14, doc='energy'),
        geantTrackId = Var('geantTrackId', 'int', precision=-1, doc='Geant4 track ID'),
        geantFineTrackId = Var('geantFineTrackId', 'int', precision=-1, doc='granular Geant4 track ID'),
    )
)

hgcEEHitsToSimClusterTable = cms.EDProducer("CaloHitToSimClusterIndexTableProducer",
    cut = hgcEESimHitsTable.cut,
    src = hgcEESimHitsTable.src,
    objName = hgcEESimHitsTable.name,
    branchName = cms.string("SimCluster"),
    objMap = cms.InputTag("mix:simHitHGCEEToSimCluster"),
    docString = cms.string("SimCluster containing SimHit")
)

hgcHEFrontSimHitsTable = hgcEESimHitsTable.clone()
hgcHEFrontSimHitsTable.src = "g4SimHits:HGCHitsHEfront"
hgcHEFrontSimHitsTable.name = "SimHitHGCHEFront"

hgcHEFrontHitsToSimClusterTable = hgcEEHitsToSimClusterTable.clone()
hgcHEFrontHitsToSimClusterTable.objMap = "mix:simHitHGCHEbackToSimCluster"

hgcHEBackSimHitsTable = hgcEESimHitsTable.clone()
hgcHEBackSimHitsTable.src = "g4SimHits:HGCHitsHEback"
hgcHEBackSimHitsTable.name = "SimHitHGCHEBack"

hgcHEBackHitsToSimClusterTable = hgcEEHitsToSimClusterTable.clone()
hgcHEBackHitsToSimClusterTable.objMap = "mix:simHitHGCHEbackToSimCluster"

hgcEESimHitsWithPos = cms.EDProducer("HGCHitWithPositionProducer", 
    src = cms.VInputTag(hgcEESimHitsTable.src) 
)

hgcHEFrontSimHitsWithPos = cms.EDProducer("HGCHitWithPositionProducer", 
    src = cms.VInputTag(hgcHEFrontSimHitsTable.src)
)

hgcHEBackSimHitsWithPos = cms.EDProducer("HGCHitWithPositionProducer", 
    src = cms.VInputTag(hgcHEBackSimHitsTable.src)
)

hgcEESimHitsPositionTable = cms.EDProducer("SimplePCaloHitWithPositionFlatTableProducer",
    src = cms.InputTag("hgcEESimHitsWithPos"),
    cut = hgcEESimHitsTable.cut, 
    name = hgcEESimHitsTable.name,
    doc  = hgcEESimHitsTable.doc,
    singleton = cms.bool(False), 
    extension = cms.bool(True), 
    variables = cms.PSet(
        pt = Var('pt_', 'float', precision=-1, doc='pt'),
        eta = Var('eta_', 'float', precision=14, doc='eta'),
        phi = Var('phi_', 'float', precision=14, doc='phi'),
        x = Var('position_.x', 'float', precision=14, doc='x position'),
        y = Var('position_.y', 'float', precision=14, doc='x position'),
        z = Var('position_.z', 'float', precision=14, doc='z position'),
    )
)

hgcHEFrontSimHitsPositionTable = hgcEESimHitsPositionTable.clone()
hgcHEFrontSimHitsPositionTable.name = hgcHEFrontSimHitsTable.name
hgcHEFrontSimHitsPositionTable.src = "hgcHEFrontSimHitsWithPos"

hgcHEBackSimHitsPositionTable = hgcEESimHitsPositionTable.clone()
hgcHEBackSimHitsPositionTable.name = hgcHEBackSimHitsTable.name
hgcHEBackSimHitsPositionTable.src = "hgcHEBackSimHitsWithPos"

hgcSimHitsSequence = cms.Sequence(hgcEESimHitsTable+hgcHEBackSimHitsTable+hgcHEFrontSimHitsTable \
                +hgcEESimHitsWithPos+hgcEESimHitsPositionTable \
                +hgcHEFrontSimHitsWithPos+hgcHEFrontSimHitsPositionTable
                +hgcHEBackSimHitsWithPos+hgcHEBackSimHitsPositionTable \
                +hgcEEHitsToSimClusterTable+hgcHEFrontSimHitsTable+hgcHEBackSimHitsTable)
