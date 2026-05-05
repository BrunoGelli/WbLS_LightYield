#ifndef TRACKING_ACTION_HH
#define TRACKING_ACTION_HH

#include "G4UserTrackingAction.hh"

class G4EventAction;

class TrackingAction : public G4UserTrackingAction {
public:
  explicit TrackingAction(G4EventAction* eventAction);
  ~TrackingAction() override = default;

  void PreUserTrackingAction(const G4Track* track) override;

private:
  G4EventAction* fEventAction;
};

#endif
