#ifndef G4EventAction_h
#define G4EventAction_h 1

#include "PhotonTrackInfo.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class G4EventAction : public G4UserEventAction {
public:
  G4EventAction() = default;
  ~G4EventAction() override = default;

  void BeginOfEventAction(const G4Event* event) override;
  void EndOfEventAction(const G4Event* event) override;

  void SetPrimary(const G4String& sourceType, G4double energy);
  void AddWbLSEdep(G4double edep) { fEdepWbLS += edep; }
  void CountProducedPhoton(PhotonTrackInfo::Origin origin);
  void CountDetectedPhoton(G4int pmtId, PhotonTrackInfo::Origin origin);
  void FillDetectedPhoton(G4int pmtId, G4double wavelength, G4double time,
                          PhotonTrackInfo::Origin origin, const G4String& creator,
                          const G4ThreeVector& position);

private:
  G4int fEventID = -1;
  G4String fSourceType = "Cs137";
  G4double fPrimaryEnergy = 0.0;
  G4double fEdepWbLS = 0.0;
  G4int fProducedOptical = 0;
  G4int fProducedCherenkov = 0;
  G4int fProducedScintillation = 0;
  G4int fDetectedLarge = 0;
  G4int fDetectedSmall = 0;
  G4int fDetectedLargeCherenkov = 0;
  G4int fDetectedLargeScintillation = 0;
  G4int fDetectedSmallCherenkov = 0;
  G4int fDetectedSmallScintillation = 0;
};

#endif
