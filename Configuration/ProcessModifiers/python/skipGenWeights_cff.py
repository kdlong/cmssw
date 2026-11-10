import FWCore.ParameterSet.Config as cms

# This modifier is for disabling the gen weight producers (genWeights, lheWeights), which run in the GEN step by default

skipGenWeights = cms.Modifier()
