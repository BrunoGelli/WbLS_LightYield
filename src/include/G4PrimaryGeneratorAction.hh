#ifndef G4PrimaryGeneratorAction_h
#define G4PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4Event;
class G4GenericMessenger;
class G4ParticleGun;

class G4PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  G4PrimaryGeneratorAction();
  ~G4PrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event*) override;

private:
  G4double SampleGammaEnergy() const;
  void DefineCommands();

  G4ParticleGun* fGun = nullptr;
  G4GenericMessenger* fMessenger = nullptr;
  G4String fIsotope = "Cs137";
  G4double fSourceRadius = 0.5 * cm;
  G4double fSourceZ = 14.0 * cm;
};

#endif
