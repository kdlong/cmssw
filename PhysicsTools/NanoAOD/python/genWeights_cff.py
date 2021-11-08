import FWCore.ParameterSet.Config as cms

genWeights = cms.EDProducer("GenWeightProductProducer",
    genInfo = cms.InputTag("generator"),
    genLumiInfoHeader = cms.InputTag("generator"),
    debug = cms.untracked.bool(False),
)

lheWeights = cms.EDProducer("LHEWeightProductProducer",
    lheSourceLabels = cms.vstring(["externalLHEProducer", "source"]),
    failIfInvalidXML = cms.untracked.bool(False),
    debug = cms.untracked.bool(False),
)

genWeightsTable = cms.EDProducer(
    "GenWeightsTableProducer",
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
    keepAllPSWeights = cms.bool(False)
)

genWeightsTables = cms.Sequence(lheWeights*genWeights*genWeightsTable)
