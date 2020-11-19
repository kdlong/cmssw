from __future__ import print_function
import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from hgcSimHits_cff import *
from hgcSimTracks_cff import *
from simClusters_cff import simClusterTable

nanoMetadata = cms.EDProducer("UniqueStringProducer",
    strings = cms.PSet(
        tag = cms.string("untagged"),
    )
)

nanoHGCMLSequence = cms.Sequence(nanoMetadata+simClusterTable+simTrackTable+hgcSimHitsSequence)
