import FWCore.ParameterSet.Config as cms
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

L1SMonitor = DQMEDAnalyzer('L1SMonitor',
    muonsTag = cms.InputTag('l1ScGmtUnpacker', 'Muon', 'SCHLP'),
    jetsTag = cms.InputTag("l1ScCaloUnpacker", "Jet", "SCHLP"),
    eGammasTag = cms.InputTag("l1ScCaloUnpacker", "EGamma", "SCHLP"),
    tausTag = cms.InputTag("l1ScCaloUnpacker", "Tau", "SCHLP"),
)
