import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from DPGAnalysis.CaloNanoAOD.hcalRecHits_cff import *

nanoMetadata = cms.EDProducer("UniqueStringProducer",
    strings = cms.PSet(
        tag = cms.string("untagged"),
    )
)

hcalNanoSeq = cms.Sequence(
   	nanoMetadata 
	+hcalRecHitTables
)
