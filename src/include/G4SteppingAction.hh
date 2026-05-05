#ifndef G4SteppingAction_h
#define G4SteppingAction_h 1

#include "DetectorConfig.hh"
#include "G4UserSteppingAction.hh"

class G4EventAction;

class G4SteppingAction : public G4UserSteppingAction {
public:
  G4SteppingAction(G4EventAction* eventAction, const DetectorConfig& config);
  ~G4SteppingAction() override = default;

  void UserSteppingAction(const G4Step*) override;

private:
  G4EventAction* fEventAction;
  const DetectorConfig& fConfig;
};

#endif
