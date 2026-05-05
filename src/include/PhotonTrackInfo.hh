#ifndef PHOTON_TRACK_INFO_HH
#define PHOTON_TRACK_INFO_HH

#include "G4VUserTrackInformation.hh"
#include "globals.hh"

#include <utility>

/// Persistent label attached when an optical photon starts tracking.
/// PMT hit code uses this tag, not just the creator process pointer at detection.
class PhotonTrackInfo : public G4VUserTrackInformation {
public:
  enum class Origin { Unknown = 0, Scintillation = 1, Cherenkov = 2 };

  PhotonTrackInfo(Origin origin, G4String creator)
      : fOrigin(origin), fCreator(std::move(creator)) {}

  Origin GetOrigin() const { return fOrigin; }
  const G4String& GetCreatorName() const { return fCreator; }

private:
  Origin fOrigin;
  G4String fCreator;
};

#endif
