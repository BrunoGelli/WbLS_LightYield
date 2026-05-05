# WbLS Light-Yield Toy Geant4 Simulation

A compact, commented Geant4 optical simulation for a WbLS sample viewed inside a
Spectralon-like integrating sphere by two PMT photocathode disks. The app is kept
small on purpose: geometry, materials, physics, primary generation, photon
tracking, PMT detection, and ROOT output live in separate classes, but each class
only implements the pieces needed for light-yield studies.

## What is simulated

- Gamma interactions from a disk source above the sample port.
- WbLS scintillation and Cherenkov production.
- Optical transport through WbLS, quartz/glass cuvette, air, and a diffuse
  Spectralon-like spherical reflector.
- Probabilistic PMT detection using configurable wavelength-dependent QE curves.
- Per-event and per-detected-photon ROOT TTrees.

## Geometry summary

| Component | Toy implementation |
| --- | --- |
| Integrating sphere | Air-filled sphere with radius `DetectorConfig::sphereRadius`; a thin Spectralon shell is drawn outside it. The inner sphere/shell boundary has a Lambertian diffuse optical surface. |
| Ports | The top sample port and side PMT ports are represented by the source/cuvette and flush PMT disks. The sphere shell is not boolean-cut, which keeps the geometry robust and compact. |
| Cuvette | Glass/quartz-like cylinder inserted from the top; the WbLS cylinder is its daughter volume. |
| PMTs | Two ideal photocathode disks flush with the inner sphere at `+x` (large PMT, ID 0) and `-x` (small PMT, ID 1). |
| Source | Isotropic gamma disk source above the sample port. Cs-137, Co-60, and Na-22 are selected from macros. |

Most dimensions and optical properties are centralized in `src/include/DetectorConfig.hh`.

## Build

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j
```

The executable is `wblsSim`.

## Run examples

From the build directory:

```bash
./wblsSim run_Cs137.mac
./wblsSim run_Co60.mac
./wblsSim run_Na22.mac
```

The macros set the isotope, source radius/position, output filename, and number
of events. Interactive visualization starts when no macro is supplied:

```bash
./wblsSim
```

## Source macro commands

Runtime gamma-source settings are exposed under `/wbls/source/`:

```text
/wbls/source/isotope Cs137   # Cs137, Co60, or Na22
/wbls/source/radius 0.5 cm   # disk radius
/wbls/source/z 14 cm         # source plane above the sphere
```

For simplicity, Co-60 and Na-22 runs sample one gamma line per event rather than
building full decay cascades. The sampled gamma energy is recorded in the event
TTree.

## ROOT output

The Geant4 analysis manager writes ROOT output with two TTrees.

### `events` tree

- `eventID`
- `sourceType`
- `primaryGammaEnergy_keV`
- `edepWbLS_MeV`
- `nOpticalProduced`
- `nCherenkovProduced`
- `nScintProduced`
- `nDetectedLargePMT`
- `nDetectedSmallPMT`
- `nDetectedLargeCherenkov`
- `nDetectedLargeScintillation`
- `nDetectedSmallCherenkov`
- `nDetectedSmallScintillation`

### `photons` tree

- `eventID`
- `pmtID` (`0 = large`, `1 = small`)
- `wavelength_nm`
- `time_ns`
- `creationProcessCode` (`0 = unknown`, `1 = scintillation`, `2 = Cherenkov`)
- `creatorProcessName`
- `hitX_mm`, `hitY_mm`, `hitZ_mm`

Optical photons are tagged in `TrackingAction` as soon as they start tracking.
PMT hits read the persistent `PhotonTrackInfo` tag instead of relying only on the
creator process pointer at detection time.

## Quick validation plots

The plotting helper uses `uproot`, `numpy`, and `matplotlib`:

```bash
python3 scripts/plot_lightyield.py build/wbls_Cs137.root --outdir plots_Cs137
```

It produces quick-look histograms for WbLS energy deposition, detected photons in
each PMT, Cherenkov vs scintillation counts, wavelength spectrum, and timing.

## File map

| File | Purpose |
| --- | --- |
| `src/include/DetectorConfig.hh` | Central constants for dimensions, optical properties, QE curves, and optional photon controls. |
| `src/src/G4DetectorConstruction.cc` | Materials, sphere/cuvette/PMT geometry, optical reflector surface, and PMT sensitive-detector registration. |
| `src/src/G4PhysicsList.cc` | FTFP_BERT with EM option4 and optical physics. |
| `src/src/G4PrimaryGeneratorAction.cc` | Isotropic disk gamma source and source macro commands. |
| `src/src/TrackingAction.cc` | Persistent optical-photon origin tags and production counters. |
| `src/src/G4SteppingAction.cc` | WbLS energy-deposition accumulation and optional photon step-limit guard. |
| `src/src/PMTSensitiveDetector.cc` | PMT QE interpolation, probabilistic detection, photon-row output, and photon absorption. |
| `src/src/G4RunAction.cc` | ROOT TTree definitions and file lifecycle. |
| `scripts/plot_lightyield.py` | Lightweight validation plots. |
