#ifndef DETECTOR_CONFIG_HH
#define DETECTOR_CONFIG_HH

#include "G4SystemOfUnits.hh"
#include "globals.hh"

#include <array>
#include <cstddef>

/// All geometry/material knobs for the compact WbLS light-yield toy.
/// Values are intentionally centralized here so a new user can tune the setup
/// without hunting through the geometry code. Runtime source settings are in
/// G4PrimaryGeneratorAction and can be changed from macros.
struct DetectorConfig {
  // Integrating sphere and ports.
  G4double sphereRadius = 10.0 * cm;
  G4double sphereWallThickness = 1.0 * cm; // only visual; the optical surface is the inner boundary
  G4double samplePortDiameter = 3.0 * cm;
  G4double largePMTPortDiameter = 5.0 * cm;
  G4double smallPMTPortDiameter = 2.5 * cm;

  // Cuvette/sample. The WbLS cylinder hangs down from the top port into the sphere.
  G4double cuvetteOuterRadius = 1.20 * cm;
  G4double cuvetteHeight = 4.0 * cm;
  G4double cuvetteWallThickness = 1.0 * mm;

  // Optical material controls.
  G4double wblsDensity = 1.0 * g / cm3;
  G4double wblsScintYield = 250.0 / MeV; // deliberately modest for fast toy runs
  G4double wblsDecayTime = 5.0 * ns;
  G4double birksConstant = 0.126 * mm / MeV;
  G4double spectralonReflectivity = 0.98;

  // Optional guard against photons that get trapped in many reflections.
  G4int maxOpticalReflections = 200;
  G4bool enableOptical = true;

  // Shared wavelength support points for toy optical properties.
  static constexpr std::size_t nOpticalPoints = 8;
  std::array<G4double, nOpticalPoints> wavelengths = {270., 300., 340., 380., 420., 460., 500., 600.}; // nm
  std::array<G4double, nOpticalPoints> wblsRIndex = {1.36, 1.35, 1.345, 1.340, 1.337, 1.335, 1.334, 1.333};
  std::array<G4double, nOpticalPoints> wblsAbsLength = {0.20, 0.50, 1.0, 3.0, 8.0, 10.0, 12.0, 15.0}; // m
  std::array<G4double, nOpticalPoints> emission = {0.00, 0.05, 0.25, 0.80, 1.00, 0.55, 0.15, 0.00};
  std::array<G4double, nOpticalPoints> glassRIndex = {1.55, 1.52, 1.49, 1.475, 1.470, 1.468, 1.466, 1.463};
  std::array<G4double, nOpticalPoints> glassAbsLength = {0.01, 0.10, 2.0, 5.0, 8.0, 10.0, 10.0, 10.0}; // m; UV cutoff near 270 nm
  std::array<G4double, nOpticalPoints> largePMTQE = {0.00, 0.03, 0.15, 0.25, 0.30, 0.24, 0.12, 0.01};
  std::array<G4double, nOpticalPoints> smallPMTQE = {0.00, 0.02, 0.12, 0.22, 0.27, 0.20, 0.10, 0.01};
};

#endif
