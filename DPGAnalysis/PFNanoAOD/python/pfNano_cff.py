import FWCore.ParameterSet.Config as cms

def customizeOutputForPF(process):
    import re
    output = [x for x in dir(process) if re.match(".*output$", x)]
    if not output:
        raise RuntimeError("No output module found in process. This shouldn't be possible")
    outputMod = getattr(process, output[0])
    outputMod.outputCommands.extend([
        "keep *_*_MergedCaloTruth_*", 
        "keep *_*_MergedTrackTruth_*",
        'keep recoPFRecTracks_*_*_*',
        'keep recoPFRecHits_*_*_*',
        "keep *_simSiStripDigis_*_*",
        "keep *_simSiPixelDigis_*_*",
        "keep *_particleFlowRecHitECAL_*_*",
        "keep *_particleFlowRecHitHBHE_*_*",
        "keep *_particleFlowRecHitHF_*_*",
        "keep *_particleFlowRecHitHO_*_*",
        "keep *_particleFlowClusterPS_*_*",
    ])
    return process

