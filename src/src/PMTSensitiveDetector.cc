#include "PMTSensitiveDetector.hh"

#include "G4AnalysisManager.hh"
#include "G4EventManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4PhysicalConstants.hh"
#include "G4RandomTools.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"

#include "G4EventAction.hh"
#include "PhotonTrackInfo.hh"

PMTSensitiveDetector::PMTSensitiveDetector(const G4String& name, const DetectorConfig& config, G4int pmtId)
    : G4VSensitiveDetector(name), fConfig(config), fPMTId(pmtId) {}

G4double PMTSensitiveDetector::QuantumEfficiency(G4double wavelengthNm) const {
  const auto& qe = (fPMTId == 0) ? fConfig.largePMTQE : fConfig.smallPMTQE;
  const auto& wl = fConfig.wavelengths;
  if (wavelengthNm <= wl.front()) return qe.front();
  if (wavelengthNm >= wl.back()) return qe.back();

  for (std::size_t i = 1; i < wl.size(); ++i) {
    if (wavelengthNm < wl[i]) {
      const G4double t = (wavelengthNm - wl[i - 1]) / (wl[i] - wl[i - 1]);
      return qe[i - 1] + t * (qe[i] - qe[i - 1]);
    }
  }
  return 0.0;
}

G4bool PMTSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
  auto* track = step->GetTrack();
  if (track->GetDefinition() != G4OpticalPhoton::Definition()) return false;

  const G4double wavelength = (h_Planck * c_light / track->GetKineticEnergy()) / nm;
  const bool detected = G4UniformRand() < QuantumEfficiency(wavelength);

  auto origin = PhotonTrackInfo::Origin::Unknown;
  G4String creator = "unknown";
  if (auto* info = dynamic_cast<PhotonTrackInfo*>(track->GetUserInformation())) {
    origin = info->GetOrigin();
    creator = info->GetCreatorName();
  }

  if (detected) {
    auto* eventAction = dynamic_cast<G4EventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());
    if (eventAction) {
      eventAction->CountDetectedPhoton(fPMTId, origin);
      eventAction->FillDetectedPhoton(fPMTId, wavelength, track->GetGlobalTime(), origin, creator,
                                      step->GetPostStepPoint()->GetPosition());
    }
  }

  // Photocathode disks absorb photons whether the photoelectron was counted or not.
  track->SetTrackStatus(fStopAndKill);
  return detected;
}
