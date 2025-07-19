#!/bin/bash

# Récupère le chemin absolu du dossier contenant ce script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Construit le chemin vers ../../ParticuleCraft depuis le script
MODULE_ROOT="$(realpath "$SCRIPT_DIR/../../../")"

# Construit le chemin vers l'interpréteur Python dans le venv
VENV_PYTHON="$(realpath "$SCRIPT_DIR/../.venv/bin/python")"

# Définit PYTHONPATH temporairement et exécute le module sans changer de répertoire courant
PYTHONPATH="$MODULE_ROOT" "$VENV_PYTHON" -m ParticuleCraft "$@"
