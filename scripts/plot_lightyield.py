#!/usr/bin/env python3
"""Quick-look plots for the WbLS light-yield ROOT output.

Usage:
  python3 scripts/plot_lightyield.py build/wbls_Cs137.root

Requires uproot, numpy, and matplotlib. The script intentionally avoids ROOT's
Python bindings so it is easy to run in lightweight analysis environments.
"""

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import uproot


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("root_file", help="Geant4 analysis ROOT file")
    parser.add_argument("--outdir", default="plots", help="Directory for PNG plots")
    args = parser.parse_args()

    outdir = Path(args.outdir)
    outdir.mkdir(parents=True, exist_ok=True)

    with uproot.open(args.root_file) as root_file:
        events = root_file["events"].arrays(library="np")
        photons = root_file["photons"].arrays(library="np") if "photons" in root_file else {}

    def save_hist(data, name, xlabel, bins=60):
        plt.figure(figsize=(7, 5))
        plt.hist(data, bins=bins, histtype="stepfilled", alpha=0.75)
        plt.xlabel(xlabel)
        plt.ylabel("Events" if len(data) == len(events["eventID"]) else "Photons")
        plt.tight_layout()
        plt.savefig(outdir / f"{name}.png", dpi=160)
        plt.close()

    save_hist(events["edepWbLS_MeV"], "edep_wbls", "Energy deposited in WbLS [MeV]")
    save_hist(events["nDetectedLargePMT"], "detected_large_pmt", "Detected photons: large PMT")
    save_hist(events["nDetectedSmallPMT"], "detected_small_pmt", "Detected photons: small PMT")

    plt.figure(figsize=(7, 5))
    labels = ["large Cher", "large Scint", "small Cher", "small Scint"]
    values = [
        np.sum(events["nDetectedLargeCherenkov"]),
        np.sum(events["nDetectedLargeScintillation"]),
        np.sum(events["nDetectedSmallCherenkov"]),
        np.sum(events["nDetectedSmallScintillation"]),
    ]
    plt.bar(labels, values)
    plt.ylabel("Detected photons")
    plt.xticks(rotation=20, ha="right")
    plt.tight_layout()
    plt.savefig(outdir / "cherenkov_vs_scintillation.png", dpi=160)
    plt.close()

    if photons and len(photons["eventID"]):
        save_hist(photons["wavelength_nm"], "detected_wavelength", "Detected wavelength [nm]")
        save_hist(photons["time_ns"], "detected_time", "Detection time [ns]")

    print(f"Wrote plots to {outdir}")


if __name__ == "__main__":
    main()
