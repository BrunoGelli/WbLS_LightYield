#include "G4SteppingAction.hh"

#include "G4OpticalPhoton.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4VPhysicalVolume.hh"

#include "G4EventAction.hh"

G4SteppingAction::G4SteppingAction(G4EventAction* eventAction, const DetectorConfig& config)
    : fEventAction(eventAction), fConfig(config) {}

void G4SteppingAction::UserSteppingAction(const G4Step* step) {
  const auto* pre = step->GetPreStepPoint();
  const auto* volume = pre->GetPhysicalVolume();
  if (!volume) return;

  auto* track = step->GetTrack();
  if (volume->GetName() == "WbLS") {
    fEventAction->AddWbLSEdep(step->GetTotalEnergyDeposit());
  }

  if (track->GetDefinition() == G4OpticalPhoton::Definition() &&
      fConfig.maxOpticalReflections > 0 &&
      track->GetCurrentStepNumber() > fConfig.maxOpticalReflections) {
    track->SetTrackStatus(fStopAndKill);
  }
}
