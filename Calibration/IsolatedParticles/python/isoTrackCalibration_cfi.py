import FWCore.ParameterSet.Config as cms

IsoTrackCalibration = cms.EDAnalyzer("IsoTrackCalibration",
                                     Verbosity       = cms.untracked.int32( 0 ),
                                     Triggers        = cms.untracked.vstring("HLT_IsoTrackHB"),
                                     TrackQuality    = cms.untracked.string("highPurity"),
                                     ProcessName     = cms.untracked.string("HLT"),
                                     L1Filter        = cms.untracked.string(""),
                                     L2Filter        = cms.untracked.string("L2Filter"),
                                     L3Filter        = cms.untracked.string("Filter"),
                                     MinTrackPt      = cms.untracked.double(10.0),
                                     MaxDxyPV        = cms.untracked.double(0.02),
                                     MaxDzPV         = cms.untracked.double(0.02),
                                     MaxChi2         = cms.untracked.double(5.0),
                                     MaxDpOverP      = cms.untracked.double(0.1),
                                     MinOuterHit     = cms.untracked.int32(4),
                                     MinLayerCrossed =cms.untracked.int32(8),
                                     MaxInMiss       = cms.untracked.int32(0),
                                     MaxOutMiss      = cms.untracked.int32(0),
                                     ConeRadius      = cms.untracked.double(34.98),
                                     ConeRadiusMIP   = cms.untracked.double(14.0),
                                     IsItQCDMC       = cms.untracked.bool(False),
                                     IsItAOD         = cms.untracked.bool(False),
                                     JetSource       = cms.InputTag("ak5GenJets"),
)
