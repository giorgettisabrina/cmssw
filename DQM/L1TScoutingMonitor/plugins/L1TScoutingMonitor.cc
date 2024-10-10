// -*- C++ -*-
//
// Package:    DQM/L1TScoutingMonitor
// Class:      L1TScoutingMonitor
//
/**\class L1TScoutingMonitor L1TScoutingMonitor.cc DQM/L1TScoutingMonitor/plugins/L1TScoutingMonitor.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  root
//         Created:  Fri, 28 Jun 2024 07:36:32 GMT
//

// Include the header file
#include "DQM/L1TScoutingMonitor/plugins/L1TScoutingMonitor.h"

// constructors and destructor
L1TScoutingMonitor::L1TScoutingMonitor(const edm::ParameterSet& iConfig)
    : muonsTokenData_(consumes(iConfig.getParameter<edm::InputTag>("muonsTag"))),
      jetsTokenData_(consumes(iConfig.getParameter<edm::InputTag>("jetsTag"))),
      eGammasTokenData_(consumes(iConfig.getParameter<edm::InputTag>("eGammasTag"))),
      tausTokenData_(consumes(iConfig.getParameter<edm::InputTag>("tausTag"))),
      m_dqm_path(iConfig.getUntrackedParameter<std::string>("dqmPath")) {
  //now do what ever initialization is needed
}

L1TScoutingMonitor::~L1TScoutingMonitor() {
}

// Implementations of virtual functions
void L1TScoutingMonitor::bookHistograms(DQMStore::IBooker& ibook,
                                edm::Run const& run,
                                edm::EventSetup const& iSetup,
                                Histograms& histos) const {
  ibook.setCurrentFolder(m_dqm_path);
  histos.histo_muon_BXocc_ = ibook.book1D("MuonBxOcc", "MuonBxOcc",  s_bx_range + 1, -0.5, s_bx_range + 0.5);
  histos.histo_jet_BXocc_ = ibook.book1D("JetBxOcc", "JetBxOcc",  s_bx_range + 1, -0.5, s_bx_range + 0.5);
  histos.histo_egamma_BXocc_ = ibook.book1D("EgammaBxOcc", "EgammaBxOcc",  s_bx_range + 1, -0.5, s_bx_range + 0.5);
  histos.histo_tau_BXocc_ = ibook.book1D("TauBxOcc", "TauBxOcc",  s_bx_range + 1, -0.5, s_bx_range + 0.5);
}

void L1TScoutingMonitor::dqmAnalyze(edm::Event const& iEvent,
                            edm::EventSetup const& iSetup,
                            Histograms const& histos) const {
  // Placeholder implementation for dqmAnalyze function
  edm::Handle<OrbitCollection<l1ScoutingRun3::Muon>> muonsCollection; 
  edm::Handle<OrbitCollection<l1ScoutingRun3::Jet>> jetsCollection; 
  edm::Handle<OrbitCollection<l1ScoutingRun3::EGamma>> eGammasCollection; 
  edm::Handle<OrbitCollection<l1ScoutingRun3::Tau>> tausCollection; 

  iEvent.getByToken(muonsTokenData_, muonsCollection);
  iEvent.getByToken(jetsTokenData_, jetsCollection); 
  iEvent.getByToken(eGammasTokenData_, eGammasCollection); 
  iEvent.getByToken(tausTokenData_, tausCollection); 

  for (const unsigned& bx : muonsCollection->getFilledBxs()) {
    histos.histo_muon_BXocc_ -> Fill(bx);
  }

  for (const unsigned& bx : jetsCollection->getFilledBxs()) {
    histos.histo_jet_BXocc_ -> Fill(bx);
  }

  for (const unsigned& bx : eGammasCollection->getFilledBxs()) {
    histos.histo_egamma_BXocc_ -> Fill(bx);
  }

  for (const unsigned& bx : tausCollection->getFilledBxs()) {
    histos.histo_tau_BXocc_ -> Fill(bx);
  }
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void L1TScoutingMonitor::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  //desc.add<edm::InputTag>("muonsTag", edm::InputTag("l1ScGmtUnpacker"));
  //desc.add<edm::InputTag>("jetsTag", edm::InputTag("l1ScCaloUnpacker"));
  //desc.add<edm::InputTag>("eGammasTag", edm::InputTag("l1ScCaloUnpacker"));
  //desc.add<edm::InputTag>("tausTag", edm::InputTag("l1ScCaloUnpacker"));
  //desc.addUntracked<std::string>("dqmPath", "/L1Scouting/BX/Occupancy");
  //descriptions.add("L1TScoutingMonitor", desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(L1TScoutingMonitor);
