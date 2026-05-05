#ifndef G4ActionInitialization_h
#define G4ActionInitialization_h 1

#include "DetectorConfig.hh"
#include "G4VUserActionInitialization.hh"

class G4ActionInitialization : public G4VUserActionInitialization {
public:
  explicit G4ActionInitialization(const DetectorConfig& config);
  ~G4ActionInitialization() override = default;

  void BuildForMaster() const override;
  void Build() const override;

private:
  DetectorConfig fConfig;
};

#endif
