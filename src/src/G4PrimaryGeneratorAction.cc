#include "G4PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4Gamma.hh"
#include "G4GenericMessenger.hh"
#include "G4ParticleGun.hh"
#include "G4RandomDirection.hh"
#include "G4RandomTools.hh"
#include "G4SystemOfUnits.hh"

#include "G4EventAction.hh"

#include <cmath>

G4PrimaryGeneratorAction::G4PrimaryGeneratorAction() : fGun(new G4ParticleGun(1)) {
  fGun->SetParticleDefinition(G4Gamma::Definition());
  DefineCommands();
}

G4PrimaryGeneratorAction::~G4PrimaryGeneratorAction() {
  delete fMessenger;
  delete fGun;
}

void G4PrimaryGeneratorAction::DefineCommands() {
  fMessenger = new G4GenericMessenger(this, "/wbls/source/", "WbLS gamma source controls");
  fMessenger->DeclareProperty("isotope", fIsotope, "Cs137, Co60, or Na22");
  fMessenger->DeclarePropertyWithUnit("radius", "cm", fSourceRadius, "Disk source radius");
  fMessenger->DeclarePropertyWithUnit("z", "cm", fSourceZ, "Source z position above top port");
}

G4double G4PrimaryGeneratorAction::SampleGammaEnergy() const {
  if (fIsotope == "Co60" || fIsotope == "Co-60") {
    return (G4UniformRand() < 0.5) ? 1173.2 * keV : 1332.5 * keV;
  }
  if (fIsotope == "Na22" || fIsotope == "Na-22") {
    return (G4UniformRand() < 2.0 / 3.0) ? 511.0 * keV : 1274.5 * keV;
  }
  return 661.7 * keV;
}

void G4PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  const G4double r = fSourceRadius * std::sqrt(G4UniformRand());
  const G4double phi = 2*3.1415 * G4UniformRand();
  const G4double energy = SampleGammaEnergy();

  fGun->SetParticleEnergy(energy);
  fGun->SetParticlePosition({r * std::cos(phi), r * std::sin(phi), fSourceZ});
  fGun->SetParticleMomentumDirection(G4RandomDirection());
  fGun->GeneratePrimaryVertex(event);

  auto* eventAction = dynamic_cast<G4EventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());
  if (eventAction) eventAction->SetPrimary(fIsotope, energy);
}
