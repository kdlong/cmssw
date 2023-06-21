import FWCore.ParameterSet.Config as cms

from DPGAnalysis.CaloNanoAOD.ecalSimHitTables_cff import *
from DPGAnalysis.CaloNanoAOD.hcalSimHitTables_cff import *
from DPGAnalysis.CaloNanoAOD.ecalRecHitTables_cff import *
from DPGAnalysis.CaloNanoAOD.hcalRecHitTables_cff import *

caloTables = cms.Sequence(ecalSimHitTables + hcalSimHitTables 
                        + ecalRecHitTables + hcalRecHitTables)
