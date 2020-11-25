from __future__ import print_function
import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from hgcSimHits_cff import *
from hgcSimTracks_cff import *
from simClusters_cff import simClusterTable
from caloParticles_cff import *
from trackingParticles_cff import *
from genparticles_cff import genParticleTable
from genVertex_cff import *

nanoMetadata = cms.EDProducer("UniqueStringProducer",
    strings = cms.PSet(
        tag = cms.string("untagged"),
    )
)

genParticleTable.src = "genParticles"

nanoHGCMLSequence = cms.Sequence(nanoMetadata+genVertexTables+genParticleTable+ \
        trackingParticleTable+caloParticleTable+simClusterTable+ \
        simTrackTables+hgcSimHitsSequence)
