#include "G4RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

G4RunAction::G4RunAction() {
  auto* analysis = G4AnalysisManager::Instance();
  analysis->SetDefaultFileType("root");
  analysis->SetFileName("wbls_lightyield");
  analysis->SetVerboseLevel(1);

  analysis->CreateNtuple("events", "per-event light-yield summary");
  analysis->CreateNtupleIColumn("eventID");
  analysis->CreateNtupleSColumn("sourceType");
  analysis->CreateNtupleDColumn("primaryGammaEnergy_keV");
  analysis->CreateNtupleDColumn("edepWbLS_MeV");
  analysis->CreateNtupleIColumn("nOpticalProduced");
  analysis->CreateNtupleIColumn("nCherenkovProduced");
  analysis->CreateNtupleIColumn("nScintProduced");
  analysis->CreateNtupleIColumn("nDetectedLargePMT");
  analysis->CreateNtupleIColumn("nDetectedSmallPMT");
  analysis->CreateNtupleIColumn("nDetectedLargeCherenkov");
  analysis->CreateNtupleIColumn("nDetectedLargeScintillation");
  analysis->CreateNtupleIColumn("nDetectedSmallCherenkov");
  analysis->CreateNtupleIColumn("nDetectedSmallScintillation");
  analysis->FinishNtuple();

  analysis->CreateNtuple("photons", "detected optical photons");
  analysis->CreateNtupleIColumn("eventID");
  analysis->CreateNtupleIColumn("pmtID");
  analysis->CreateNtupleDColumn("wavelength_nm");
  analysis->CreateNtupleDColumn("time_ns");
  analysis->CreateNtupleIColumn("creationProcessCode");
  analysis->CreateNtupleSColumn("creatorProcessName");
  analysis->CreateNtupleDColumn("hitX_mm");
  analysis->CreateNtupleDColumn("hitY_mm");
  analysis->CreateNtupleDColumn("hitZ_mm");
  analysis->FinishNtuple();
}

G4RunAction::~G4RunAction() {
  delete G4AnalysisManager::Instance();
}

void G4RunAction::BeginOfRunAction(const G4Run*) {
  G4AnalysisManager::Instance()->OpenFile();
}

void G4RunAction::EndOfRunAction(const G4Run*) {
  auto* analysis = G4AnalysisManager::Instance();
  analysis->Write();
  analysis->CloseFile();
}
