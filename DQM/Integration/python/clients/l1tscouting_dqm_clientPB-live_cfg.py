import FWCore.ParameterSet.Config as cms
import sys

from Configuration.Eras.Era_Run3_cff import Run3
process = cms.Process("HARVESTING", Run3)


unitTest = False
if 'unitTest=True' in sys.argv:
	unitTest=True

#----------------------------
#### Histograms Source
#----------------------------

if unitTest:
   process.load("DQM.Integration.config.unittestinputsource_cfi")
   from DQM.Integration.config.unittestinputsource_cfi import options
else:
   # for live online DQM in P5
   process.load("DQM.Integration.config.pbsource_cfi")
   from DQM.Integration.config.pbsource_cfi import options

#----------------------------
#### DQM Environment
#----------------------------
process.load("DQM.Integration.config.environment_cfi")
process.dqmEnv.subSystemFolder = 'L1Scouting'
process.dqmEnv.eventInfoFolder = 'EventInfo'
process.dqmSaver.tag = 'L1Scouting' 
#process.dqmSaver.path = './L1Scouting'
process.dqmSaver.runNumber = options.runNumber
process.dqmSaverPB.tag = 'L1Scouting'
process.dqmSaverPB.runNumber = options.runNumber
#-----------------------------

# customise for playback
if process.dqmRunConfig.type.value() == "playback":
    process.dqmEnv.eventInfoFolder = 'EventInfo/Random'

# DQM Modules
process.load('DQM.L1TScoutingMonitor.L1TScoutingMonitor_cfi')


print("Final Source settings:", process.source)
process.p = cms.EndPath(process.L1TScoutingMonitor + process.dqmEnv + process.dqmSaver + process.dqmSaverPB )

### process customizations included here
from DQM.Integration.config.online_customizations_cfi import *
process = customise(process)
print("Final Source settings:", process.source)
