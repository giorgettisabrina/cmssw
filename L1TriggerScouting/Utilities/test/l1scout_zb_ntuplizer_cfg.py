import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

options = VarParsing.VarParsing ('analysis')

options.parseArguments()

process = cms.Process( "DUMP" )


process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(-1) #-1 
)

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.source = cms.Source("PoolSource",
  #fileNames = cms.untracked.vstring(options.inputFiles)
  #fileNames = cms.untracked.vstring('')
  #MENUSTUDIES
  #run386604 lumi 100 
  fileNames = cms.untracked.vstring('root://cms-xrd-global.cern.ch//store/data/Run2024I/L1Scouting/L1SCOUT/v1/000/386/604/00000/81ad0277-e7c0-42a2-bfd6-195f50c92223.root')
  #run386604 lumi 101
  #fileNames = cms.untracked.vstring('root://cms-xrd-global.cern.ch//store/data/Run2024I/L1Scouting/L1SCOUT/v1/000/386/604/00000/4323e051-55e9-4260-bea7-34c51c976a8a.root')
  #link: https://cmsweb.cern.ch/das/request?view=list&limit=50&instance=prod%2Fglobal&input=file+dataset%3D%2FL1Scouting%2FRun2024I-v1%2FL1SCOUT+run%3D386604+lumi%3D102
)

selbx = None
saveStubs = False

PhysicalVarsFlag = False
HardwareVarsFlag = True

process.scMuonTable = cms.EDProducer("ConvertScoutingMuonsToOrbitFlatTable",
  src = cms.InputTag("FinalBxSelectorMuon" if selbx else "l1ScGmtUnpacker", "Muon"),
  name = cms.string("L1Mu"),
  doc = cms.string("Muons from GMT"),
  PhysicalVars = cms.bool(PhysicalVarsFlag),  # Physical variables flag
  HardwareVars = cms.bool(HardwareVarsFlag)  # Hardware variables flag
)
process.scJetTable = cms.EDProducer("ConvertScoutingJetsToOrbitFlatTable",
  src = cms.InputTag("FinalBxSelectorJet" if selbx else "l1ScCaloUnpacker", "Jet"),
  name = cms.string("L1Jet"),
  doc = cms.string("Jets from Calo Demux"),
  PhysicalVars = cms.bool(PhysicalVarsFlag),  # Physical variables flag
  HardwareVars = cms.bool(HardwareVarsFlag)  # Hardware variables flag
)
process.scEgammaTable = cms.EDProducer("ConvertScoutingEGammasToOrbitFlatTable",
  src = cms.InputTag("FinalBxSelectorEGamma" if selbx else "l1ScCaloUnpacker", "EGamma"),
  name = cms.string("L1EG"),
  doc = cms.string("EGammas from Calo Demux"),
  PhysicalVars = cms.bool(PhysicalVarsFlag),  # Physical variables flag
  HardwareVars = cms.bool(HardwareVarsFlag)  # Hardware variables flag
)
process.scTauTable = cms.EDProducer("ConvertScoutingTausToOrbitFlatTable",
  src = cms.InputTag("l1ScCaloUnpacker", "Tau"),
  name = cms.string("L1Tau"),
  doc = cms.string("Taus from Calo Demux"),
  PhysicalVars = cms.bool(PhysicalVarsFlag),  # Physical variables flag
  HardwareVars = cms.bool(HardwareVarsFlag)  # Hardware variables flag
)
process.scStubsTable = cms.EDProducer("ConvertScoutingStubsToOrbitFlatTable",
  src = cms.InputTag("FinalBxSelectorBMTFStub" if selbx else "l1ScBMTFUnpacker", "BMTFStub"),
  name = cms.string("L1BMTFStub"),
  doc = cms.string("Stubs from BMTF"),
)
process.scSumTable = cms.EDProducer("ConvertScoutingSumsToOrbitFlatTable",
  src = cms.InputTag("FinalBxSelectorBxSums" if selbx else "l1ScCaloUnpacker", "EtSum"),
  name = cms.string("L1EtSum"),
  doc = cms.string("Sums from Calo Demux"),
  #singleObject = cms.bool(False),
  writeHF = cms.bool(True),
  writeMinBias = cms.bool(False),
  writeCentrality = cms.bool(False),
  writeAsym = cms.bool(False),
  #Issue to fix with SUMS when sigleObjects=False: VarsFlag cannot be both true 
  singleObject = cms.bool(False),
  PhysicalVars = cms.bool(False),  # Physical variables flag
  HardwareVars = cms.bool(True)  # Hardware variables flag
)
process.p = cms.Path(
  process.scMuonTable +
  process.scJetTable +
  process.scEgammaTable +
  process.scTauTable +
  #process.scStubsTable +
  process.scSumTable
)

process.out = cms.OutputModule("OrbitNanoAODOutputModule",
    fileName = cms.untracked.string(options.outputFile),
    skipEmptyBXs = cms.bool(True),
    SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('p')),
    outputCommands = cms.untracked.vstring("drop *", "keep l1ScoutingRun3OrbitFlatTable_*_*_*"),
    compressionLevel = cms.untracked.int32(5),
    compressionAlgorithm = cms.untracked.string("ZSTD"),
)

if not saveStubs:
  process.p.remove(process.scStubsTable)
if selbx:
  process.p.remove(process.scTauTable)
  process.out.outputCommands += [ "keep uints_*_SelBx_*" ]
  if selbx != "any":
    process.out.selectedBx = cms.InputTag(selbx, "SelBx")
    process.out.skipEmptyBXs = False
  else:
    process.out.selectedBx = cms.InputTag("FinalBxSelector", "SelBx")
    process.out.skipEmptyBXs = False

process.o = cms.EndPath(
  process.out
)
