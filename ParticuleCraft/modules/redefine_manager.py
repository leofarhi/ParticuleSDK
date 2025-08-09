
import os
from ParticuleCraft.utils import CheckIfContentIsSame

class RedefineManager:
    def __init__(self):
        self.includes: list[str] = [
            "<Particule/Core/Graphics/Image/Texture.hpp>",
            "<Particule/Core/Graphics/Image/Sprite.hpp>",
            "<Particule/Core/Font/Font.hpp>",
            "<Particule/Core/System/Input.hpp>",
            "<Particule/Core/System/Basic.hpp>",
        ]
        # Chaque entrée est la liste de tokens passés à DECLARE_BUILTIN_ASSET
        # ex: ["TextureP4", "___&whatever"]
        self.asset_declarations: list[list[str]] = []

        # ("NomSymbolique", "gint::KEY_UP")  -> utilisé par GetInput
        self.input_mappings: list[tuple[str, str]] = []

        # ("NomRessource", 42)  -> utilisé par GetResourceID
        self.resource_mappings: list[tuple[str, int]] = []

        self.additional_code_before: str = ""
        self.additional_code_after: str = ""

        self.assets_path: str = "assets"

    # ---------- Redefine.hpp ----------
    def _generate_redefine_hpp(self) -> str:
        external_asset_count = len(self.resource_mappings) - len(self.asset_declarations)
        code = []
        code.append("#ifndef REDEFINE_HPP")
        code.append("#define REDEFINE_HPP")
        code.append("")
        code.append("#include <Particule/Core/System/References/Input.hpp>")
        code.append("#include <Particule/Core/System/References/Resource.hpp>")
        code.append("")
        code.append("namespace Particule::Core")
        code.append("{")
        code.append(f"    #define EXTERNAL_ASSET_COUNT {external_asset_count}")
        code.append(f"    #define EXTERNAL_ASSET_PATH \"{self.assets_path}\"")
        code.append("}")
        code.append("")
        code.append("#endif // REDEFINE_HPP")
        return "\n".join(code)

    # ---------- References/Input.hpp ----------
    def _generate_reference_input_hpp(self) -> str:
        inputs_chain = ""
        for key, value in self.input_mappings:
            inputs_chain += f'\nCONST_STR_CMP(str, "{key}") ? Input({value}) : \\'

        code = []
        code.append("#ifndef REFERENCE_INPUT_HPP")
        code.append("#define REFERENCE_INPUT_HPP")
        code.append("")
        code.append("#include <Particule/Core/System/Input.hpp>")
        code.append("#include <Particule/Core/System/Basic.hpp>")
        code.append("")
        code.append("namespace Particule::Core")
        code.append("{")
        code.append("    #ifndef GetInput")
        if inputs_chain:
            code.append(f"    #define GetInput(str)(\\{inputs_chain}\n    Input())")
        else:
            code.append("    #define GetInput(str) Input()")
        code.append("    #endif")
        code.append("")
        code.append("    /*Exemple de redéfinition de GetInput pour le compilateur GCC")
        code.append("    #define GetInput(str)(\\")
        code.append('        CONST_STR_CMP(str, "A") ? Input(gint::KEY_UP) : \\')
        code.append('        CONST_STR_CMP(str, "B") ? Input(gint::KEY_DOWN) : \\')
        code.append("        Input()\\")
        code.append("        )")
        code.append("    */")
        code.append("}")
        code.append("")
        code.append("#endif // REFERENCE_INPUT_HPP")
        return "\n".join(code)

    # ---------- References/Resource.hpp ----------
    def _generate_reference_resource_hpp(self) -> str:
        resources_chain = ""
        for key, value in self.resource_mappings:
            resources_chain += f'\nCONST_STR_CMP(str, "{key}") ? {value} : \\'

        code = []
        code.append("#ifndef REFERENCE_RESOURCE_HPP")
        code.append("#define REFERENCE_RESOURCE_HPP")
        code.append("")
        code.append("#include <Particule/Core/System/Basic.hpp>")
        code.append("")
        code.append("namespace Particule::Core")
        code.append("{")
        code.append("    #ifndef GetResourceID")
        if resources_chain:
            code.append(f"    #define GetResourceID(str)(\\{resources_chain}\n    -1)")
        else:
            code.append("    #define GetResourceID(str) -1 //sera remplacé avec le code")
        code.append("    #endif")
        code.append("}")
        code.append("")
        code.append("#endif // REFERENCE_RESOURCE_HPP")
        return "\n".join(code)

    # ---------- System/Resources.cpp ----------
    def _generate_resources_cpp(self) -> str:
        code = []
        code.append("// Auto-generated resources implementation")
        for include in self.includes:
            code.append(f"#include {include}")
        code.append("")

        # Code additionnel avant
        if self.additional_code_before:
            code.append(self.additional_code_before)
            if not self.additional_code_before.endswith("\n"):
                code.append("")

        code.append("namespace Particule::Core{")
        code.append("")
        # Les déclarations des assets (macro) sont dans le .cpp uniquement
        for idx, asset in enumerate(self.asset_declarations):
            tmp = ", ".join(asset)
            code.append(f"DECLARE_BUILTIN_ASSET({idx}, {tmp});")
        code.append("")
        # Définition du tableau (pas de 'inline', une seule TU)
        code.append("void* __builtInAssetsRaw[] = {")
        for idx in range(len(self.asset_declarations)):
            code.append(f"    (void*)&__builtin_asset_{idx},")
        code.append("    nullptr,")
        code.append("};")
        code.append("")
        code.append("} // namespace Particule::Core")
        code.append("")

        # Code additionnel après
        if self.additional_code_after:
            code.append(self.additional_code_after)
            if not self.additional_code_after.endswith("\n"):
                code.append("")

        return "\n".join(code)

    # ---------- Sauvegarde ----------
    def save_code(self, build_dir: str):
        # Dossiers et chemins de sortie
        root = os.path.join(build_dir, "include", "Particule", "Core", "System")
        refs_dir = os.path.join(root, "References")
        resources_cpp_path = os.path.join(build_dir, "Resources.cpp")

        redefine_hpp_path = os.path.join(root, "Redefine.hpp")
        ref_input_hpp_path = os.path.join(refs_dir, "Input.hpp")
        ref_resource_hpp_path = os.path.join(refs_dir, "Resource.hpp")

        # Création des dossiers
        os.makedirs(root, exist_ok=True)
        os.makedirs(refs_dir, exist_ok=True)
        os.makedirs(os.path.dirname(resources_cpp_path), exist_ok=True)

        # Génération
        redefine_hpp = self._generate_redefine_hpp()
        ref_input_hpp = self._generate_reference_input_hpp()
        ref_resource_hpp = self._generate_reference_resource_hpp()
        resources_cpp = self._generate_resources_cpp()

        # Écriture conditionnelle
        if not CheckIfContentIsSame(redefine_hpp_path, redefine_hpp):
            with open(redefine_hpp_path, "w", encoding="utf-8") as f:
                f.write(redefine_hpp)

        if not CheckIfContentIsSame(ref_input_hpp_path, ref_input_hpp):
            with open(ref_input_hpp_path, "w", encoding="utf-8") as f:
                f.write(ref_input_hpp)

        if not CheckIfContentIsSame(ref_resource_hpp_path, ref_resource_hpp):
            with open(ref_resource_hpp_path, "w", encoding="utf-8") as f:
                f.write(ref_resource_hpp)

        if not CheckIfContentIsSame(resources_cpp_path, resources_cpp):
            with open(resources_cpp_path, "w", encoding="utf-8") as f:
                f.write(resources_cpp)
