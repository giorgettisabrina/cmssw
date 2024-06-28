import FWCore.ParameterSet.Config as cms
from DQM.L1SMonitor.L1SMonitor_cfi import L1SMonitor

l1sMonitorSequence = cms.Sequence(L1SMonitor)
