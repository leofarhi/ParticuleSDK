from ParticuleCraft.core.distribution_base import *
from ParticuleCraft.config.fields import *
from ParticuleCraft.utils.multi_platform import *
from .Builder.Gint.BuilderGint import BuilderGint


class MakeAppConfig(Distribution):
    """
    MakeAppConfig est une configuration spécifique
    pour construire une application Casio.CG90 avec des assets,
    des entrées clavier et des options de build personnalisées.

    Exemple :
        app_config = MakeAppConfig()
        app_config.load(json_config)
    """
    def __init__(self) -> None:
        super().__init__("Casio.CG90", is_library=False)
        self.Keys = ["KEY_F1","KEY_F2","KEY_F3","KEY_F4","KEY_F5","KEY_F6","KEY_SHIFT","KEY_OPTN","KEY_VARS","KEY_MENU","KEY_LEFT","KEY_UP","KEY_ALPHA","KEY_SQUARE","KEY_POWER","KEY_EXIT","KEY_DOWN","KEY_RIGHT","KEY_XOT","KEY_LOG","KEY_LN","KEY_SIN","KEY_COS","KEY_TAN","KEY_FRAC","KEY_FD","KEY_LEFTP","KEY_RIGHTP","KEY_COMMA","KEY_ARROW","KEY_7","KEY_8","KEY_9","KEY_DEL","KEY_4","KEY_5","KEY_6","KEY_MUL","KEY_DIV","KEY_1","KEY_2","KEY_3","KEY_ADD","KEY_SUB","KEY_0","KEY_DOT","KEY_EXP","KEY_NEG","KEY_EXE","KEY_ACON","KEY_HELP","KEY_LIGHT","KEY_KBD","KEY_X","KEY_Y","KEY_Z","KEY_EQUALS","KEY_CLEAR","KEY_X2","KEY_CARET","KEY_SWITCH","KEY_LEFTPAR","KEY_RIGHTPAR","KEY_STORE","KEY_TIMES","KEY_PLUS","KEY_MINUS"]

        self.libraries = VarList(VarPath("","*.a", filetypes=[("Library Files", "*.a")]),[], "Libraries to link against")
        self.compile_flags = VarString("","Additional flags for the compiler")
        self.link_flags = VarString("","Additional flags for the linker")
        self.assets_files = VarDict({
            "textures": VarList(VarDict({
                "path": VarPath("","(*.png, *.jpg)", filetypes=[("Image Files", "*.png;*.jpg")]),
                "reference_path": VarString("","(*.*)"),
                "format": VarEnum(["rgb565", "p8", "p4"], "rgb565", "Texture format"),
                "alpha": VarBool(False, "Enable alpha channel"),
                "external": VarBool(False, "Use external texture"),
                "include_sprites": VarBool(False, "Include sprite sheets"),
            }),[], "Texture files"),
            "fonts": VarList(VarDict({
                "path": VarPath("","(*.ttf, *.otf)", filetypes=[("Font Files", "*.ttf;*.otf")]),
                "reference_path": VarString("","(*.font)"),
                "resolution": VarInt(8, "Resolution of the font"),
                "charset":  VarString("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz","Charset of the font"),
            }),[], "Font files"),
            "other": VarList(VarDict({
                "path": VarPath("","(*.*)", filetypes=[("All Files", "*.*")]),
                "reference_path": VarString("","(*.*)"),
            }),[], "Other files")
        }, None, "Assets files to be included in the build")
        self.inputs = VarFreeDict(VarString("","key name"),VarEnum(self.Keys, "KEY_EXE", "input key"),{}, "Input files to be included in the build")
        self.display_name = VarString("MyApp", "Display name of the application (8 characters max)")
        self.icon_uns = VarPath("icon-uns.png", "Icon file for unselected application", filetypes=[("Image Files", "*.png")])
        self.icon_sel = VarPath("icon-sel.png", "Icon file for selected application", filetypes=[("Image Files", "*.png")])
        self.memtrack = VarBool(False, "Enable memory leak tracking")

    def validate(self) -> None:
        detect_wsl()
        if not ("wsl" in platform_available or "linux" in platform_available):
            raise RuntimeError("Must run under WSL or Linux.")
        return True

    def generate_builder(self, makefile_path: str) -> Builder:
        return BuilderGint(self, makefile_path)
