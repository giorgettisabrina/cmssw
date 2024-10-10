import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
from DQM.L1TScoutingMonitor.L1TScoutingMonitor_cfi import L1TScoutingMonitor

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
        #'root://xrootd-cms.infn.it//store/data/Run2024D/L1Scouting/L1SCOUT/v1/000/380/346/00000/1648bf25-aa3d-4baa-a4af-aaceaa36774c.root',  
        #'root://xrootd-cms.infn.it//store/data/Run2024D/L1Scouting/L1SCOUT/v1/000/380/649/00000/200565df-dd66-4673-908a-3974db703921.root',
        #'root://xrootd-cms.infn.it//store/data/Run2024F/L1Scouting/L1SCOUT/v1/000/383/175/00000/000ed7ac-eae8-412e-bcc4-0818a47543b2.root'
        #'root://cms-xrd-global.cern.ch//store/data/Run2024G/L1Scouting/L1SCOUT/v1/000/385/415/00000/0839bff8-1f63-4d39-8383-f223a26f4173.root'
        'root://cms-xrd-global.cern.ch//store/data/Run2024G/L1Scouting/L1SCOUT/v1/000/385/415/00000/022a94be-83df-41cd-b4f6-1fd9481ffa83.root'
        #'root://cms-xrd-global.cern.ch//store/data/Run2024G/L1Scouting/L1SCOUT/v1/000/385/415/00000/08043bc4-c19d-4c5c-a5a4-0fd44d8bf016.root'
        #'root://cms-xrd-global.cern.ch/'
        #'file:008f5349-0862-4c23-ba4e-1a2a3a230c4b_run383779.root'
        #'file:3b4651ce-2e49-4541-a136-848c1fc293d2_run383779_2ls.root'
        #'file:run380346_cc332979-2afa-48f9-b02e-cfca4d3a22d7.root',  # Adjust path here
    )
)

from DQM.L1TScoutingMonitor.L1TScoutingMonitor_cfi import L1TScoutingMonitor


# Load L1SMonitor configuration
process.L1TScoutingMonitor = L1TScoutingMonitor


#test ... 'root://xrootd-cms.infn.it//store/data/Run2024F/L1Scouting/L1SCOUT/v1/000/383/175/00000/02754680-7a88-461b-8ea7-8ced77220c84.root'
process.DQMStore = cms.Service( "DQMStore",
    #enableMultiThread = cms.untracked.bool( True ),
    #trackME = cms.untracked.string( "" ),
    saveByLumi = cms.untracked.bool( True ),
    verbose = cms.untracked.int32( 0 )
)

# Example DQMFileSaverPB module
process.dqmFileSaverPB = cms.EDAnalyzer("DQMFileSaverPB",
    streamLabel = cms.untracked.string("streamDQMHistograms"),
    referenceHandling = cms.untracked.string("all"),
    producer = cms.untracked.string('DQM'),
    fakeFilterUnitMode = cms.untracked.bool(True), #True
    tag = cms.untracked.string('UNKNOWN'), #leave unknown, output name based on ls 
    referenceRequireStatus = cms.untracked.int32(100),
    path = cms.untracked.string("./")
)

process.dqmFileSaver = cms.EDAnalyzer( "DQMFileSaver",
    convention        = cms.untracked.string( "Online" ),
    workflow          = cms.untracked.string( "/L1Scouting/BX/Occupancy" ),
    dirName           = cms.untracked.string( "/" ),
    saveByRun         = cms.untracked.int32(1),
    saveByLumiSection = cms.untracked.int32(-1), 
    saveByEvent       = cms.untracked.int32(-1),
    saveByTime        = cms.untracked.int32(-1),
    saveByMinute      = cms.untracked.int32(-1),
    saveAtJobEnd      = cms.untracked.bool(False),
    forceRunNumber    = cms.untracked.int32(-1),
)

process.dqmFileSaver.saveByLumiSection=True




# End path configuration
process.endp = cms.EndPath(process.L1TScoutingMonitor  
                            + process.dqmFileSaverPB 


                            
                            + process.dqmFileSaver
                            )


