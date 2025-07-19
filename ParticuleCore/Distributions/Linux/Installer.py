import os
from typing import List

from ParticuleCraft.core.installer_base import Installer
from ParticuleCraft.utils.multi_platform import *
from ParticuleCraft.system.log import logger

LOCAL_PATH = os.path.dirname(os.path.realpath(__file__))

PACKAGES = {
    "apt": [
        "curl", "python3", "gcc", "g++", "build-essential",
        "make", "pkg-config", "libsdl2-dev", "libsdl2-image-dev",
        "libsdl2-ttf-dev", "libsdl2-mixer-dev"
    ],
    "pacman": [
        "curl", "python3", "gcc", "g++", "make", "pkgconf",
        "sdl2", "sdl2_image", "sdl2_ttf", "sdl2_mixer"
    ]
}

class Installer(Installer):
    def __init__(self) -> None:
        super().__init__()
        detect_wsl()
        if not ("wsl" in platform_available or "linux" in platform_available):
            raise RuntimeError("Must run under WSL or Linux.")
        logger.info("Linux distribution installer initialized.")
    def install(self) -> None:
        logger.info("Installing Linux distribution...")
        if self.is_installed():
            raise RuntimeError("This distribution is already installed.")

        manager = DetectPackageManager()
        if manager == "apt":
            ProcessLinuxValue(f"sudo -v && sudo apt-get update && sudo apt-get upgrade -y && sudo apt-get install -y {' '.join(PACKAGES['apt'])}")
        elif manager == "pacman":
            ProcessLinuxValue(f"sudo -v && sudo pacman -Sy --noconfirm && sudo pacman -S --noconfirm {' '.join(PACKAGES['pacman'])}")

        if not self.is_installed():
            raise RuntimeError("Installation failed.")
        logger.info("[OK] Installation completed successfully.")

    def is_installed(self) -> bool:
        if not ("wsl" in platform_available or "linux" in platform_available):
            raise RuntimeError("Must run under WSL or Linux.")

        manager = DetectPackageManager()
        missing: List[str] = []

        if manager == "apt":
            for pkg in PACKAGES["apt"]:
                if ProcessLinux(f"dpkg -s {pkg}").wait() != 0:
                    missing.append(pkg)
        elif manager == "pacman":
            for pkg in PACKAGES["pacman"]:
                if ProcessLinux(f"pacman -Qi {pkg}").wait() != 0:
                    missing.append(pkg)

        if missing:
            logger.warning("Missing packages:")
            for pkg in missing:
                logger.warning(f"  - {pkg}")
            return False

        return True

    def configure(self) -> None:
        logger.info("No additional configuration needed for Linux.")

    def update(self) -> None:
        logger.info("No updates available for this distribution.")

    def uninstall(self) -> None:
        raise NotImplementedError("Uninstall method not implemented for Linux.")
