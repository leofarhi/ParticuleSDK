import os
import sys
import subprocess
from pathlib import Path
from typing import Optional, List
from ..system.log import logger


# ------------------------------------------------------------
# Plateformes détectées : ex. ["windows", "wsl"], ["linux"], etc.
# ------------------------------------------------------------

platform: str = sys.platform.lower()
platform_available: List[str] = []

def detect_wsl() -> bool:
    """
    Vérifie si WSL est installé.
    """
    if platform == "win32":
        if "wsl" not in platform_available:
            try:
                completed = subprocess.run(
                    ["wsl", "-e", "echo", "wsl"],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.DEVNULL,
                    check=True
                )
                if completed.returncode == 0:
                    logger.info("\033[92mWSL detected.\033[0m")
                    platform_available.append("wsl")
                    return True
            except Exception:
                pass

            logger.info("\033[91mWSL not detected.\033[0m")
            return False
        else:
            return True
    else:
        return False


if platform == "win32":
    platform_available.append("windows")

elif platform == "linux":
    platform_available.append("linux")
else:
    raise ValueError(f"Unsupported platform: {platform}")


# ------------------------------------------------------------
# Fonctions génériques pour lancer des commandes Linux
# ------------------------------------------------------------

def _wrap_wsl_command(cmd: str) -> str:
    """Prépare une commande bash WSL pour Windows."""
    return f'wsl -e bash -lic "{cmd}"'


def ProcessLinuxStdout(cmd: str, cwd: Optional[str] = None) -> str:
    """
    Exécute une commande Linux et renvoie stdout.
    """
    if "windows" in platform_available and "wsl" in platform_available:
        cmd = _wrap_wsl_command(cmd)

    p = subprocess.run(
        cmd,
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd=cwd,
        text=True
    )
    return p.stdout.strip()


def ProcessLinux(cmd: str, cwd: Optional[str] = None) -> subprocess.CompletedProcess:
    """
    Exécute une commande Linux et renvoie le CompletedProcess.
    """
    if "windows" in platform_available and "wsl" in platform_available:
        cmd = _wrap_wsl_command(cmd)

    return subprocess.Popen(
        cmd,
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd=cwd,
        text=True
    )


def ProcessLinuxValue(cmd: str, *args, **kargs) -> int:
    """
    Exécute une commande Linux et renvoie le code de retour.
    """
    if "windows" in platform_available and "wsl" in platform_available:
        cmd = _wrap_wsl_command(cmd)

    kargs.setdefault("shell", True)  # n'ajoute que si absent

    return subprocess.run(cmd, *args, **kargs).returncode


def DetectPackageManager() -> str:
    if ProcessLinux("apt --version").wait() == 0:
        return "apt"
    if ProcessLinux("pacman --version").wait() == 0:
        return "pacman"
    raise RuntimeError("Unsupported system. Only apt/pacman supported.")


# ------------------------------------------------------------
# Conversion de chemin Windows -> Linux (via WSL)
# ------------------------------------------------------------

def GetPathLinux(path: str, relative: Optional[str] = None) -> str:
    """
    Convertit un chemin Windows en chemin Linux (via WSL) sur Windows,
    ou renvoie un chemin relatif natif sur Linux.
    """
    path = os.path.abspath(path)
    if not os.path.exists(path):
        raise FileNotFoundError(f"Path does not exist: {path}")

    if platform == "linux":
        if relative:
            return os.path.relpath(path, relative)
        return path

    if "windows" in platform_available and "wsl" in platform_available:
        # On se place dans le dossier parent
        target = Path(path)
        file_part = ""
        if target.is_file():
            file_part = target.name
            target = target.parent

        cmd = _wrap_wsl_command("pwd")
        linux_path = subprocess.run(
            cmd, shell=True, cwd=str(target),
            stdout=subprocess.PIPE, text=True
        ).stdout.strip()

        if file_part:
            linux_path = f"{linux_path}/{file_part}"

        if relative:
            rel = GetPathLinux(relative)
            cmd = _wrap_wsl_command(f"realpath --relative-to={rel} {linux_path}")
            linux_path = subprocess.run(
                cmd, shell=True,
                stdout=subprocess.PIPE, text=True
            ).stdout.strip()

        return linux_path

    raise RuntimeError("GetPathLinux: WSL not available on this system.")
