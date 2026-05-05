#include "G4DetectorConstruction.hh"
#include "PMTSensitiveDetector.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4Element.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalConstants.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4Sphere.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

#include <vector>

namespace {
std::vector<G4double> EnergiesFromWavelengths(const DetectorConfig& cfg) {
  std::vector<G4double> energies;
  energies.reserve(cfg.wavelengths.size());
  // Geant4 material property vectors must be ordered by increasing photon energy.
  for (auto it = cfg.wavelengths.rbegin(); it != cfg.wavelengths.rend(); ++it) {
    energies.push_back(h_Planck * c_light / (*it * nm));
  }
  return energies;
}

template <typename Array>
std::vector<G4double> ReverseValues(const Array& input) {
  return std::vector<G4double>(input.rbegin(), input.rend());
}

void AddProperty(G4MaterialPropertiesTable* mpt, const char* name,
                 const std::vector<G4double>& energies,
                 const std::vector<G4double>& values) {
  // Geant4 10.x declares AddProperty with non-const G4double* arguments even
  // though it copies the arrays into a G4MaterialPropertyVector. Keep this
  // helper const-correct for callers, then pass mutable local buffers to remain
  // compatible with both Geant4 10.x and 11.x.
  auto mutableEnergies = energies;
  auto mutableValues = values;
  mpt->AddProperty(name, mutableEnergies.data(), mutableValues.data(),
                   static_cast<G4int>(mutableEnergies.size()));
}
}

G4DetectorConstruction::G4DetectorConstruction(const DetectorConfig& config) : fConfig(config) {}

G4VPhysicalVolume* G4DetectorConstruction::Construct() {
  DefineMaterials();
  return DefineVolumes();
}

void G4DetectorConstruction::DefineMaterials() {
  auto* nist = G4NistManager::Instance();
  nist->FindOrBuildMaterial("G4_AIR");
  nist->FindOrBuildMaterial("G4_Galactic");
  nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");

  auto* water = nist->FindOrBuildMaterial("G4_WATER");
  auto* wbls = new G4Material("WbLS", fConfig.wblsDensity, 1);
  wbls->AddMaterial(water, 1.0);

  const auto energies = EnergiesFromWavelengths(fConfig);
  auto* wblsMPT = new G4MaterialPropertiesTable();
  AddProperty(wblsMPT, "RINDEX", energies, ReverseValues(fConfig.wblsRIndex));
  auto absM = ReverseValues(fConfig.wblsAbsLength);
  for (auto& value : absM) value *= m;
  AddProperty(wblsMPT, "ABSLENGTH", energies, absM);
  AddProperty(wblsMPT, "SCINTILLATIONCOMPONENT1", energies, ReverseValues(fConfig.emission));
  // Legacy aliases keep the toy usable with Geant4 10.x examples/configurations.
  AddProperty(wblsMPT, "FASTCOMPONENT", energies, ReverseValues(fConfig.emission));
  wblsMPT->AddConstProperty("SCINTILLATIONYIELD", fConfig.wblsScintYield);
  wblsMPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
  wblsMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", fConfig.wblsDecayTime);
  wblsMPT->AddConstProperty("SCINTILLATIONYIELD1", 1.0);
  wblsMPT->AddConstProperty("FASTTIMECONSTANT", fConfig.wblsDecayTime);
  wblsMPT->AddConstProperty("YIELDRATIO", 1.0);
  wbls->SetMaterialPropertiesTable(wblsMPT);
  wbls->GetIonisation()->SetBirksConstant(fConfig.birksConstant);

  auto* glass = G4Material::GetMaterial("G4_SILICON_DIOXIDE");
  auto* glassMPT = new G4MaterialPropertiesTable();
  AddProperty(glassMPT, "RINDEX", energies, ReverseValues(fConfig.glassRIndex));
  auto glassAbsM = ReverseValues(fConfig.glassAbsLength);
  for (auto& value : glassAbsM) value *= m;
  AddProperty(glassMPT, "ABSLENGTH", energies, glassAbsM);
  glass->SetMaterialPropertiesTable(glassMPT);

  auto* air = G4Material::GetMaterial("G4_AIR");
  auto* airMPT = new G4MaterialPropertiesTable();
  std::vector<G4double> airIndex(energies.size(), 1.00029);
  AddProperty(airMPT, "RINDEX", energies, airIndex);
  air->SetMaterialPropertiesTable(airMPT);

  auto* spectralon = new G4Material("Spectralon", 2.2 * g / cm3, 2);
  spectralon->AddElement(nist->FindOrBuildElement("C"), 2);
  spectralon->AddElement(nist->FindOrBuildElement("F"), 4);

  auto* photocathode = new G4Material("Photocathode", 1.0 * g / cm3, 1);
  photocathode->AddElement(nist->FindOrBuildElement("Si"), 1);
  auto* pcMPT = new G4MaterialPropertiesTable();
  std::vector<G4double> pcIndex(energies.size(), 1.5);
  AddProperty(pcMPT, "RINDEX", energies, pcIndex);
  photocathode->SetMaterialPropertiesTable(pcMPT);
}

G4VPhysicalVolume* G4DetectorConstruction::DefineVolumes() {
  auto* vacuum = G4Material::GetMaterial("G4_Galactic");
  auto* air = G4Material::GetMaterial("G4_AIR");
  auto* spectralon = G4Material::GetMaterial("Spectralon");
  auto* glass = G4Material::GetMaterial("G4_SILICON_DIOXIDE");
  auto* wbls = G4Material::GetMaterial("WbLS");

  const G4double worldHalf = fConfig.sphereRadius + 8.0 * cm;
  auto* worldS = new G4Box("World", worldHalf, worldHalf, worldHalf);
  auto* worldLV = new G4LogicalVolume(worldS, vacuum, "World");
  auto* worldPV = new G4PVPlacement(nullptr, {}, worldLV, "World", nullptr, false, 0, true);

  // Thin visual shell; optical reflection is assigned to the air/world boundary below.
  auto* shellS = new G4Sphere("SpectralonShell", fConfig.sphereRadius,
                              fConfig.sphereRadius + fConfig.sphereWallThickness,
                              0., twopi, 0., pi);
  auto* shellLV = new G4LogicalVolume(shellS, spectralon, "SpectralonShell");
  auto* shellPV = new G4PVPlacement(nullptr, {}, shellLV, "SpectralonShell", worldLV, false, 0, true);

  auto* sphereS = new G4Sphere("SphereInterior", 0., fConfig.sphereRadius, 0., twopi, 0., pi);
  auto* sphereLV = new G4LogicalVolume(sphereS, air, "SphereInterior");
  auto* spherePV = new G4PVPlacement(nullptr, {}, sphereLV, "SphereInterior", worldLV, false, 0, true);

  // Cuvette: a simple glass cylinder enclosing a WbLS cylinder. The top face is at the sample port.
  const G4double sampleZ = fConfig.sphereRadius - 0.5 * fConfig.cuvetteHeight;
  auto* cuvetteS = new G4Tubs("CuvetteGlass", 0., fConfig.cuvetteOuterRadius,
                              0.5 * fConfig.cuvetteHeight, 0., twopi);
  auto* cuvetteLV = new G4LogicalVolume(cuvetteS, glass, "CuvetteGlass");
  new G4PVPlacement(nullptr, {0., 0., sampleZ}, cuvetteLV, "CuvetteGlass", sphereLV, false, 0, true);

  auto* wblsS = new G4Tubs("WbLS", 0., fConfig.cuvetteOuterRadius - fConfig.cuvetteWallThickness,
                           0.5 * (fConfig.cuvetteHeight - 2.0 * fConfig.cuvetteWallThickness), 0., twopi);
  auto* wblsLV = new G4LogicalVolume(wblsS, wbls, "WbLS");
  new G4PVPlacement(nullptr, {}, wblsLV, "WbLS", cuvetteLV, false, 0, true);

  fLargePMTLogical = MakePhotocathode("largePMT", 0.5 * fConfig.largePMTPortDiameter, 0);
  fSmallPMTLogical = MakePhotocathode("smallPMT", 0.5 * fConfig.smallPMTPortDiameter, 1);
  auto* yRot = new G4RotationMatrix();
  yRot->rotateY(90.0 * deg);
  new G4PVPlacement(yRot, {fConfig.sphereRadius - 0.01 * mm, 0., 0.}, fLargePMTLogical,
                    "largePMT", sphereLV, false, 0, true);
  auto* yRotOpposite = new G4RotationMatrix();
  yRotOpposite->rotateY(90.0 * deg);
  new G4PVPlacement(yRotOpposite, {-fConfig.sphereRadius + 0.01 * mm, 0., 0.}, fSmallPMTLogical,
                    "smallPMT", sphereLV, false, 1, true);

  const auto energies = EnergiesFromWavelengths(fConfig);
  auto* reflectorSurface = new G4OpticalSurface("SpectralonLambertian");
  reflectorSurface->SetType(dielectric_dielectric);
  reflectorSurface->SetModel(unified);
  reflectorSurface->SetFinish(groundfrontpainted);
  auto* reflectMPT = new G4MaterialPropertiesTable();
  std::vector<G4double> reflectivity(energies.size(), fConfig.spectralonReflectivity);
  std::vector<G4double> efficiency(energies.size(), 0.0);
  AddProperty(reflectMPT, "REFLECTIVITY", energies, reflectivity);
  AddProperty(reflectMPT, "EFFICIENCY", energies, efficiency);
  reflectorSurface->SetMaterialPropertiesTable(reflectMPT);
  new G4LogicalBorderSurface("SphereDiffuseReflector", spherePV, shellPV, reflectorSurface);

  auto* shellVis = new G4VisAttributes(G4Colour(0.95, 0.95, 0.90, 0.25));
  shellVis->SetForceWireframe(true);
  shellLV->SetVisAttributes(shellVis);
  wblsLV->SetVisAttributes(new G4VisAttributes(G4Colour(0.1, 0.4, 1.0, 0.6)));
  cuvetteLV->SetVisAttributes(new G4VisAttributes(G4Colour(0.8, 0.9, 1.0, 0.25)));
  fLargePMTLogical->SetVisAttributes(new G4VisAttributes(G4Colour(0.2, 0.8, 0.2)));
  fSmallPMTLogical->SetVisAttributes(new G4VisAttributes(G4Colour(0.2, 0.8, 0.2)));

  return worldPV;
}

G4LogicalVolume* G4DetectorConstruction::MakePhotocathode(const G4String& name, G4double radius, G4int) {
  auto* disk = new G4Tubs(name + "Disk", 0., radius, 0.05 * mm, 0., twopi);
  return new G4LogicalVolume(disk, G4Material::GetMaterial("Photocathode"), name);
}

void G4DetectorConstruction::ConstructSDandField() {
  auto* sdManager = G4SDManager::GetSDMpointer();
  auto* largeSD = new PMTSensitiveDetector("/det/largePMT", fConfig, 0);
  auto* smallSD = new PMTSensitiveDetector("/det/smallPMT", fConfig, 1);
  sdManager->AddNewDetector(largeSD);
  sdManager->AddNewDetector(smallSD);
  if (fLargePMTLogical) fLargePMTLogical->SetSensitiveDetector(largeSD);
  if (fSmallPMTLogical) fSmallPMTLogical->SetSensitiveDetector(smallSD);
}