import os
import json
from ParticuleCraft.utils import normalize_path
from ParticuleCraft.modules.uuid_manager import UUIDManager
from ParticuleCraft.system.working_dirs import interface_path, engine_path


class MakefileGeneratorMSVC:
    def __init__(self, builder) -> None:
        self.builder = builder
        self.config = builder.config_data
        self.build_dir = builder.build_dir
        self.uuid_manager: UUIDManager = builder.uuid_manager
        self.distribution_path = os.path.dirname(self.builder.config.__config_path__)

        self.output_file = self.config["output_file"]
        self.architecture = self.config["architecture"]
        self.icon = normalize_path(self.config["icon"]) if self.config["icon"] else ""
        self.no_console = int(not self.config["console"])
        self.include_paths = self.config["include_paths"]
        self.source_files = self.config["source_files"]
        self.project_name = self.config["project_name"]
        self.library_paths = self.config.get("library_paths", [])
        self.defines = self.config.get("defines", {})

    def generate_makefile(self) -> None:
        # Préparation des chemins
        output_name = self.output_file
        include_dirs = self._collect_includes()
        all_src_files = self._collect_source_files()

        obj_defs, rule_defs, obj_refs = self._generate_objects_and_rules(all_src_files)

        makefile_content = f"""
OUTPUT_NAME = {output_name}
BUILD_DIR = Build
OUTPUT_DIR = Bin
NO_CONSOLE = {self.no_console}
ICON_PATH = "{self.icon}"

INCLUDES = {' '.join(f'/I"{inc}"' for inc in include_dirs)}
LIBFLAGS = {" ".join(self._get_library_flags())}
OBJ_FILES = {' '.join(obj_refs)}

{obj_defs}

!if $(ICON_PATH) != ""
RES_FILE = $(BUILD_DIR)\\icon.res
RES_RC = $(BUILD_DIR)\\icon.rc
!endif

!if $(NO_CONSOLE)
SUBSYSTEM = /SUBSYSTEM:WINDOWS
!else
SUBSYSTEM = /SUBSYSTEM:CONSOLE
!endif

all: icon.res $(OUTPUT_DIR)\\$(OUTPUT_NAME).exe

$(OUTPUT_DIR)\\$(OUTPUT_NAME).exe: $(OBJ_FILES)
\t@cmd /C "if not exist $(OUTPUT_DIR) mkdir $(OUTPUT_DIR)"
\tlink $(OBJ_FILES) $(LIBFLAGS) $(RES_FILE) $(SUBSYSTEM) /OUT:$@

{rule_defs}

icon.res:
!if $(ICON_PATH) != ""
\t@cmd /C "if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)"
\t@echo IDI_ICON1 ICON $(ICON_PATH) > $(RES_RC)
\trc /fo $(RES_FILE) $(RES_RC)
!endif
"""
        with open(os.path.join(self.build_dir, "Makefile"), "w", encoding="utf-8") as f:
            f.write(makefile_content.strip())

        self.uuid_manager.save()

    def generate_build_script(self) -> None:
        config_path = os.path.join(self.distribution_path,"Compiler", "MSVC", "config.json")
        if not os.path.exists(config_path):
            raise FileNotFoundError(f"Configuration file not found: {config_path}")
        with open(config_path, "r") as f:
            json_data = json.load(f)

        vcvars_path = json_data.get(self.architecture, "")

        build_script = f"""
@echo off
setlocal

mkdir Build 2>nul
mkdir Bin 2>nul

:: Définir le chemin vers vcvars
set VCVARS="{vcvars_path}"

:: Vérifie si le fichier existe
if not exist %VCVARS% (
    echo Erreur : vcvars introuvable.
    pause
    exit /b 1
)

:: Appel de l'environnement de compilation Visual Studio
call %VCVARS%

nmake /f Makefile
endlocal
"""
        with open(os.path.join(self.build_dir, "Build.bat"), "w", encoding="utf-8") as f:
            f.write(build_script.strip())

    def _collect_includes(self) -> list:
        includes = [
            os.path.join(self.build_dir, "include"),
            os.path.join(self.distribution_path, "Sources", "SDL2", "include"),
            os.path.join(interface_path, "include"),
        ]
        if self.config["include_engine"]:
            includes.append(os.path.join(engine_path, "include"))
        includes.extend(self.include_paths)
        return [normalize_path(p) for p in includes if os.path.isdir(p)]

    def _collect_source_files(self) -> list:
        all_files = []
        all_files.append(os.path.join(self.build_dir, "Resources.cpp"))

        for root, _, files in os.walk(os.path.join(self.distribution_path, "Sources", "SDL2", "src")):
            for file in files:
                if file.endswith((".cpp", ".c")):
                    all_files.append(os.path.join(root, file))

        if self.config["include_engine"]:
            engine_src = os.path.join(engine_path, "src")
            for root, _, files in os.walk(engine_src):
                for file in files:
                    if file.endswith((".cpp", ".c")):
                        all_files.append(os.path.join(root, file))

        for src in self.source_files:
            if os.path.exists(src):
                all_files.append(src)
            else:
                print(f"[WARNING] Source file not found: {src}")

        return [normalize_path(path) for path in all_files]

    def _generate_objects_and_rules(self, src_files: list) -> tuple:
        obj_defs = []
        rule_defs = []
        obj_refs = []

        for src in src_files:
            uuid = self.uuid_manager.add(src, "code")
            var_src = f"SRC_{uuid}"
            var_obj = f"OBJ_{uuid}"
            obj_file = f"$(BUILD_DIR)\\{uuid}.obj"

            obj_defs.append(f"{var_src} = {src}")
            obj_defs.append(f"{var_obj} = {obj_file}")
            obj_refs.append(f"$({var_obj})")

            if src.endswith(".cpp"):
                compile_cmd = "cl /nologo /std:c++20 /EHsc /Zi /MD $(INCLUDES) /Fo$@ -c $**"
            elif src.endswith(".c"):
                compile_cmd = "cl /nologo /std:c11 /Zi /MD $(INCLUDES) /Fo$@ -c $**"
            else:
                continue

            rule = f"$({var_obj}): $({var_src})\n\t{compile_cmd}\n"
            rule_defs.append(rule)

        return "\n".join(obj_defs), "\n".join(rule_defs), obj_refs

    def _get_library_flags(self) -> list:
        libs = [
            "SDL2.lib", "SDL2_image.lib", "SDL2_ttf.lib"
        ]
        lib_path = normalize_path(os.path.join(self.builder.SDL2_Project, "Lib", self.architecture))
        return [f'"{os.path.join(lib_path, lib)}"' for lib in libs]
