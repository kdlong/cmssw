from PhysicsTools.NanoAOD.taus_cff import *
from PhysicsTools.NanoAOD.jets_cff import *
from PhysicsTools.NanoAOD.globals_cff import *
from PhysicsTools.NanoAOD.genparticles_cff import *
from PhysicsTools.NanoAOD.particlelevel_cff import *
from PhysicsTools.NanoAOD.lheInfoTable_cfi import *
from PhysicsTools.NanoAOD.genWeightsTable_cfi import *

genWeights = cms.EDProducer("GenWeightProductProducer",
    genInfo = cms.InputTag("generator"),
    genLumiInfoHeader = cms.InputTag("generator"))

lheWeights = cms.EDProducer("LHEWeightProductProducer",
    lheSourceLabels = cms.vstring(["externalLHEProducer", "source"]),
    failIfInvalidXML = cms.untracked.bool(False)
)

lheWeightsTable = cms.EDProducer(
    "LHEWeightsTableProducer",
    lheWeights = cms.VInputTag(["externalLHEProducer", "source", "lheWeights"]),
    lheWeightPrecision = cms.int32(14),
    genWeights = cms.InputTag("genWeights"),
    # Warning: you can use a full string, but only the first character is read.
    # Note also that the capitalization is important! For example, 'parton shower' 
    # must be lower case and 'PDF' must be capital
    weightgroups = cms.vstring(['scale', 'PDF', 'matrix element', 'unknown', 'parton shower']),
    # Max number of groups to store for each type above, -1 ==> store all found
    maxGroupsPerType = cms.vint32([-1, -1, -1, -1, 1]),
    # If empty or not specified, no critieria is applied to filter on LHAPDF IDs 
    #pdfIds = cms.untracked.vint32([91400, 306000, 260000]),
    #unknownOnlyIfEmpty = cms.untracked.vstring(['scale', 'PDF']),
    #unknownOnlyIfAllEmpty = cms.untracked.bool(False),
)

nanoMetadata = cms.EDProducer("UniqueStringProducer",
    strings = cms.PSet(
        tag = cms.string("untagged"),
    )
)

metGenTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
    src = cms.InputTag("genMetTrue"),
    name = cms.string("GenMET"),
    doc = cms.string("Gen MET"),
    singleton = cms.bool(True),
    extension = cms.bool(False),
    variables = cms.PSet(
       pt  = Var("pt",  float, doc="pt", precision=10),
       phi = Var("phi", float, doc="phi", precision=10),
    ),
)

nanogenSequence = cms.Sequence(
    nanoMetadata+
    genWeights+
    lheWeights+
    particleLevel+
    genJetTable+
    patJetPartons+
    genJetFlavourAssociation+
    genJetFlavourTable+
    genJetAK8Table+
    genJetAK8FlavourAssociation+
    genJetAK8FlavourTable+
    tauGenJets+
    tauGenJetsSelectorAllHadrons+
    genVisTaus+
    genVisTauTable+
    genTable+
    genParticleTables+
    tautagger+
    rivetProducerHTXS+
    particleLevelTables+
    metGenTable+
    genWeightsTable+
    lheWeightsTable+
    lheInfoTable
)

NANOAODGENoutput = cms.OutputModule("NanoAODOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(9),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('NANOAODSIM'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('nanogen.root'),
    outputCommands = cms.untracked.vstring(
        'drop *',
        "keep nanoaodFlatTable_*Table_*_*",     # event data
        "keep String_*_genModel_*",  # generator model data
        "keep nanoaodMergeableCounterTable_*Table_*_*", # accumulated per/run or per/lumi data
        "keep nanoaodUniqueString_nanoMetadata_*_*",   # basic metadata
    )
)

def nanoGenCommonCustomize(process):
    process.lheInfoTable.storeLHEParticles = True
    process.lheInfoTable.precision = 14
    process.genJetFlavourAssociation.jets = process.genJetTable.src
    process.genJetFlavourTable.src = process.genJetTable.src
    process.genJetAK8FlavourAssociation.jets = process.genJetAK8Table.src
    process.genJetAK8FlavourTable.src = process.genJetAK8Table.src
    process.particleLevel.particleMaxEta = 999.
    process.particleLevel.lepMinPt = 0.
    process.particleLevel.lepMaxEta = 999.
    process.genJetFlavourTable.jetFlavourInfos = "genJetFlavourAssociation"

def customizeNanoGENFromMini(process):
    process.nanoAOD_step.insert(0, process.genParticles2HepMCHiggsVtx)
    process.nanoAOD_step.insert(0, process.genParticles2HepMC)
    process.nanoAOD_step.insert(0, process.mergedGenParticles)

    process.rivetProducerHTXS.HepMCCollection = "genParticles2HepMCHiggsVtx:unsmeared"
    process.genParticleTable.src = "prunedGenParticles"
    process.patJetPartons.particles = "prunedGenParticles"
    process.particleLevel.src = "genParticles2HepMC:unsmeared"

    process.genJetTable.src = "slimmedGenJets"
    process.genJetAK8Table.src = "slimmedGenJetsAK8"
    process.tauGenJets.GenParticles = "prunedGenParticles"
    process.genVisTaus.srcGenParticles = "prunedGenParticles"
    nanoGenCommonCustomize(process)

    return process

def customizeNanoGEN(process):
    process.rivetProducerHTXS.HepMCCollection = "generatorSmeared"
    process.genParticleTable.src = "genParticles"
    process.patJetPartons.particles = "genParticles"
    process.particleLevel.src = "generatorSmeared"

    process.genJetTable.src = "ak4GenJets"
    process.genJetAK8Table.src = "ak8GenJets"
    process.tauGenJets.GenParticles = "genParticles"
    process.genVisTaus.srcGenParticles = "genParticles"
    nanoGenCommonCustomize(process)
    return process

# Prune gen particles with tight conditions applied in usual NanoAOD
def pruneGenParticlesNano(process):
    process.finalGenParticles = finalGenParticles.clone()
    process.genParticleTable.src = "prunedGenParticles"
    process.patJetPartons.particles = "prunedGenParticles"
    process.nanoAOD_step.insert(0, process.finalGenParticles)
    return process

# Prune gen particles with conditions applied in usual MiniAOD
def pruneGenParticlesMini(process):
    from PhysicsTools.PatAlgos.slimming.prunedGenParticles_cfi import prunedGenParticles
    process.prunedGenParticles = prunedGenParticles.clone()
    if process.nanoAOD_step.contains(process.nanogenMiniSequence):
        raise ValueError("Applying the MiniAOD genParticle pruner to MiniAOD is redunant. " \
            "Use a different customization.")
    process.genParticleTable.src = "prunedGenParticles"
    process.patJetPartons.particles = "prunedGenParticles"
    process.nanoAOD_step.insert(0, process.prunedGenParticles)
    return process

def setGenFullPrecision(process):
    process.genParticleTable.variables.pt.precision = 23
    process.genParticleTable.variables.eta.precision = 23
    process.genParticleTable.variables.phi.precision = 23
    process.genJetTable.variables.pt.precision = 23
    process.genJetTable.variables.eta.precision = 23
    process.genJetTable.variables.phi.precision = 23
    process.metGenTable.variables.pt.precision = 23
    process.metGenTable.variables.phi.precision = 23
    return process

def setLHEFullPrecision(process):
    process.lheInfoTable.precision = 23
    return process

def setGenWeightsFullPrecision(process):
    process.genWeightsTable.lheWeightPrecision = 23
    return process
