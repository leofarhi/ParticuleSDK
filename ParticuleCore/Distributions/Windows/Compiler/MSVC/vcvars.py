import os
from pathlib import Path
from typing import List, Dict

from dataclasses import dataclass

@dataclass
class VcVarsEntry:
    path: str
    version: str
    edition: str
    arch: str


def find_vcvars() -> List[VcVarsEntry]:
    """
    Search for Visual Studio vcvars*.bat files.
    Returns a list of VcVarsEntry with path, version, edition, and arch.
    """
    results: List[VcVarsEntry] = []

    base_dirs = [
        os.environ.get("ProgramFiles", r"C:\Program Files"),
        os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"),
    ]

    for base_dir in base_dirs:
        vs_root = Path(base_dir) / "Microsoft Visual Studio"
        if not vs_root.is_dir():
            continue

        for version_dir in vs_root.iterdir():
            if not version_dir.is_dir():
                continue

            for edition_dir in version_dir.iterdir():
                if not edition_dir.is_dir():
                    continue

                vc_path = edition_dir / "VC" / "Auxiliary" / "Build"
                if not vc_path.is_dir():
                    continue

                for vcvars_bat in vc_path.glob("vcvars*.bat"):
                    results.append(VcVarsEntry(
                        path=str(vcvars_bat.resolve()),
                        version=version_dir.name,
                        edition=edition_dir.name,
                        arch="x86" if "Program Files (x86)" in base_dir else "x64"
                    ))

    return results


if __name__ == "__main__":
    vcvars_list = find_vcvars()

    if not vcvars_list:
        print("[INFO] No vcvars*.bat found.")
    else:
        for entry in vcvars_list:
            print(f"[{entry.arch}] VS {entry.version} {entry.edition} → {entry.path}")
