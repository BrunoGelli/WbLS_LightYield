#ifndef PMT_SENSITIVE_DETECTOR_HH
#define PMT_SENSITIVE_DETECTOR_HH

#include "DetectorConfig.hh"
#include "G4VSensitiveDetector.hh"

class PMTSensitiveDetector : public G4VSensitiveDetector {
public:
  PMTSensitiveDetector(const G4String& name, const DetectorConfig& config, G4int pmtId);
  ~PMTSensitiveDetector() override = default;

  G4bool ProcessHits(G4Step* step, G4TouchableHistory*) override;

private:
  G4double QuantumEfficiency(G4double wavelengthNm) const;

  const DetectorConfig& fConfig;
  G4int fPMTId; // 0 = large (+x), 1 = small (-x)
};

#endif
