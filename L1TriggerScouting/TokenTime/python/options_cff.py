# import l1scouting options
from L1TriggerScouting.Phase2.options_cff import options, VarParsing

# extra options for the PUPPI tokenizer timing (TokenTimeML)
options.register ("pipelineStep",
    "tokenizing",
    VarParsing.VarParsing.multiplicity.singleton,
    VarParsing.VarParsing.varType.string,
    "Step up to which run the pipeline (unpacking, tokenizing)."
)

options.register ("model",
    "L1TriggerScouting/TokenTime/data/orbit_puppi128_tt_vq4096_jit.pt",
    VarParsing.VarParsing.multiplicity.singleton,
    VarParsing.VarParsing.varType.string,
    "TorchScript tokenizer model (path relative to CMSSW src, resolved with FileInPath)."
)

options.register ("batchSize",
    512,
    VarParsing.VarParsing.multiplicity.singleton,
    VarParsing.VarParsing.varType.int,
    "Batch size (in terms of bx) for model inference."
)

options.register ("reportEvery",
    100,
    VarParsing.VarParsing.multiplicity.singleton,
    VarParsing.VarParsing.varType.int,
    "Print message after the specified number of events (proper events in this case)"
)

options.register ("tracePerfetto",
    False,
    VarParsing.VarParsing.multiplicity.singleton,
    VarParsing.VarParsing.varType.bool,
    "Generate trace with Perfetto"
)
