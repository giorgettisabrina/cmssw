#!/bin/bash
# to streamline debugging and timing of the PUPPI tokenizer (TokenTimeML)
# usage: bash L1TriggerScouting/TokenTime/test/runScoutingPhase2TokenTime.sh <cpu|cuda> <num_events> <num_threads> <num_streams> <run_number> <unpacking|tokenizing> [trace]
# example usage: bash L1TriggerScouting/TokenTime/test/runScoutingPhase2TokenTime.sh cuda 100 4 4 40 tokenizing
# the raw data directory can be overridden with DATA=/path/to/raw
function die { echo Failed $1: status $2 ; exit $2 ; }

SCRIPT="L1TriggerScouting/TokenTime/test/runScoutingPhase2TokenTime.py"
DATA="${DATA:-/mnt/ramdisk/gizago/raw}"
TRACE="False"

USAGE="1st ('cpu', 'cuda'), 2nd ('num_events'), 3rd ('num_threads'), 4th ('num_streams'), 5th ('run_number'), 6th ('unpacking', 'tokenizing'), 7th ('trace' [optional])"

if [ "$#" -lt "6" ]; then
    die "Need at least 6 arguments: ${USAGE}; got $#" 1
elif [ "$#" -eq "7" ]; then
    if [[ "$7" =~ ^(trace)$ ]]; then
        TRACE="True"
        if [ "$2" -gt "40" ]; then
            die "Trying to run a Perfetto-traced job with more than 40 events, not allowed!" 1
        fi
    else
        die "Argument needs to be 'trace'; got '$7'" 1
    fi
elif [ "$#" -gt "7" ]; then
    die "Need at most 7 arguments: ${USAGE}; got $#" 1
fi
if [[ "$1" =~ ^(cpu|cuda)$ ]]; then
    TARGET=$1
else
    die "Argument needs to be 'cpu' or 'cuda'; got '$1'" 1
fi
if ! [[ "$6" =~ ^(unpacking|tokenizing)$ ]]; then
    die "Argument needs to be 'unpacking' or 'tokenizing'; got '$6'" 1
fi

if [ "${TARGET}" == "cpu" ]; then
  echo "Running CPU-only test"
  BACKEND=serial_sync
else
  echo "Running CUDA test"
  BACKEND=cuda_async
fi

cmsRun "${SCRIPT}" backend=${BACKEND} buBaseDir=${DATA} fuBaseDir=${DATA} buNumStreams=2 maxEvents=$2 numThreads=$3 numFwkStreams=$4 runNumber=$5 pipelineStep=$6 tracePerfetto="${TRACE}"
