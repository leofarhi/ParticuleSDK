import os
import shutil
from ParticuleCraft.modules.asset_manager import AssetManager, RefactoredAsset
from ParticuleCraft.utils.font_converter import convert_font_to_binary_file
from ParticuleCraft.utils.multi_platform import GetPathLinux
from . import fxconv
import struct

def convert_image_cg(input,profile="rgb565a"):
    params = {"type": "bopti-image","profile": profile}
    if isinstance(input, fxconv.Image.Image):
        img = input.copy()
    else:
        img = fxconv.Image.open(input)
    if img.width >= 65536 or img.height >= 65536:
        raise fxconv.FxconvError(f"'{input}' is too large (max. 65535x65535)")
    # Crop image to key "area"
    area = fxconv.Area(params.get("area", {}), img)
    img = img.crop(area.tuple())
    img = img.convert("RGBA")
    format_name = params.get("profile", "")
    has_alpha = fxconv.image_has_alpha(img)
    if format_name == "":
        format_name = "rgb565a" if has_alpha else "rgb565"
    elif format_name == "p8":
        format_name = "p8_rgb565a" if has_alpha else "p8_rgb565"
    elif format_name == "p4":
        format_name = "p4_rgb565a" if has_alpha else "p4_rgb565"
    # Otherwise, just use the format as specified
    format = fxconv.CgProfile.find(format_name)
    if format is None:
        raise fxconv.FxconvError(f"unknown image format '{format_name}")
    # Check that we have transparency support if we need it
    if has_alpha and not format.has_alpha:
        raise fxconv.FxconvError(f"'{input}' has transparency, which {format_name} "+
            "doesn't support")
    return *fxconv.image_encode(img, format), format, img

def ExportTexture(pathImage, pathOutput, profile="rgb565a"):
    data, stride, palette, color_count, format, img = convert_image_cg(pathImage, profile)
    with open(pathOutput, 'wb') as f:
        f.write(struct.pack('>B', format.id))
        f.write(struct.pack('>h', color_count))
        f.write(struct.pack('>H', img.width))
        f.write(struct.pack('>H', img.height))
        f.write(struct.pack('>i', stride))
        data = fxconv.ref(data, padding=4) #type is collections.namedtuple("bytes", "data padding")
        f.write(struct.pack('>Q',len(data.target)))
        f.write(data.target)
        if palette == None:
            f.write(struct.pack('>I',0))
        else:
            palette = fxconv.ref(palette)
            f.write(struct.pack('>I',len(palette.target)))
            f.write(palette.target)

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
        external_asset_path = os.path.join(self.builder.bin_dir,self.builder.config_data["output_assets_dir"])
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
        else:
            for texture in self.builder.config_data["assets_files"]["textures"]:
                if not texture["external"]:
                    continue
                src = os.path.join(self.builder.project_path, texture["path"])
                os.makedirs(external_asset_path, exist_ok=True)
                dst = os.path.join(external_asset_path, str(self.current_id) + ".asset")
                refactored_asset = RefactoredAsset("textures", texture, texture["reference_path"])
                if not os.path.exists(src):
                    print(f"[ERROR] Missing file: {src}")
                    return None
                self.refactored_assets.append(refactored_asset)
                self.current_id += 1
                profile = texture["format"]
                if "rgb565" not in profile:
                    profile +="_rgb565"
                profile += "a" if texture["alpha"] else ""
                ExportTexture(src, dst, profile)
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