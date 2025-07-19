@echo off
setlocal

REM Récupère le chemin absolu du dossier contenant ce script
set "SCRIPT_DIR=%~dp0"

REM Chemin vers le dossier racine du module (deux niveaux au-dessus du script)
set "MODULE_ROOT=%SCRIPT_DIR%..\..\.."

REM Convertit en chemin absolu
for %%I in ("%MODULE_ROOT%") do set "ABS_MODULE_ROOT=%%~fI"

REM Détermine le chemin de l'interpréteur Python dans l'environnement virtuel
set "VENV_PYTHON=%SCRIPT_DIR%..\.venv\Scripts\python.exe"

REM Convertit en chemin absolu
for %%I in ("%VENV_PYTHON%") do set "ABS_VENV_PYTHON=%%~fI"

REM Lance le module avec l'interpréteur de l'environnement virtuel sans changer de répertoire
set "PYTHONPATH=%ABS_MODULE_ROOT%"
"%ABS_VENV_PYTHON%" -m ParticuleCraft %*

endlocal
