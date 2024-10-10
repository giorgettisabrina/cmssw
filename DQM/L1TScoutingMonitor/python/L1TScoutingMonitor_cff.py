import FWCore.ParameterSet.Config as cms
from DQM.L1TScoutingMonitor.L1TScoutingMonitor_cfi import L1TScoutingMonitor

l1sMonitorSequence = cms.Sequence(L1TScoutingMonitor)
