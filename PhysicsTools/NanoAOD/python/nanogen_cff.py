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
    lheSourceLabel = cms.string("externalLHEProducer"))

lheWeightsTable = cms.EDProducer(
    "LHEWeightsTableProducer",
    lheWeights = cms.VInputTag(["externalLHEProducer", "lheWeights"]),
    lheWeightPrecision = cms.int32(14),
    genWeights = cms.InputTag("genWeights"),
    # Warning: you can use a full string, but only the first character is read.
    # Note also that the capitalization is important! For example, 'parton shower' 
    # must be lower case and 'PDF' must be capital
    weightgroups = cms.vstring(['scale', 'PDF', 'matrix element', 'unknown', 'parton shower']),
    # Max number of groups to store for each type above, -1 ==> store all found
    maxGroupsPerType = cms.vint32([1, -1, 1, 2, 1]),
    # If empty or not specified, no critieria is applied to filter on LHAPDF IDs 
    pdfIds = cms.untracked.vint32([91400, 306000, 260000]),
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
    genWeights+
    lheWeights+
    nanoMetadata+
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
    genWeightsTable+
    lheWeightsTable+
    genParticleTables+
    tautagger+
    rivetProducerHTXS+
    particleLevelTables+
    metGenTable+
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
        "keep *_lheWeightsTable_*_*",     # event data
        "keep nanoaodFlatTable_*Table_*_*",     # event data
        "keep String_*_genModel_*",  # generator model data
        "keep nanoaodMergeableCounterTable_*Table_*_*", # accumulated per/run or per/lumi data
        "keep nanoaodUniqueString_nanoMetadata_*_*",   # basic metadata
    )
)

def customizeNanoGEN(process):
    process.genParticleTable.src = "genParticles"
    process.patJetPartons.particles = "genParticles"
    process.particleLevel.src = "generatorSmeared"
    process.rivetProducerHTXS.HepMCCollection = "generatorSmeared"

    process.genJetTable.src = "ak4GenJetsNoNu"
    process.genJetFlavourAssociation.jets = process.genJetTable.src
    process.genJetFlavourTable.src = process.genJetTable.src
    process.genJetFlavourTable.jetFlavourInfos = "genJetFlavourAssociation"
    process.genJetAK8Table.src = "ak8GenJetsNoNu"
    process.genJetAK8FlavourAssociation.jets = process.genJetAK8Table.src
    process.genJetAK8FlavourTable.src = process.genJetAK8Table.src
    process.tauGenJets.GenParticles = "genParticles"
    process.genVisTaus.srcGenParticles = "genParticles"
    return process
