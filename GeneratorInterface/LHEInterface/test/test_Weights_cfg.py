# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: Configuration/GenProduction/python/HIG-RunIIFall18wmLHEGS-00509-fragment.py --fileout file:HIG-RunIIFall18wmLHEGS-00509.root --mc --eventcontent LHE --datatier LHE --conditions 102X_upgrade2018_realistic_v11 --step LHE --python_filename HIG-RunIIFall18wmLHEGS-00509_1_cfg.py --no_exec
import FWCore.ParameterSet.Config as cms



process = cms.Process('LHE')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

# Input source
process.source = cms.Source("EmptySource")

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('Configuration/GenProduction/python/HIG-RunIIFall18wmLHEGS-00509-fragment.py nevts:10'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition
process.LHEoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('LHE'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('file:HIG-RunIIFall18wmLHEGS-00509.root'),
    #outputCommands = process.LHEEventContent.outputCommands,
    outputCommands = cms.untracked.vstring('keep *', 
        'drop ME*_MEtoEDM*_*_*'),
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

process.externalLHEProducer = cms.EDProducer("ExternalLHEProducer",
                                             #args = cms.vstring('/afs/cern.ch/user/k/kelong/work/public/DummyGridpacks/WLLJJ_WToLNu_EWK_4F_MLL-60_slc6_amd64_gcc481_CMSSW_7_1_30_tarball_Dummy.tgz'),
                                             args = cms.vstring('/afs/cern.ch/user/k/kelong/work/public/DummyGridpacks/VVV_aQGCfs_dummy.tgz'),
                                             nEvents = cms.untracked.uint32(10),
    numberOfParameters = cms.uint32(1),
    outputFile = cms.string('cmsgrid_final.lhe'),
    scriptName = cms.FileInPath('GeneratorInterface/LHEInterface/data/run_generic_tarball_cvmfs.sh')
)

# args = cms.vstring('/afs/cern.ch/user/k/kelong/work/public/DummyGridpacks/DarkMatter_MonoZPrime_V_Mx50_Mv500_gDMgQ1_LO_slc6_amd64_gcc481_CMSSW_7_1_30_tarball_Dummy.tgz'),
# args = cms.vstring('/afs/cern.ch/user/k/kelong/work/public/DummyGridpacks/WLLJJ_WToLNu_EWK_4F_MLL-60_slc6_amd64_gcc481_CMSSW_7_1_30_tarball_Dummy.tgz'),
# args = cms.vstring('/afs/cern.ch/user/k/kelong/work/public/DummyGridpacks/ZZ_4L_NNPDF30_13TeV_tarballDummy.tar.gz'),
# args = cms.vstring('/afs/cern.ch/user/k/kelong/work/public/DummyGridpacks/ZZ_slc6_amd64_gcc630_CMSSW_9_3_0_ZZTo4L2017_pdf306000Dummy.tgz'),



# Path and EndPath definitions
process.lhe_step = cms.Path(process.externalLHEProducer)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.LHEoutput_step = cms.EndPath(process.LHEoutput)

# Schedule definition
process.schedule = cms.Schedule(process.lhe_step,process.endjob_step,process.LHEoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)



 
# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
