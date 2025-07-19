import os
import json
import shutil
from typing import List, Dict

from ParticuleCraft.core.installer_base import Installer
from .Compiler.MSVC.vcvars import find_vcvars, VcVarsEntry
from ParticuleCraft.system.log import logger

LOCAL_PATH = os.path.dirname(os.path.realpath(__file__))


class Installer(Installer):
    def __init__(self) -> None:
        self.vcvars_list: List[VcVarsEntry] = find_vcvars()

    def install(self) -> None:
        logger.info("Installing Windows distribution...")

        if not self.vcvars_list:
            raise RuntimeError(
                "No Visual Studio installation found. Please install Visual Studio (recommended 2022)."
            )

        if self.is_installed():
            raise RuntimeError("This distribution is already installed.")

        zip_path = os.path.join(LOCAL_PATH, "Libs", "SDL2_Project.zip")
        if not os.path.isfile(zip_path):
            raise FileNotFoundError(f"Archive not found: {zip_path}")

        shutil.unpack_archive(
            zip_path, os.path.join(LOCAL_PATH, "Libs", "SDL2_Project"), "zip"
        )

        self.configure()
        logger.info("[OK] Installation completed successfully.")

    def is_installed(self) -> bool:
        if not self.vcvars_list:
            return False
        return (
            os.path.isdir(os.path.join(LOCAL_PATH, "Libs", "SDL2_Project"))
            and os.path.isfile(os.path.join(LOCAL_PATH, "Compiler", "MSVC", "config.json"))
        )

    def configure(self) -> None:
        if not self.vcvars_list:
            raise RuntimeError("No Visual Studio installations found to configure.")

        def choose_version(target_arch: str) -> VcVarsEntry:
            for idx, entry in enumerate(self.vcvars_list, start=1):
                print(f"[{idx}]\t: [{entry.arch}] VS {entry.version} {entry.edition} → {entry.path}")
            while True:
                try:
                    choice = int(input(f"Select Visual Studio version for {target_arch} (1-{len(self.vcvars_list)}): "))
                    if 1 <= choice <= len(self.vcvars_list):
                        return self.vcvars_list[choice - 1]
                except ValueError:
                    pass
                print("Invalid input. Please choose a valid number.")

        print("Configuring Visual Studio paths:")
        path_x64 = choose_version("64-bit")
        path_x86 = choose_version("32-bit")

        config_path = os.path.join(LOCAL_PATH, "Compiler", "MSVC", "config.json")
        with open(config_path, "w", encoding="utf-8") as f:
            json.dump({"x64": path_x64.path, "x86": path_x86.path}, f, indent=4)

        logger.info(f"[OK] Config saved: {config_path}")

    def update(self) -> None:
        logger.info("No updates available for the Windows distribution.")

    def uninstall(self) -> None:
        raise NotImplementedError("Uninstall method not implemented for Windows.")
