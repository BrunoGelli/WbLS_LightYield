#ifndef G4DetectorConstruction_h
#define G4DetectorConstruction_h 1

#include "DetectorConfig.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

class G4DetectorConstruction : public G4VUserDetectorConstruction {
public:
  explicit G4DetectorConstruction(const DetectorConfig& config = DetectorConfig{});
  ~G4DetectorConstruction() override = default;

  G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;

  const DetectorConfig& GetConfig() const { return fConfig; }

private:
  void DefineMaterials();
  G4VPhysicalVolume* DefineVolumes();
  G4LogicalVolume* MakePhotocathode(const G4String& name, G4double radius, G4int pmtId);

  DetectorConfig fConfig;
  G4LogicalVolume* fLargePMTLogical = nullptr;
  G4LogicalVolume* fSmallPMTLogical = nullptr;
};

#endif
