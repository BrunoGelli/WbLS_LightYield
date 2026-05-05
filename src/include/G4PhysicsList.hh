#ifndef G4PhysicsList_h
#define G4PhysicsList_h 1

#include "DetectorConfig.hh"
#include "FTFP_BERT.hh"

/// Small modular physics list: FTFP_BERT + EM option4 + optical physics.
class G4PhysicsList : public FTFP_BERT {
public:
  explicit G4PhysicsList(const DetectorConfig& config);
  ~G4PhysicsList() override = default;
};

#endif
