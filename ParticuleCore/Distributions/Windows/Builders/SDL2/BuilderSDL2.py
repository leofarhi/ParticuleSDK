import os
import shutil
import subprocess
import json
from typing import Dict, List
from ParticuleCraft.core.builder_base import Builder
from ParticuleCraft.modules.asset_manager import AssetManager
from ParticuleCraft.modules.redefine_manager import RedefineManager
from ParticuleCraft.modules.uuid_manager import UUIDManager
from .MakefileGeneratorMSVC import MakefileGeneratorMSVC
from ParticuleCraft.utils import normalize_path


class BuilderSDL2(Builder):
    def __init__(self, config, makefile_path: str) -> None:
        super().__init__(config, makefile_path)
        self.asset_manager = AssetManager(self)
        self.uuid_manager = UUIDManager(os.path.join(self.build_dir, "uuid.json"))
        self.distribution_path = os.path.dirname(self.config.__config_path__)
        self.SDL2_Project = os.path.join(self.distribution_path, "Libs", "SDL2_Project")

    def prepare_assets(self) -> None:
        self.asset_manager.prepare_all()
        self.asset_manager.export_all()

    def prepare_headers(self) -> None:
        redefine = RedefineManager()
        redefine.includes.append("<Particule/Core/System/sdl2.hpp>")
        redefine.includes.append("<Particule/Core/System/References/Resource.hpp>")
        for asset in self.asset_manager.refactored_assets:
            if asset.category == "sprites":
                redefine.asset_declarations.append(["Sprite", f"GetResourceID(\"{asset.data['texture']}\")",f"Rect({asset.data['x']}, {asset.data['y']}, {asset.data['w']}, {asset.data['h']})"])
        for k, v in self.config_data["inputs"].items():
            redefine.input_mappings.append((k, f"sdl2::{v}"))
        for idx, asset in enumerate(self.asset_manager.refactored_assets):
            redefine.resource_mappings.append((asset.reference_path, idx))
        redefine.assets_path = self.config_data["output_assets_dir"]
        redefine.save_code(self.build_dir)

    def prepare_makefile(self) -> None:
        generator = MakefileGeneratorMSVC(self)
        generator.generate_makefile()
        generator.generate_build_script()

    def run_compilation(self) -> None:
        subprocess.run([".\\Build.bat"], shell=True, cwd=self.build_dir, check=True)

    def post_build(self) -> None:
        bin_dir = os.path.join(self.project_path, self.config_data["bin_dir"])
        os.makedirs(bin_dir, exist_ok=True)

        exe_path = os.path.join(self.build_dir, "Bin", f"{self.config_data['output_file']}.exe")
        if os.path.exists(exe_path):
            shutil.copy(exe_path, os.path.join(bin_dir, os.path.basename(exe_path)))
        else:
            raise FileNotFoundError(f"Build failed: {exe_path} not found.")

        # Copie des DLLs SDL2
        dlls = [
            "SDL2.dll", "SDL2_image.dll", "SDL2_ttf.dll", "libfreetype-6.dll",
            "libjpeg-9.dll", "libpng16-16.dll", "zlib1.dll", "libtiff-5.dll",
            "libwebp-7.dll"
        ]
        lib_path = os.path.join(self.SDL2_Project, "Lib", self.config_data["architecture"])
        for dll in dlls:
            full_path = os.path.join(lib_path, dll)
            if os.path.exists(full_path):
                shutil.copy(full_path, bin_dir)
            else:
                print(f"[WARNING] DLL not found: {full_path}")

    def success_check(self) -> bool:
        exe = os.path.join(self.project_path, self.config_data["bin_dir"], self.config_data["output_file"] + ".exe")
        return os.path.isfile(exe)
