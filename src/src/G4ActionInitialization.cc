#include "G4ActionInitialization.hh"

#include "G4EventAction.hh"
#include "G4PrimaryGeneratorAction.hh"
#include "G4RunAction.hh"
#include "G4SteppingAction.hh"
#include "TrackingAction.hh"

G4ActionInitialization::G4ActionInitialization(const DetectorConfig& config)
    : G4VUserActionInitialization(), fConfig(config) {}

void G4ActionInitialization::BuildForMaster() const {
  SetUserAction(new G4RunAction());
}

void G4ActionInitialization::Build() const {
  auto* eventAction = new G4EventAction();
  SetUserAction(new G4PrimaryGeneratorAction());
  SetUserAction(new G4RunAction());
  SetUserAction(eventAction);
  SetUserAction(new G4SteppingAction(eventAction, fConfig));
  SetUserAction(new TrackingAction(eventAction));
}
