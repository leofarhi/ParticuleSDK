import os
import shutil
from ParticuleCraft.core.builder_base import Builder
from ParticuleCraft.modules.uuid_manager import UUIDManager
from ParticuleCraft.modules.redefine_manager import RedefineManager
from ParticuleCraft.utils.multi_platform import GetPathLinux
from ..asset_managerCG import AssetManagerCG
from .MakefileGenerator import MakefileGenerator
from ParticuleCraft.utils.multi_platform import *
from ParticuleCraft.utils import *

class BuilderGint(Builder):
    def __init__(self, config, makefile_path: str) -> None:
        super().__init__(config, makefile_path)
        self.uuid_manager = UUIDManager(os.path.join(self.build_dir, "uuid.json"))
        self.asset_manager = AssetManagerCG(self)
        self.distribution_path = os.path.dirname(self.config.__config_path__)

    def prepare_assets(self) -> None:
        self.asset_manager.prepare_all()
        self.asset_manager.export_all()

    def prepare_makefile(self) -> None:
        generator = MakefileGenerator(self)
        generator.generate_makefile()

    def run_compilation(self) -> None:
        ProcessLinuxValue("fxsdk build-cg", cwd=self.build_dir, check=True)

    def post_build(self) -> None:
        output = self.config_data["output_file"]
        g3a = output + ".g3a"
        src = os.path.join(self.build_dir, g3a)
        dst = os.path.join(self.bin_dir, g3a)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copy(src, dst)
        print(f"✅ Copied {g3a} to: {dst}")

    def success_check(self) -> bool:
        exe = os.path.join(self.bin_dir, self.config_data["output_file"]+ ".g3a")
        return os.path.isfile(exe)
    

    def prepare_headers(self) -> None:
        redefine = RedefineManager()
        redefine.includes.append("<Particule/Core/System/gint.hpp>")
        redefine.includes.append("<Particule/Core/System/References/Resource.hpp>")
        redefine.includes.append("<span>")
        redefine.additional_code_before += "typedef struct {\nuint32_t size;\nuint8_t *data;\n} __gint_lib_font_t;\n\n"
        for asset in self.asset_manager.refactored_assets:
            if asset.category == "sprites":
                redefine.asset_declarations.append(["Sprite", f"GetResourceID(\"{asset.data['texture']}\")",f"Rect({asset.data['x']}, {asset.data['y']}, {asset.data['w']}, {asset.data['h']})"])
            if asset.category == "textures":
                if not asset.data["external"]:
                    uuid = self.uuid_manager.get_uuid(asset.data["path"])
                    redefine.additional_code_before += f"extern gint::image_t ___IMG_{uuid};\n"
                    img_type = "Texture"
                    if "p8" in asset.data["format"]:
                        img_type += "P8"
                    if "p4" in asset.data["format"]:
                        img_type += "P4"
                    redefine.asset_declarations.append([img_type, f"&___IMG_{uuid}"])
            if asset.category == "fonts":
                uuid = self.uuid_manager.get_uuid(asset.data["path"])
                redefine.additional_code_before += f"extern __gint_lib_font_t ___FONT_{uuid};\n"
                redefine.asset_declarations.append(["Font", f"std::span(___FONT_{uuid}.data, ___FONT_{uuid}.size)"])
        for k, v in self.config_data["inputs"].items():
            redefine.input_mappings.append((k, f"gint::{v}"))
        for idx, asset in enumerate(self.asset_manager.refactored_assets):
            redefine.resource_mappings.append((asset.reference_path, idx))
        redefine.assets_path = self.config_data["output_assets_dir"]
        redefine.save_code(self.build_dir)
        return


