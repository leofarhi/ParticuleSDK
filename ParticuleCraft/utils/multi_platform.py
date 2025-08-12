import os
import sys
import subprocess
from pathlib import Path
from typing import Optional, List
from ..system.log import logger
import threading
from contextlib import contextmanager, nullcontext
import logging


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

    #return subprocess.run(cmd, *args, **kargs).returncode
    result = subprocess.run(cmd, *args, **kargs)
    return result.returncode


@contextmanager
def mute_console_handlers(log: logging.Logger):
    saved = []
    for h in log.handlers:
        if isinstance(h, logging.StreamHandler) and getattr(h, "stream", None) in (sys.stdout, sys.stderr):
            saved.append((h, h.level))
            h.setLevel(1000)
    try:
        yield
    finally:
        for h, lvl in saved:
            h.setLevel(lvl)

@contextmanager
def force_console_utf8():
    """
    Force temporairement stdout/stderr en UTF-8 si l'IO le permet (Python 3.7+).
    Évite les caractères bizarres dans le terminal.
    """
    out_enc = getattr(sys.stdout, "encoding", None)
    err_enc = getattr(sys.stderr, "encoding", None)
    try:
        if hasattr(sys.stdout, "reconfigure"):
            sys.stdout.reconfigure(encoding="utf-8", errors="replace")
        if hasattr(sys.stderr, "reconfigure"):
            sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        # Pas grave si on ne peut pas reconfigurer
        pass
    try:
        yield
    finally:
        try:
            if hasattr(sys.stdout, "reconfigure") and out_enc:
                sys.stdout.reconfigure(encoding=out_enc)
            if hasattr(sys.stderr, "reconfigure") and err_enc:
                sys.stderr.reconfigure(encoding=err_enc)
        except Exception:
            pass

def _tee_stream(stream, write_func, log_func):
    for line in iter(stream.readline, ''):
        write_func(line)                 # affichage terminal
        log_func(line.rstrip('\n'))      # log fichier
    stream.close()

# --- ta fonction ---

def ProcessLinuxValue(cmd: str, *args, **kargs) -> int:
    """
    Exécute une commande Linux/WSL, n'affiche que la sortie du process au terminal,
    loggue stdout (INFO) et stderr (ERROR) en UTF-8, et renvoie le code de retour.
    """
    if "windows" in platform_available and "wsl" in platform_available:
        cmd = _wrap_wsl_command(cmd)

    # paramètres par défaut
    kargs.setdefault("shell", True)
    kargs.setdefault("text", True)            # mode texte
    kargs.setdefault("encoding", "utf-8")     # <-- décodage UTF-8 explicite
    kargs.setdefault("errors", "replace")     # évite les plantages sur caractères exotiques
    kargs.setdefault("bufsize", 1)            # line-buffered (Unix)
    kargs["stdout"] = subprocess.PIPE
    kargs["stderr"] = subprocess.PIPE

    check = kargs.pop("check", False)

    # Pas de doublon logger console + forcer terminal en UTF-8 pendant le run
    with mute_console_handlers(logger), force_console_utf8():
        proc = subprocess.Popen(cmd, *args, **kargs)

        t_out = threading.Thread(
            target=_tee_stream,
            args=(proc.stdout, lambda s: (sys.stdout.write(s), sys.stdout.flush()), logger.info),
            daemon=True
        )
        t_err = threading.Thread(
            target=_tee_stream,
            args=(proc.stderr, lambda s: (sys.stderr.write(s), sys.stderr.flush()), logger.error),
            daemon=True
        )

        t_out.start(); t_err.start()
        returncode = proc.wait()
        t_out.join(); t_err.join()

    if check and returncode != 0:
        raise subprocess.CalledProcessError(returncode, cmd)

    return returncode


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
