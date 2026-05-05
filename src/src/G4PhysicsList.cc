#include "G4PhysicsList.hh"

#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"

G4PhysicsList::G4PhysicsList(const DetectorConfig& config) : FTFP_BERT() {
  ReplacePhysics(new G4EmStandardPhysics_option4());

  if (config.enableOptical) {
    RegisterPhysics(new G4OpticalPhysics());
  }
}
