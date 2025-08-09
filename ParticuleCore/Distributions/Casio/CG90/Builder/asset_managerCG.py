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
    """
    Prépare la liste des assets en mémoire (aucun I/O dans prepare_all),
    puis réalise les copies/conversions et la génération de metadata dans export_all.
    Ordonnancement demandé : builtin en tête (insert(0)), fonts ensuite, external à la fin.
    """

    def prepare_all(self) -> None:
        self.uuid_manager = self.builder.uuid_manager
        self.asset_list: list[tuple[str, dict]] = []  # (uuid, texture_data) pour metadata

        # On ne touche pas au disque ici.
        self.refactored_assets.clear()

        # 1) Textures builtin (external == False) -> insert(0)
        for tex in self.builder.config_data["assets_files"].get("textures", []):
            if not tex.get("external", False):
                # Réserve/garantit un UUID (utile plus tard pour le build + metadata)
                uuid = self.uuid_manager.add(tex["path"], "assets.textures")
                # Liste pour metadata fxconv
                self.asset_list.append((uuid, tex))
                # Enregistrer un RefactoredAsset et l'insérer en tête
                self.refactored_assets.insert(0, RefactoredAsset("textures", tex, tex.get("reference_path")))

        # 2) Fonts builtin -> insert(0)
        for font in self.builder.config_data["assets_files"].get("fonts", []):
            # Garantit l'existence d'un UUID pour la police (même si on le re-récupère plus tard)
            self.uuid_manager.add(font["path"], "assets.fonts")
            self.refactored_assets.insert(0, RefactoredAsset("fonts", font, font.get("reference_path")))

        # 3) Textures external -> append (à la fin)
        for tex in self.builder.config_data["assets_files"].get("textures", []):
            if tex.get("external", False):
                self.refactored_assets.append(RefactoredAsset("textures", tex, tex.get("reference_path")))

        # NB: Pas d'écriture de metadata/uuid ici ; ça part dans export_all()

    # --- Helpers internes spécifiques CG ---

    def _build_assets_dir(self) -> str:
        d = os.path.join(self.builder.build_dir, "assets")
        os.makedirs(d, exist_ok=True)
        return d

    def _src_from_data(self, data: dict) -> str:
        return os.path.join(self.builder.project_path, data["path"])

    def _copy_if_needed_to(self, src: str, dst: str) -> None:
        """Copie src -> dst si nécessaire (utilise la logique de comparaison binaire)."""
        if not os.path.exists(src):
            print(f"[ERROR] Missing file: {src}")
            return
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        if os.path.exists(dst):
            with open(src, "rb") as fsrc, open(dst, "rb") as fdst:
                if fsrc.read() == fdst.read():
                    return
        print(f"Copying asset: {src} → {dst}")
        shutil.copy(src, dst)

    def _texture_profile(self, tex: dict) -> str:
        profile = tex.get("format", "")
        if "rgb565" not in profile:
            profile += "_rgb565"
        if tex.get("alpha"):
            profile += "a"
        return profile

    def _write_metadata(self) -> None:
        path = os.path.join(self.builder.build_dir, "assets", "fxconv-metadata.txt")
        content = ""

        # Textures builtin
        for uuid, tex in self.asset_list:
            fmt = tex["format"]
            if tex.get("alpha") and fmt == "rgb565":
                fmt += "a"
            content += f"{uuid + os.path.splitext(tex['path'])[1]}:\n"
            content += f"\tname: ___IMG_{uuid}\n\ttype: bopti-image\n\tprofile: {fmt}\n"

        # Fonts
        for font in self.builder.config_data["assets_files"].get("fonts", []):
            uuid = self.uuid_manager.get_uuid(font["path"])
            content += f"{uuid}.font:\n\tcustom-type: api_font\n\tname: ___FONT_{uuid}\n"

        # Écrire seulement si différent
        if not os.path.exists(path) or open(path, "r", encoding="utf-8").read() != content:
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path, "w", encoding="utf-8") as f:
                f.write(content)

    # --- Phase d'export (I/O) ---

    def export_all(self) -> None:
        """
        Réalise:
          - Textures builtin : copie -> build/assets/{uuid}{.ext}
          - Fonts           : conversion -> build/assets/{uuid}.font
          - Textures external : export -> {output_assets_dir}/{index}.asset via ExportTexture()
          - (fallback) autres catégories : copie -> {output_assets_dir}/{index}.asset
          - Metadata fxconv + sauvegarde UUID
        """
        build_assets = self._build_assets_dir()

        for idx, asset in enumerate(self.refactored_assets):
            cat = asset.category
            data = asset.data
            src = self._src_from_data(data)

            # Destination "indexée" (seulement pour les assets qu'on écrit dans output_assets_dir)
            dst_indexed = self._dst_path_for_index(idx)

            if cat == "textures":
                if data.get("external", False):
                    # EXTERNAL: export dans l'output indexé
                    if not os.path.exists(src):
                        print(f"[ERROR] Missing file: {src}")
                        continue
                    profile = self._texture_profile(data)
                    ExportTexture(src, dst_indexed, profile)
                    print(f"Exported texture (external): {src} [{profile}] → {dst_indexed}")
                else:
                    # BUILTIN: copie vers build/assets/{uuid}{.ext}, pas d'écriture dans output_assets_dir
                    uuid = self.uuid_manager.get_uuid(data["path"])
                    ext = os.path.splitext(src)[1]
                    dst_builtin = os.path.join(build_assets, uuid + ext)
                    self._copy_if_needed_to(src, dst_builtin)

            elif cat == "fonts":
                # Conversion vers build/assets/{uuid}.font (pas d'écriture output indexée ici)
                if data.get("resolution", 0) <= 0:
                    print(f"[ERROR] Font resolution must be greater than 0. ({src})")
                    continue
                if not os.path.exists(src):
                    print(f"[ERROR] Missing file: {src}")
                    continue
                uuid = self.uuid_manager.get_uuid(data["path"])
                dst_font = os.path.join(build_assets, uuid + ".font")
                convert_font_to_binary_file(src, data["resolution"], dst_font, data["charset"])
                print(f"Converted font: {src} → {dst_font}")

            else:
                # Fallback : copie simple dans l'output indexé (au cas où)
                self._copy_if_needed(src, dst_indexed)

        # Metadata + sauvegarde UUID
        self._write_metadata()
        self.uuid_manager.save()