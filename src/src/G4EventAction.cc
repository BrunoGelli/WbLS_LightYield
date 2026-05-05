#include "G4EventAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

namespace {
G4int OriginCode(PhotonTrackInfo::Origin origin) {
  return static_cast<G4int>(origin);
}
}

void G4EventAction::BeginOfEventAction(const G4Event* event) {
  fEventID = event->GetEventID();
  fPrimaryEnergy = 0.0;
  fEdepWbLS = 0.0;
  fProducedOptical = fProducedCherenkov = fProducedScintillation = 0;
  fDetectedLarge = fDetectedSmall = 0;
  fDetectedLargeCherenkov = fDetectedLargeScintillation = 0;
  fDetectedSmallCherenkov = fDetectedSmallScintillation = 0;
}

void G4EventAction::EndOfEventAction(const G4Event*) {
  auto* analysis = G4AnalysisManager::Instance();
  analysis->FillNtupleIColumn(0, 0, fEventID);
  analysis->FillNtupleSColumn(0, 1, fSourceType);
  analysis->FillNtupleDColumn(0, 2, fPrimaryEnergy / keV);
  analysis->FillNtupleDColumn(0, 3, fEdepWbLS / MeV);
  analysis->FillNtupleIColumn(0, 4, fProducedOptical);
  analysis->FillNtupleIColumn(0, 5, fProducedCherenkov);
  analysis->FillNtupleIColumn(0, 6, fProducedScintillation);
  analysis->FillNtupleIColumn(0, 7, fDetectedLarge);
  analysis->FillNtupleIColumn(0, 8, fDetectedSmall);
  analysis->FillNtupleIColumn(0, 9, fDetectedLargeCherenkov);
  analysis->FillNtupleIColumn(0, 10, fDetectedLargeScintillation);
  analysis->FillNtupleIColumn(0, 11, fDetectedSmallCherenkov);
  analysis->FillNtupleIColumn(0, 12, fDetectedSmallScintillation);
  analysis->AddNtupleRow(0);
}

void G4EventAction::SetPrimary(const G4String& sourceType, G4double energy) {
  fSourceType = sourceType;
  fPrimaryEnergy = energy;
}

void G4EventAction::CountProducedPhoton(PhotonTrackInfo::Origin origin) {
  ++fProducedOptical;
  if (origin == PhotonTrackInfo::Origin::Cherenkov) ++fProducedCherenkov;
  if (origin == PhotonTrackInfo::Origin::Scintillation) ++fProducedScintillation;
}

void G4EventAction::CountDetectedPhoton(G4int pmtId, PhotonTrackInfo::Origin origin) {
  G4int* total = (pmtId == 0) ? &fDetectedLarge : &fDetectedSmall;
  G4int* cherenkov = (pmtId == 0) ? &fDetectedLargeCherenkov : &fDetectedSmallCherenkov;
  G4int* scintillation = (pmtId == 0) ? &fDetectedLargeScintillation : &fDetectedSmallScintillation;
  ++(*total);
  if (origin == PhotonTrackInfo::Origin::Cherenkov) ++(*cherenkov);
  if (origin == PhotonTrackInfo::Origin::Scintillation) ++(*scintillation);
}

void G4EventAction::FillDetectedPhoton(G4int pmtId, G4double wavelength, G4double time,
                                        PhotonTrackInfo::Origin origin, const G4String& creator,
                                        const G4ThreeVector& position) {
  auto* analysis = G4AnalysisManager::Instance();
  analysis->FillNtupleIColumn(1, 0, fEventID);
  analysis->FillNtupleIColumn(1, 1, pmtId);
  analysis->FillNtupleDColumn(1, 2, wavelength);
  analysis->FillNtupleDColumn(1, 3, time / ns);
  analysis->FillNtupleIColumn(1, 4, OriginCode(origin));
  analysis->FillNtupleSColumn(1, 5, creator);
  analysis->FillNtupleDColumn(1, 6, position.x() / mm);
  analysis->FillNtupleDColumn(1, 7, position.y() / mm);
  analysis->FillNtupleDColumn(1, 8, position.z() / mm);
  analysis->AddNtupleRow(1);
}
