import FWCore.ParameterSet.Config as cms
process = cms.Process("ScoutingPhase2TokenTime")

# import TokenTime options
from L1TriggerScouting.TokenTime.options_cff import options, VarParsing

# parse arguments
options.parseArguments()

# check arguments consistency
step_mapper = {
    "unpacking": 0,
    "tokenizing": 1,
}
if options.pipelineStep not in step_mapper:
    raise ValueError("pipelineStep must be one among " + ", ".join(step_mapper))

# common framework options, accelerator support, and DAQ input
from L1TriggerScouting.Phase2.L1TScPhase2RunScouting_cff import setupPhase2Scouting
setupPhase2Scouting(process, options)

# message logger
process.MessageLogger.cerr.FwkReport.reportEvery = options.reportEvery

# disable pytorch inner threading
process.PyTorchService = cms.Service("PyTorchService")

# timing
process.load( "HLTrigger.Timer.FastTimerService_cfi" )
process.FastTimerService.printEventSummary = True
process.FastTimerService.writeJSONSummary = cms.untracked.bool(True)
process.FastTimerService.jsonFileName = cms.untracked.string(f"Phase2TokenTiming_resources.json")
process.FastTimerService.enableTimingPaths = cms.untracked.bool(True)
process.FastTimerService.enableTimingModules = cms.untracked.bool(True)
process.FastTimerService.useRealTimeClock = cms.untracked.bool(True)

# define pipeline path
process.p_pipeline = cms.Path()

# unpacking
"""
streams: stream IDs of the PUPPI stream files inside the buBaseDir(s).
Either pass pfBarrelStreamIDs/pfEndcapStreamIDs explicitly, or just buNumStreams (e.g. 2),
in which case streams = range(buNumStreams[0]).
"""
process.unpacker = cms.EDProducer("l1sc::L1TScPhase2PuppiRawToDigiV2@alpaka",
    alpaka = cms.untracked.PSet(
        backend = cms.untracked.string(options.backend)
    ),
    src = cms.InputTag("rawDataCollector"),
    streams = cms.vuint32(
        *list(range(options.buNumStreams[0]))
        if options.pfBarrelStreamIDs == [] else options.pfBarrelStreamIDs + options.pfEndcapStreamIDs
    ),
    splitFactor = cms.uint32(2)
)
process.p_pipeline += process.unpacker

# tokenizing: PUPPI candidates of each bx -> [128, N] input matrix -> one token per candidate
process.tokens = cms.EDProducer("l1sc::TokenTimeML@alpaka",
    alpaka = cms.untracked.PSet(
        backend = cms.untracked.string(options.backend)
    ),
    srcCandidates = cms.InputTag("unpacker", "candidates"),
    srcBxLookup = cms.InputTag("unpacker", "bxLookup"),
    model = cms.FileInPath(options.model),
    batchSize = cms.uint32(options.batchSize)
)

if step_mapper[options.pipelineStep] >= step_mapper["tokenizing"]:
    process.p_pipeline += process.tokens

# schedule
process.schedule = cms.Schedule(
    process.p_pipeline
)

# tracing with Perfetto
if options.tracePerfetto:
    from PerfTools.Perfetto.customisePerfetto import customisePerfetto
    customisePerfetto(process,
                fileName="trace.pftrace",
                traceFunctions=True,
                traceGpuKernels=True,
                traceAllocations=True)
