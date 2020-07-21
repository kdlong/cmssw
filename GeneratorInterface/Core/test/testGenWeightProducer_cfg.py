import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.register(
    "lheSource",
    "externalLHEProducer",
    VarParsing.multiplicity.singleton,
    VarParsing.varType.string,
    "LHE source (default externalLHEProducer)"
)
options.parseArguments()

process = cms.Process("test")

process.source = cms.Source("PoolSource",
            fileNames = cms.untracked.vstring(options.inputFiles)
)

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(options.maxEvents))

process.out = cms.OutputModule("PoolOutputModule",
            fileName = cms.untracked.string('test.root'),
            outputCommands = cms.untracked.vstring(['drop *', 
                'keep GenWeightProduct_test*Weights*_*_*',
                'keep GenWeightInfoProduct_test*Weights*_*_*',])
)

#process.testLHEWeights = cms.EDProducer("LHEWeightProductProducer",
#    lheSourceLabel = cms.string("externalLHEProducer"))

process.testGenWeights = cms.EDProducer("GenWeightProductProducer",
    genInfo = cms.InputTag("generator"),
    genLumiInfoHeader = cms.InputTag("generator"))

#process.p = cms.Path(process.testLHEWeights*process.testGenWeights)
process.p = cms.Path(process.testGenWeights)

process.output = cms.EndPath(process.out)
process.schedule = cms.Schedule(process.p,process.output)

