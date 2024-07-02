import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
from DQM.L1SMonitor.L1SMonitor_cfi import L1SMonitor

# Define command-line options
options = VarParsing.VarParsing('analysis')
options.register('nThreads', 4, options.multiplicity.singleton, options.varType.int, 'number of threads')
options.register('nStreams', 4, options.multiplicity.singleton, options.varType.int, 'number of streams')
options.parseArguments()

# Initialize process
process = cms.Process("scDQM")

# Load necessary modules
process.load("DQMServices.Core.DQMStore_cfi")

# Set options for number of threads and streams
process.options = cms.untracked.PSet(
    numberOfThreads = cms.untracked.uint32(options.nThreads),
    numberOfStreams = cms.untracked.uint32(options.nStreams),
    numberOfConcurrentLuminosityBlocks = cms.untracked.uint32(1),
)

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100000

# Maximum number of events to process
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

# Source definition (example file in the same folder as the script)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'root://xrootd-cms.infn.it//store/data/Run2024D/L1Scouting/L1SCOUT/v1/000/380/346/00000/1648bf25-aa3d-4baa-a4af-aaceaa36774c.root',  
        #'file:run380346_cc332979-2afa-48f9-b02e-cfca4d3a22d7.root',  # Adjust path here
    )
)

from DQM.L1SMonitor.L1SMonitor_cfi import L1SMonitor


# Load L1SMonitor configuration
process.L1SMonitor = L1SMonitor

"""
L1SMonitor.clone(
    muonsTag = cms.InputTag('l1ScGmtUnpacker', 'Muon', 'SCHLP'),
    jetsTag = cms.InputTag("l1ScCaloUnpacker", "Jet", "SCHLP"),
    eGammasTag = cms.InputTag("l1ScCaloUnpacker", "EGamma", "SCHLP"),
    tausTag = cms.InputTag("l1ScCaloUnpacker", "Tau", "SCHLP"),
)
"""



# Example DQMFileSaverPB module
process.dqmFileSaverPB = cms.EDAnalyzer("DQMFileSaverPB",
    streamLabel = cms.untracked.string("streamDQMHistograms"),
    referenceHandling = cms.untracked.string("all"),
    producer = cms.untracked.string('DQM'),
    fakeFilterUnitMode = cms.untracked.bool(True),
    tag = cms.untracked.string('UNKNOWN'),
    referenceRequireStatus = cms.untracked.int32(100),
    path = cms.untracked.string("./")
)

# End path configuration
process.endp = cms.EndPath(process.L1SMonitor + process.dqmFileSaverPB)
