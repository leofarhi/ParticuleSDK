import os
import shutil
import json
from ParticuleCraft.utils.font_converter import convert_font_to_binary_file

class RefactoredAsset:
    def __init__(self, category, data, reference_path):
        self.category = category
        self.data = data
        self.reference_path = reference_path


class AssetManager:
    def __init__(self, builder):
        self.builder = builder
        self.refactored_assets: list[RefactoredAsset] = []

    def _inject_sprites_from_json(self, texture_data: dict, sprites_json: list[dict]) -> None:
        """
        Insère en tête de self.refactored_assets des RefactoredAsset('sprites', ...)
        à partir d'une liste JSON (contenu du fichier .sprites). Ne touche pas au disque.
        """
        # On récupère un identifiant de texture stable pour référencer les sprites
        texture_reference_path = texture_data.get("reference_path") or texture_data.get("path")
        if not texture_reference_path:
            return

        # On insère en ordre d'origine malgré insert(0) → on itère à l'envers
        for raw in reversed(sprites_json or []):
            if not isinstance(raw, dict) or "name" not in raw:
                continue
            sprite_data = dict(raw)  # copie défensive
            sprite_data["texture"] = texture_reference_path  # rattache la texture
            ref_path = f"{texture_reference_path}:{sprite_data['name']}"
            self.refactored_assets.insert(0, RefactoredAsset("sprites", sprite_data, ref_path))

    # --- PREPARE PHASE (ne touche pas au disque) ---

    def _prepare_asset_entry(self, category: str, data: dict) -> None:
        """Enregistre un asset à traiter plus tard par export_all."""
        # On garde reference_path comme avant pour compat.
        self.refactored_assets.append(
            RefactoredAsset(category, data, data.get("reference_path"))
        )

    def _collect_category(self, category: str) -> None:
        for asset in self.builder.config_data["assets_files"].get(category, []):
            self._prepare_asset_entry(category, asset)

    def _collect_textures_with_sprites(self) -> None:
        """
        Enregistre toutes les textures comme avant, et, si include_sprites == True
        et que <path>.sprites existe, insère des 'sprites' artificiels en tête.
        """
        for tex in self.builder.config_data["assets_files"].get("textures", []):
            # Enregistre l'asset texture comme d'habitude
            self._prepare_asset_entry("textures", tex)

            # Sprites optionnels
            if tex.get("include_sprites"):
                sprites_path = os.path.join(self.builder.project_path, tex["path"] + ".sprites")
                if os.path.exists(sprites_path):
                    try:
                        with open(sprites_path, "r", encoding="utf-8") as f:
                            sprites_json = json.load(f)
                        if isinstance(sprites_json, list):
                            self._inject_sprites_from_json(tex, sprites_json)
                    except Exception as e:
                        print(f"[WARN] Failed to read sprites for {tex.get('path')}: {e}")
                # sinon : considéré comme include_sprites == False (rien à faire)

    def _collect_fonts(self) -> None:
        for font in self.builder.config_data["assets_files"].get("fonts", []):
            self._prepare_asset_entry("fonts", font)

    def prepare_all(self) -> None:
        """Ne fait plus que remplir self.refactored_assets."""
        self.refactored_assets.clear()
        self._collect_textures_with_sprites()
        self._collect_category("audio")
        self._collect_category("other")
        self._collect_fonts()

    # --- EXPORT PHASE (écrit sur le disque) ---

    def _dst_path_for_index(self, index: int) -> str:
        out_dir = os.path.join(
            self.builder.bin_dir, self.builder.config_data["output_assets_dir"]
        )
        os.makedirs(out_dir, exist_ok=True)
        return os.path.join(out_dir, f"{index}.asset")

    def _src_from_data(self, data: dict) -> str:
        """Construit le chemin source à partir des données de config."""
        return os.path.join(self.builder.project_path, data["path"])

    def _copy_if_needed(self, src: str, dst: str) -> None:
        if not os.path.exists(src):
            print(f"[ERROR] Missing file: {src}")
            return
        if os.path.exists(dst):
            with open(src, "rb") as fsrc, open(dst, "rb") as fdst:
                if fsrc.read() == fdst.read():
                    return  # Rien à faire, déjà identique
        print(f"Copying asset: {src} → {dst}")
        shutil.copy(src, dst)

    def export_all(self) -> None:
        """
        Parcourt self.refactored_assets et effectue les copies/conversions
        vers {output_assets_dir}/{index}.asset
        """
        for idx, asset in enumerate(self.refactored_assets):
            if asset.category == "sprites":
                # Les sprites sont builtin
                continue
            dst = self._dst_path_for_index(idx)
            if asset.category == "fonts":
                data = asset.data
                src = self._src_from_data(data)

                # Validation au moment de l’export
                if data.get("resolution", 0) <= 0:
                    print(f"[ERROR] Font resolution must be greater than 0. ({src})")
                    continue
                if not os.path.exists(src):
                    print(f"[ERROR] Missing file: {src}")
                    continue

                convert_font_to_binary_file(
                    src,
                    data["resolution"],
                    dst,
                    data["charset"],
                )
                print(f"Converted font: {src} → {dst}")

            else:
                # Catégories "textures", "audio", "other" (et tout autre cas par défaut : copie)
                src = self._src_from_data(asset.data)
                self._copy_if_needed(src, dst)
