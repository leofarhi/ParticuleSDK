from . import working_dirs as wd
from .log import logger
import os
import sys
import importlib.util
import importlib
from typing import Dict

Distributions: Dict[str, Dict[str, type]] = {}


"""
#Ancien code :
def load_distributions():
    REQUIRED_FILES = [
        "Installer.py",
        "MakeAppConfig.py",
        "MakeLibConfig.py",
    ]

    base_dir = os.path.join(wd.distributions_path)

    for root, dirs, files in os.walk(base_dir):
        if all(req_file in files for req_file in REQUIRED_FILES):

            rel_path = os.path.relpath(root, base_dir)
            dist_name = rel_path.replace(os.sep, ".") if rel_path != "." else ""

            attrs = {}
            success = True

            for file_name in REQUIRED_FILES:
                module_name = file_name[:-3]
                module_path = os.path.join(root, file_name)

                try:
                    spec = importlib.util.spec_from_file_location(
                        module_name, module_path
                    )
                    if spec and spec.loader:
                        module = importlib.util.module_from_spec(spec)
                        spec.loader.exec_module(module)
                    else:
                        logger.error(f"Failed to load spec for {module_path}")
                        success = False
                        break

                except Exception as e:
                    logger.error(f"Could not load module '{module_name}' from '{module_path}': {e}")
                    success = False
                    break

                if hasattr(module, module_name):
                    attrs[module_name] = getattr(module, module_name)
                else:
                    logger.error(f"Class '{module_name}' not found in {module_path}")
                    success = False
                    break

            if success:
                Distributions[dist_name] = attrs
                logger.info(f"[OK] Loaded distribution: {dist_name}")
    logger.info(f"{len(Distributions)} distributions loaded.")
"""


def load_distributions():
    """
    Parcourt le dossier Distributions/ récursivement.
    Pour chaque sous-arborescence contenant tous les modules requis,
    importe les classes et enregistre dans Distributions.
    """
    global Distributions

    REQUIRED_FILES = [
        "Installer.py",
        "MakeAppConfig.py",
        "MakeLibConfig.py",
    ]

    if wd.sdk_path not in sys.path:
        sys.path.append(wd.sdk_path)
    if os.path.dirname(wd.distributions_path) not in sys.path:
        sys.path.append(os.path.dirname(wd.distributions_path))

    base_dir = os.path.join(wd.distributions_path)
    
    for root, dirs, files in os.walk(base_dir):
        # Vérifie que tous les fichiers requis sont dans ce dossier
        if all(req_file in files for req_file in REQUIRED_FILES):

            # Chemin relatif de base_dir à root
            rel_path = os.path.relpath(root, base_dir)
            # dist_name : style Windows, Linux, Casio.CG, etc.
            dist_name = rel_path.replace(os.sep, ".")

            attrs = {}
            success = True

            for file_name in REQUIRED_FILES:
                module_name = file_name[:-3]  # Remove .py
                full_module_name = f"Distributions.{dist_name}.{module_name}" if dist_name else f"Distributions.{module_name}"
                try:
                    module = importlib.import_module(full_module_name)
                except Exception as e:
                    logger.error(f"Could not import module '{full_module_name}': {e}")
                    success = False
                    break

                if hasattr(module, module_name):
                    attrs[module_name] = getattr(module, module_name)
                else:
                    logger.error(f"Class '{module_name}' not found in module '{full_module_name}'")
                    success = False
                    break

            if success:
                Distributions[dist_name] = attrs
                logger.info(f"[OK] Loaded distribution: {dist_name}")
    logger.info(f"{len(Distributions)} distributions loaded.")

