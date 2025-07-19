
import os
from ParticuleCraft.utils import CheckIfContentIsSame
class RedefineManager:
    def __init__(self):
        self.includes: list[str] = [
            "<Particule/Core/Graphics/Image/Texture.hpp>",
            "<Particule/Core/Font/Font.hpp>",
            "<Particule/Core/System/Input.hpp>",
            "<Particule/Core/System/Basic.hpp>"
        ]
        self.asset_declarations: list[list[str]] = []
        self.input_mappings: list[tuple[str, str]] = []
        self.resource_mappings: list[tuple[str, int]] = []
        self.additional_code_before: str = ""
        self.additional_code_after: str = ""
        self.assets_path = "assets"


    def generate_code(self) -> str:
        code = "#ifndef REDEFINE_HPP\n#define REDEFINE_HPP\n"
        for include in self.includes:
            code += f"#include {include}\n"
        code += "\n"
        code += self.additional_code_before + "\n"
        code += "namespace Particule::Core::Resources {\n"
        for idx, asset in enumerate(self.asset_declarations):
            tmp = ", ".join(asset)
            code += f"DECLARE_BUILTIN_ASSET({idx}, {tmp});\n"
        code += "inline void* __builtInAssetsRaw[] = {\n"
        for idx in range(len(self.asset_declarations)):
            code += f"    (void*)&__builtin_asset_{idx},\n"
        code += f"    nullptr,\n"
        code += "};\n"
        code += "}\n"
        code += "namespace Particule::Core {\n"
        inputs = ""
        for key, value in self.input_mappings:
            inputs += f'\nCONST_STR_CMP(str, "{key}") ? Input({value}) : \\'
        code += f"#ifndef GetInput\n#define GetInput(str)(\\{inputs}\nInput())\n#endif\n"
        resources = ""
        for key, value in self.resource_mappings:
            resources += f'\nCONST_STR_CMP(str, "{key}") ? {value} : \\'
        code += f"#ifndef GetResourceID\n#define GetResourceID(str)(\\{resources}\n-1)\n#endif\n"
        code += "}\n"
        external_asset_count = len(self.resource_mappings) - len(self.asset_declarations)
        code += f"#define EXTERNAL_ASSET_COUNT {external_asset_count}\n"
        code += f"#define EXTERNAL_ASSET_PATH \"{self.assets_path}\"\n"
        code += self.additional_code_after + "\n"
        code += "#endif"
        return code

    def save_code(self, build_dir:str):
        code = self.generate_code()
        redefine_path = os.path.join(build_dir, "include", "Particule", "Core", "System", "Redefine.hpp")
        os.makedirs(os.path.dirname(redefine_path), exist_ok=True)
        if not CheckIfContentIsSame(redefine_path, code):
            with open(redefine_path, "w") as f:
                f.write(code)