import os
import shutil
from ParticuleCraft.modules.asset_manager import AssetManager
from ParticuleCraft.utils.font_converter import convert_font_to_binary_file
from ParticuleCraft.utils.multi_platform import GetPathLinux

class AssetManagerCG(AssetManager):

    def prepare_all(self) -> None:
        self.uuid_manager = self.builder.uuid_manager
        self.asset_list = []
        self._prepare_texture_assets(True)
        self._prepare_font_assets()
        self._prepare_texture_assets(False)
        self._write_metadata()
        self.uuid_manager.save()

    def _check_and_copy(self, src, dst, category, data):
        self._prepare_asset_path(src, category, data)
        if os.path.exists(dst) and open(src, "rb").read() == open(dst, "rb").read():
            return
        print(f"Copying asset: {src} → {dst}")
        shutil.copy(src, dst)

    def _prepare_texture_assets(self, builtIn) -> None:
        asset_build = os.path.join(self.builder.build_dir, "assets")
        os.makedirs(asset_build, exist_ok=True)

        if builtIn:
            for texture in self.builder.config_data["assets_files"]["textures"]:
                if texture["external"]:
                    continue
                src = os.path.join(self.builder.project_path, texture["path"])
                uuid = self.uuid_manager.add(texture["path"], "assets.textures")
                dst = os.path.join(asset_build, uuid + os.path.splitext(src)[1])
                self._check_and_copy(src, dst, "textures", texture)
                self.asset_list.append((uuid, texture))
    def _prepare_font_assets(self) -> None:
        for font in self.builder.config_data["assets_files"]["fonts"]:
            src = os.path.join(self.builder.project_path, font["path"])
            uuid = self.uuid_manager.add(font["path"], "assets.fonts")
            dst = os.path.join(self.builder.build_dir, "assets", uuid + ".font")
            self._prepare_asset_path(src, "fonts", font)
            convert_font_to_binary_file(src, font["resolution"], dst, font["charset"])

    def _write_metadata(self) -> None:
        path = os.path.join(self.builder.build_dir, "assets", "fxconv-metadata.txt")
        content = ""
        for uuid, tex in self.asset_list:
            fmt = tex["format"]
            if tex["alpha"] and fmt == "rgb565":
                fmt += "a"
            content += f"{uuid + os.path.splitext(tex['path'])[1]}:\n"
            content += f"\tname: ___IMG_{uuid}\n\ttype: bopti-image\n\tprofile: {fmt}\n"

        for font in self.builder.config_data["assets_files"]["fonts"]:
            uuid = self.uuid_manager.get_uuid(font["path"])
            content += f"{uuid}.font:\n\tcustom-type: api_font\n\tname: ___FONT_{uuid}\n"

        if not os.path.exists(path) or open(path).read() != content:
            with open(path, "w") as f:
                f.write(content)