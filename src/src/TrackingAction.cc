#include "TrackingAction.hh"

#include "G4OpticalPhoton.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

#include "G4EventAction.hh"
#include "PhotonTrackInfo.hh"

TrackingAction::TrackingAction(G4EventAction* eventAction) : fEventAction(eventAction) {}

void TrackingAction::PreUserTrackingAction(const G4Track* track) {
  if (track->GetDefinition() != G4OpticalPhoton::Definition()) return;

  auto origin = PhotonTrackInfo::Origin::Unknown;
  G4String creatorName = "unknown";
  if (const auto* creator = track->GetCreatorProcess()) {
    creatorName = creator->GetProcessName();
    if (creatorName == "Cerenkov") origin = PhotonTrackInfo::Origin::Cherenkov;
    if (creatorName == "Scintillation") origin = PhotonTrackInfo::Origin::Scintillation;
  }

  const_cast<G4Track*>(track)->SetUserInformation(new PhotonTrackInfo(origin, creatorName));
  if (fEventAction) fEventAction->CountProducedPhoton(origin);
}
