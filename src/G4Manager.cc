#include "DetectorConfig.hh"
#include "G4ActionInitialization.hh"
#include "G4DetectorConstruction.hh"
#include "G4PhysicsList.hh"

#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

int main(int argc, char** argv) {
  DetectorConfig config;

  auto* runManager = new G4RunManager();
  runManager->SetUserInitialization(new G4DetectorConstruction(config));
  runManager->SetUserInitialization(new G4PhysicsList(config));
  runManager->SetUserInitialization(new G4ActionInitialization(config));
  runManager->Initialize();

  auto* visManager = new G4VisExecutive();
  visManager->Initialize();

  auto* uiManager = G4UImanager::GetUIpointer();
  if (argc > 1) {
    const G4String command = "/control/execute ";
    uiManager->ApplyCommand(command + argv[1]);
  } else {
    auto* ui = new G4UIExecutive(argc, argv);
    uiManager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;
  return 0;
}
