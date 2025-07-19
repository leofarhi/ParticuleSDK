import os
import shutil
from ParticuleCraft.utils.font_converter import convert_font_to_binary_file

class RefactoredAsset:
    def __init__(self, category, asset_data, reference_path):
        self.category = category
        self.asset_data = asset_data
        self.reference_path = reference_path


class AssetManager:
    def __init__(self, builder):
        self.builder = builder
        self.current_id = 0
        self.refactored_assets:list[RefactoredAsset] = []

    def _prepare_asset_path(self, src, category, data) -> str:
        dst = os.path.join(self.builder.bin_dir, self.builder.config_data["output_assets_dir"], str(self.current_id)+".asset")
        refactored_asset = RefactoredAsset(category, data, data["reference_path"])
        if not os.path.exists(src):
            print(f"[ERROR] Missing file: {src}")
            return None
        self.refactored_assets.append(refactored_asset)
        self.current_id += 1
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        return dst

    def _check_and_copy(self, src, category, data):
        dst = self._prepare_asset_path(src, category, data)
        if dst is None:
            return
        if os.path.exists(dst) and open(src, "rb").read() == open(dst, "rb").read():
            return
        print(f"Copying asset: {src} → {dst}")
        shutil.copy(src, dst)

    def prepare_all(self):
        self._copy_category("textures")
        self._copy_category("audio")
        self._copy_category("other")
        self._convert_fonts()

    def _copy_category(self, category: str):
        for asset in self.builder.config_data["assets_files"].get(category, []):
            src = os.path.join(self.builder.project_path, asset["path"])
            self._check_and_copy(src, category, asset)

    def _convert_fonts(self):
        for font in self.builder.config_data["assets_files"]["fonts"]:
            src = os.path.join(self.builder.project_path, font["path"])
            if font["resolution"] <= 0:
                raise ValueError("Font resolution must be greater than 0.")
            dst = self._prepare_asset_path(src, "fonts", font)
            if dst is None:
                continue
            convert_font_to_binary_file(src, font["resolution"], dst, font["charset"])
            print(f"Converted font: {src} → {dst}")
