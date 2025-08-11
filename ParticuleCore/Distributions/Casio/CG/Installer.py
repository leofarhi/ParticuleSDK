import os
from typing import List

from ParticuleCraft.core.installer_base import Installer
from ParticuleCraft.utils.multi_platform import *
from ParticuleCraft.system.log import logger

LOCAL_PATH = os.path.dirname(os.path.realpath(__file__))

PACKAGES = {
    "apt": [
        "curl", "git", "python3", "gcc", "g++", "build-essential",
        "cmake", "pkg-config"
    ],
    "pacman": [
        "curl", "git", "python3", "gcc", "g++", "make", "cmake", "pkgconf"
    ]
}

GITEAPC_PACKAGES = [
    "Lephenixnoir/fxsdk",
    "Lephenixnoir/sh-elf-binutils",
    "Lephenixnoir/sh-elf-gcc",
    "Lephenixnoir/sh-elf-gdb",
    "Lephenixnoir/OpenLibm",
    "Vhex-Kernel-Core/fxlibc",
    "Lephenixnoir/gint",
    "Lephenixnoir/libprof",
]



class Installer(Installer):
    def __init__(self) -> None:
        super().__init__()
        detect_wsl()
        if not ("wsl" in platform_available or "linux" in platform_available):
            raise RuntimeError("Must run under WSL or Linux.")
        logger.info("Casio distribution installer initialized.")
    def install(self) -> None:
        logger.info("Installing Casio distribution...")
        if self.is_installed():
            raise RuntimeError("This distribution is already installed.")

        manager = DetectPackageManager()
        if manager == "apt":
            ProcessLinuxValue(f"sudo -v && apt-get update && apt-get upgrade -y && apt-get install -y {' '.join(PACKAGES['apt'])}")
        elif manager == "pacman":
            ProcessLinuxValue(f"sudo -v && pacman -Sy --noconfirm && pacman -S --noconfirm {' '.join(PACKAGES['pacman'])}")

        if ProcessLinux("giteapc --version").wait() != 0:
            ProcessLinuxValue(
                'curl "https://git.planet-casio.com/Lephenixnoir/GiteaPC/raw/branch/master/install.sh" -o /tmp/giteapc-install.sh'
            )
            ProcessLinuxValue("bash /tmp/giteapc-install.sh")

        if ProcessLinux("giteapc --version").wait() != 0:
            raise RuntimeError("Giteapc installation failed.")

        for package in GITEAPC_PACKAGES:
            ProcessLinuxValue(f"giteapc install {package} -y")

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

        for cmd in ["giteapc", "sh-elf-gcc", "sh-elf-gdb"]:
            if ProcessLinux(f"{cmd} --version").wait() != 0:
                missing.append(cmd)

        installed_packages = ProcessLinuxStdout("giteapc list")
        for package in GITEAPC_PACKAGES:
            if package not in installed_packages:
                missing.append(f"giteapc:{package}")

        if missing:
            logger.warning("Missing packages/components:")
            for m in missing:
                logger.warning(f"  - {m}")
            return False

        return True

    def configure(self) -> None:
        logger.info("No additional configuration needed for Casio.")

    def update(self) -> None:
        if not self.is_installed():
            raise RuntimeError("Distribution not installed. Cannot update.")

        logger.info("[INFO] Updating Giteapc packages...")
        ret = ProcessLinuxValue("giteapc install -u -y")
        if ret != 0:
            raise RuntimeError("Update failed.")
        logger.info("[OK] Update completed.")

    def uninstall(self) -> None:
        raise NotImplementedError("Uninstall method not implemented for Casio.")
