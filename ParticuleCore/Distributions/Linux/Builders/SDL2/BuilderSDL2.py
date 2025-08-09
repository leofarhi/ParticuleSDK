import os
import shutil
import subprocess
from typing import List
from ParticuleCraft.core.builder_base import Builder
from ParticuleCraft.modules.asset_manager import AssetManager
from ParticuleCraft.modules.uuid_manager import UUIDManager
from ParticuleCraft.modules.redefine_manager import RedefineManager
from .MakefileGenerator import MakefileGenerator
from ParticuleCraft.utils import normalize_path
from ParticuleCraft.utils.multi_platform import *


class BuilderSDL2(Builder):
    def __init__(self, config, makefile_path: str) -> None:
        super().__init__(config, makefile_path)
        self.asset_manager = AssetManager(self)
        self.uuid_manager = UUIDManager(os.path.join(self.build_dir, "uuid.json"))
        self.distribution_path = os.path.dirname(self.config.__config_path__)

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
        generator = MakefileGenerator(self)
        generator.generate_makefile()

    def run_compilation(self) -> None:
        ProcessLinuxValue("make", cwd=self.build_dir, check=True)

    def post_build(self) -> None:
        output_name = self.config_data["output_file"]
        binary_path = os.path.join(self.build_dir, "bin", output_name)
        if not os.path.isfile(binary_path):
            raise FileNotFoundError(f"Build failed: {binary_path} not found.")

        final_path = os.path.join(self.bin_dir, output_name)
        shutil.copy(binary_path, final_path)
        print(f"✅ Copied executable to: {final_path}")

    def success_check(self) -> bool:
        exe = os.path.join(self.bin_dir, self.config_data["output_file"])
        return os.path.isfile(exe)
