from ParticuleCraft.core.distribution_base import *
from ParticuleCraft.config.fields import *
from ParticuleCraft.utils.multi_platform import *
from .Builders.SDL2.BuilderSDL2 import BuilderSDL2, Builder

class MakeAppConfig(Distribution):
    """
    MakeAppConfig est une configuration spécifique
    pour construire une application Linux avec des assets,
    des entrées clavier et des options de build personnalisées.

    Exemple :
        app_config = MakeAppConfig()
        app_config.load(json_config)
    """
    def __init__(self) -> None:
        super().__init__("Linux", is_library=False)
        Keys = ['SDLK_UNKNOWN', 'SDLK_RETURN', 'SDLK_ESCAPE', 'SDLK_BACKSPACE', 'SDLK_TAB', 'SDLK_SPACE', 'SDLK_EXCLAIM', 'SDLK_QUOTEDBL', 'SDLK_HASH', 'SDLK_PERCENT', 'SDLK_DOLLAR', 'SDLK_AMPERSAND', 'SDLK_QUOTE', 'SDLK_LEFTPAREN', 'SDLK_RIGHTPAREN', 'SDLK_ASTERISK', 'SDLK_PLUS', 'SDLK_COMMA', 'SDLK_MINUS', 'SDLK_PERIOD', 'SDLK_SLASH', 'SDLK_0', 'SDLK_1', 'SDLK_2', 'SDLK_3', 'SDLK_4', 'SDLK_5', 'SDLK_6', 'SDLK_7', 'SDLK_8', 'SDLK_9', 'SDLK_COLON', 'SDLK_SEMICOLON', 'SDLK_LESS', 'SDLK_EQUALS', 'SDLK_GREATER', 'SDLK_QUESTION', 'SDLK_AT', 'SDLK_LEFTBRACKET', 'SDLK_BACKSLASH', 'SDLK_RIGHTBRACKET', 'SDLK_CARET', 'SDLK_UNDERSCORE', 'SDLK_BACKQUOTE', 'SDLK_a', 'SDLK_b', 'SDLK_c', 'SDLK_d', 'SDLK_e', 'SDLK_f', 'SDLK_g', 'SDLK_h', 'SDLK_i', 'SDLK_j', 'SDLK_k', 'SDLK_l', 'SDLK_m', 'SDLK_n', 'SDLK_o', 'SDLK_p', 'SDLK_q', 'SDLK_r', 'SDLK_s', 'SDLK_t', 'SDLK_u', 'SDLK_v', 'SDLK_w', 'SDLK_x', 'SDLK_y', 'SDLK_z', 'SDLK_CAPSLOCK', 'SDLK_F1', 'SDLK_F2', 'SDLK_F3', 'SDLK_F4', 'SDLK_F5', 'SDLK_F6', 'SDLK_F7', 'SDLK_F8', 'SDLK_F9', 'SDLK_F10', 'SDLK_F11', 'SDLK_F12', 'SDLK_PRINTSCREEN', 'SDLK_SCROLLLOCK', 'SDLK_PAUSE', 'SDLK_INSERT', 'SDLK_HOME', 'SDLK_PAGEUP', 'SDLK_DELETE', 'SDLK_END', 'SDLK_PAGEDOWN', 'SDLK_RIGHT', 'SDLK_LEFT', 'SDLK_DOWN', 'SDLK_UP', 'SDLK_NUMLOCKCLEAR', 'SDLK_KP_DIVIDE', 'SDLK_KP_MULTIPLY', 'SDLK_KP_MINUS', 'SDLK_KP_PLUS', 'SDLK_KP_ENTER', 'SDLK_KP_1', 'SDLK_KP_2', 'SDLK_KP_3', 'SDLK_KP_4', 'SDLK_KP_5', 'SDLK_KP_6', 'SDLK_KP_7', 'SDLK_KP_8', 'SDLK_KP_9', 'SDLK_KP_0', 'SDLK_KP_PERIOD', 'SDLK_APPLICATION', 'SDLK_POWER', 'SDLK_KP_EQUALS', 'SDLK_F13', 'SDLK_F14', 'SDLK_F15', 'SDLK_F16', 'SDLK_F17', 'SDLK_F18', 'SDLK_F19', 'SDLK_F20', 'SDLK_F21', 'SDLK_F22', 'SDLK_F23', 'SDLK_F24', 'SDLK_EXECUTE', 'SDLK_HELP', 'SDLK_MENU', 'SDLK_SELECT', 'SDLK_STOP', 'SDLK_AGAIN', 'SDLK_UNDO', 'SDLK_CUT', 'SDLK_COPY', 'SDLK_PASTE', 'SDLK_FIND', 'SDLK_MUTE', 'SDLK_VOLUMEUP', 'SDLK_VOLUMEDOWN', 'SDLK_KP_COMMA', 'SDLK_KP_EQUALSAS400', 'SDLK_ALTERASE', 'SDLK_SYSREQ', 'SDLK_CANCEL', 'SDLK_CLEAR', 'SDLK_PRIOR', 'SDLK_RETURN2', 'SDLK_SEPARATOR', 'SDLK_OUT', 'SDLK_OPER', 'SDLK_CLEARAGAIN', 'SDLK_CRSEL', 'SDLK_EXSEL', 'SDLK_KP_00', 'SDLK_KP_000', 'SDLK_THOUSANDSSEPARATOR', 'SDLK_DECIMALSEPARATOR', 'SDLK_CURRENCYUNIT', 'SDLK_CURRENCYSUBUNIT', 'SDLK_KP_LEFTPAREN', 'SDLK_KP_RIGHTPAREN', 'SDLK_KP_LEFTBRACE', 'SDLK_KP_RIGHTBRACE', 'SDLK_KP_TAB', 'SDLK_KP_BACKSPACE', 'SDLK_KP_A', 'SDLK_KP_B', 'SDLK_KP_C', 'SDLK_KP_D', 'SDLK_KP_E', 'SDLK_KP_F', 'SDLK_KP_XOR', 'SDLK_KP_POWER', 'SDLK_KP_PERCENT', 'SDLK_KP_LESS', 'SDLK_KP_GREATER', 'SDLK_KP_AMPERSAND', 'SDLK_KP_DBLAMPERSAND', 'SDLK_KP_VERTICALBAR', 'SDLK_KP_DBLVERTICALBAR', 'SDLK_KP_COLON', 'SDLK_KP_HASH', 'SDLK_KP_SPACE', 'SDLK_KP_AT', 'SDLK_KP_EXCLAM', 'SDLK_KP_MEMSTORE', 'SDLK_KP_MEMRECALL', 'SDLK_KP_MEMCLEAR', 'SDLK_KP_MEMADD', 'SDLK_KP_MEMSUBTRACT', 'SDLK_KP_MEMMULTIPLY', 'SDLK_KP_MEMDIVIDE', 'SDLK_KP_PLUSMINUS', 'SDLK_KP_CLEAR', 'SDLK_KP_CLEARENTRY', 'SDLK_KP_BINARY', 'SDLK_KP_OCTAL', 'SDLK_KP_DECIMAL', 'SDLK_KP_HEXADECIMAL', 'SDLK_LCTRL', 'SDLK_LSHIFT', 'SDLK_LALT', 'SDLK_LGUI', 'SDLK_RCTRL', 'SDLK_RSHIFT', 'SDLK_RALT', 'SDLK_RGUI', 'SDLK_MODE', 'SDLK_AUDIONEXT', 'SDLK_AUDIOPREV', 'SDLK_AUDIOSTOP', 'SDLK_AUDIOPLAY', 'SDLK_AUDIOMUTE', 'SDLK_MEDIASELECT', 'SDLK_WWW', 'SDLK_MAIL', 'SDLK_CALCULATOR', 'SDLK_COMPUTER', 'SDLK_AC_SEARCH', 'SDLK_AC_HOME', 'SDLK_AC_BACK', 'SDLK_AC_FORWARD', 'SDLK_AC_STOP', 'SDLK_AC_REFRESH', 'SDLK_AC_BOOKMARKS', 'SDLK_BRIGHTNESSDOWN', 'SDLK_BRIGHTNESSUP', 'SDLK_DISPLAYSWITCH', 'SDLK_KBDILLUMTOGGLE', 'SDLK_KBDILLUMDOWN', 'SDLK_KBDILLUMUP', 'SDLK_EJECT', 'SDLK_SLEEP', 'SDLK_APP1', 'SDLK_APP2', 'SDLK_AUDIOREWIND', 'SDLK_AUDIOFASTFORWARD']

        self.assets_files = VarDict({
            "textures": VarList(VarDict({
                "path": VarPath("", "(*.png, *.jpg)", filetypes=[("Image Files", "*.png;*.jpg")]),
                "reference_path": VarString("", "Reference path"),
            }), [], "Texture files"),

            "fonts": VarList(VarDict({
                "path": VarPath("", "(*.ttf, *.otf)", filetypes=[("Font Files", "*.ttf;*.otf")]),
                "reference_path": VarString("", "Reference path"),
                "resolution": VarInt(8, "Resolution of the font"),
                "charset": VarString(
                    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
                    "Charset of the font"
                ),
            }), [], "Font files"),

            "audio": VarList(VarDict({
                "path": VarPath("", "(*.wav, *.mp3)", filetypes=[("Audio Files", "*.wav;*.mp3")]),
                "reference_path": VarString("", "Reference path"),
            }), [], "Audio files"),

            "other": VarList(VarDict({
                "path": VarPath("", "(*.*)", filetypes=[("All Files", "*.*")]),
                "reference_path": VarString("", "Reference path"),
            }), [], "Other files"),
        }, None, "Assets files to be included in the build")

        self.inputs = VarFreeDict(
            VarString("", "Key name"),
            VarEnum(Keys, 'SDLK_UNKNOWN', "Input key"),
            {},
            "Input keys to be mapped"
        )

        self.icon = VarPath("icons.png", "Icon file for the application", filetypes=[("Image Files", "*.png")])
        self.console = VarBool(False, "Enable console window")
        #self.architecture = VarEnum(
        #    ["x86", "x64"], "x64", "Target architecture"
        #)
        self.lib_api = VarEnum(
            ["SDL2"], "SDL2", "Graphics API used for the application"
        )

    def validate(self) -> None:
        detect_wsl()
        if not ("wsl" in platform_available or "linux" in platform_available):
            raise RuntimeError("Must run under WSL or Linux.")
        if self.lib_api.to_data() != "SDL2":
            raise ValueError("Only SDL2 is supported for now.")
        return True

    def generate_builder(self, makefile_path: str) -> Builder:
        return BuilderSDL2(self, makefile_path)