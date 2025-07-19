import sys, os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..","..")))
from ParticuleCraft.config.fields import Var, VarBool, VarInt, VarFloat, VarString, VarPath, VarEnum, VarList, VarDict, VarFreeDict

import customtkinter as ctk
from typing import Any, Dict
from functools import partial
from enum import Enum
import tkinter.filedialog as fd

class VarVisibility(Enum):
    SHOW_COMMON = "common"
    SHOW_INDIVIDUAL = "individual"
    SHOW_BOTH = "both"

def navtabs_from_distributions(distributions: list[str], master: ctk.CTk) -> Dict[str, ctk.CTkFrame]:
    root_tabview = ctk.CTkTabview(master)
    root_tabview.pack(fill="both", expand=True, padx=10, pady=10)

    result_frames: Dict[str, ctk.CTkFrame] = {}

    # On construit un arbre hiérarchique depuis la liste
    hierarchy: dict = {}
    for path in distributions:
        parts = path.split(".")
        node = hierarchy
        for part in parts:
            node = node.setdefault(part, {})

    # Fonction récursive pour créer les onglets et sous-onglets
    def build_tabs(node: dict, tabview: ctk.CTkTabview, prefix: str = ""):
        for name, children in node.items():
            full_path = f"{prefix}.{name}" if prefix else name
            frame = tabview.add(name)
            if children:
                # Si enfants : créer un sous-tabview
                sub_tabview = ctk.CTkTabview(tabview.tab(name))
                sub_tabview.pack(fill="both", expand=True, padx=1, pady=1)
                build_tabs(children, sub_tabview, full_path)
            else:
                # Sinon : ajouter la frame au résultat
                result_frames[full_path] = tabview.tab(name)

    build_tabs(hierarchy, root_tabview)
    return result_frames

def makefile_to_frame(makefile: Any, parent: ctk.CTk, visibility: VarVisibility, makefile_path:str = "") -> ctk.CTkFrame:
    frame = ctk.CTkFrame(parent, fg_color="transparent")
    frame.grid_columnconfigure(1, weight=1)

    row = 0
    for attr_name, var in vars(makefile).items():  # conserve l'ordre d'insertion
        if not isinstance(var, Var):
            continue
        if var.private:
            continue
        if visibility == VarVisibility.SHOW_COMMON and not var.common:
            continue
        if visibility == VarVisibility.SHOW_INDIVIDUAL and var.common:
            continue
        row = render_var_widget(attr_name, var, frame, row, makefile_path)

    return frame

def render_var_widget(name: str, var: Var, master: ctk.CTkFrame, row: int, makefile_path:str = "") -> int:
    def create_label(master, text):
        return ctk.CTkLabel(master, text=text)

    def on_var_change(var: Var, tk_var: Any, *_):
        val = tk_var.get()
        try:
            var.load(val)
        except Exception:
            pass

    def restore_default_if_empty(var, tk_var, event=None):
        val = tk_var.get().strip()
        if val == "":
            tk_var.set(str(var.default_value))
            var.load(var.default_value)

    label_text = f"[{name}] {var.description}" if var.description else f"[{name}]"

    if isinstance(var, VarBool):
        tk_var = ctk.BooleanVar(value=var.value)
        create_label(master, label_text).grid(row=row, column=0, sticky="w", padx=5, pady=5)
        checkbox = ctk.CTkCheckBox(master, text="", variable=tk_var,
                                   command=partial(on_var_change, var, tk_var))
        checkbox.grid(row=row, column=1, sticky="ew", padx=5, pady=5)

    elif isinstance(var, VarInt):
        tk_var = ctk.StringVar(value=str(var.value))
        create_label(master, label_text).grid(row=row, column=0, sticky="w", padx=5, pady=5)
        entry = ctk.CTkEntry(master, textvariable=tk_var)
        entry.grid(row=row, column=1, sticky="ew", padx=5, pady=5)
        tk_var.trace_add("write", partial(on_var_change, var, tk_var))
        entry.bind("<FocusOut>", partial(restore_default_if_empty, var, tk_var))

    elif isinstance(var, VarFloat):
        tk_var = ctk.StringVar(value=str(var.value))
        create_label(master, label_text).grid(row=row, column=0, sticky="w", padx=5, pady=5)
        entry = ctk.CTkEntry(master, textvariable=tk_var)
        entry.grid(row=row, column=1, sticky="ew", padx=5, pady=5)
        tk_var.trace_add("write", partial(on_var_change, var, tk_var))
        entry.bind("<FocusOut>", partial(restore_default_if_empty, var, tk_var))

    elif isinstance(var, (VarString, VarPath)):
        tk_var = ctk.StringVar(value=var.value)
        create_label(master, label_text).grid(row=row, column=0, sticky="w", padx=5, pady=5)

        entry = ctk.CTkEntry(master, textvariable=tk_var)
        entry.grid(row=row, column=1, sticky="ew", padx=5, pady=5)

        tk_var.trace_add("write", partial(on_var_change, var, tk_var))

        if isinstance(var, VarPath):
            def browse():
                path = ""
                if var.is_dir:
                    path = fd.askdirectory()
                else:
                    filetypes = var.filetypes
                    path = fd.askopenfilename(filetypes=[(f, f) for f in filetypes])
                if path:
                    #relatif au makefile
                    if makefile_path:
                        path = os.path.relpath(path, os.path.dirname(makefile_path)).replace("\\","/")
                    tk_var.set(path)
            browse_btn = ctk.CTkButton(master, text="📂", width=32, command=browse)
            browse_btn.grid(row=row, column=2, padx=5, pady=5)


    elif isinstance(var, VarEnum):
        tk_var = ctk.StringVar(value=var.value)
        create_label(master, label_text).grid(row=row, column=0, sticky="w", padx=5, pady=5)
        option = ctk.CTkOptionMenu(master, variable=tk_var, values=var.enum_values,
                                   command=partial(var.load))
        option.grid(row=row, column=1, sticky="ew", padx=5, pady=5)

    elif isinstance(var, VarDict):
        create_label(master, label_text).grid(row=row, column=0, columnspan=2, sticky="we", padx=5, pady=(6, 0))
        subframe = ctk.CTkFrame(master,fg_color="transparent",border_width=1,corner_radius=6,border_color="#666")
        subframe.grid(row=row + 1, column=0, columnspan=2, sticky="ew", padx=5, pady=5)
        subframe.grid_columnconfigure(1, weight=1)
        sub_row = 0
        for sub_name, sub_var in var.value.items():
            sub_row = render_var_widget(sub_name, sub_var, subframe, sub_row, makefile_path)
        row += 1

    elif isinstance(var, VarList):
        container = ctk.CTkFrame(master)
        container.grid(row=row + 1, column=0, columnspan=2, sticky="nsew", padx=5, pady=5)
        container.grid_columnconfigure(0, weight=1)

        scroll_frame = ctk.CTkScrollableFrame(container, label_text=label_text)
        scroll_frame.grid(row=0, column=0, sticky="nsew")
        scroll_frame.grid_columnconfigure(0, weight=1)

        def refresh_list(var):
            for widget in scroll_frame.winfo_children():
                widget.destroy()

            for i, item in enumerate(var.value):
                item_frame = ctk.CTkFrame(scroll_frame)
                item_frame.grid(row=i, column=0, sticky="ew", pady=4, padx=2)
                item_frame.grid_columnconfigure(0, weight=1)
                item_frame.grid_rowconfigure(0, weight=1)

                # Contenu à gauche
                content_frame = ctk.CTkFrame(item_frame, fg_color="transparent")
                content_frame.grid(row=0, column=0, sticky="nsew")
                content_frame.grid_columnconfigure(1, weight=1)

                render_var_widget(f"{name}_{i}", item, content_frame, 0, makefile_path)

                # Bouton centré verticalement à droite
                remove_btn = ctk.CTkButton(item_frame, text="-", width=30,
                                        command=partial(remove_item, i, var))
                remove_btn.grid(row=0, column=1, padx=5, pady=5, sticky="ns")

        def add_item(var):
            new = var.type_instance.clone()
            new.load(new.default_value)
            var.value.append(new)
            refresh_list(var)

        def remove_item(i, var):
            var.value.pop(i)
            refresh_list(var)

        add_btn = ctk.CTkButton(container, text="+ Ajouter", command=partial(add_item, var))
        add_btn.grid(row=1, column=0, sticky="ew", pady=5)

        refresh_list(var)
        row += 1

    elif isinstance(var, VarFreeDict):
        container = ctk.CTkFrame(master)
        container.grid(row=row, column=0, columnspan=2, sticky="nsew", padx=5, pady=5)
        container.grid_columnconfigure(0, weight=1)

        scroll_frame = ctk.CTkScrollableFrame(container, label_text=label_text)
        scroll_frame.grid(row=0, column=0, sticky="nsew")
        scroll_frame.grid_columnconfigure(0, weight=1)

        def refresh_dict(var):
            for widget in scroll_frame.winfo_children():
                widget.destroy()

            for i, (k, v) in enumerate(list(var.value.items())):
                row_frame = ctk.CTkFrame(scroll_frame)
                row_frame.grid(row=i, column=0, sticky="ew", pady=4, padx=2)
                row_frame.grid_columnconfigure(0, weight=1)
                row_frame.grid_columnconfigure(1, weight=0)

                # Sous-frame pour la clé + valeur
                pair_frame = ctk.CTkFrame(row_frame, fg_color="transparent")
                pair_frame.grid(row=0, column=0, sticky="ew")
                pair_frame.grid_columnconfigure(0, weight=1)
                pair_frame.grid_columnconfigure(1, weight=1)

                render_var_widget(f"key_{i}", k, pair_frame, 0, makefile_path)
                render_var_widget(f"val_{i}", v, pair_frame, 1, makefile_path)

                # Bouton "-" à droite, centré verticalement
                rm_btn = ctk.CTkButton(row_frame, text="-", width=30, command=partial(remove_entry, k, var))
                rm_btn.grid(row=0, column=1, padx=5, pady=5, sticky="ns")

        def add_entry(var):
            key_inst = var.key_type.clone()
            val_inst = var.value_type.clone()
            key_inst.load(key_inst.default_value)
            val_inst.load(val_inst.default_value)
            var.value[key_inst] = val_inst
            refresh_dict(var)

        def remove_entry(key, var):
            if key in var.value:
                del var.value[key]
                refresh_dict(var)

        add_btn = ctk.CTkButton(container, text="Ajouter une entrée", command=partial(add_entry, var))
        add_btn.grid(row=1, column=0, sticky="ew", pady=5)

        refresh_dict(var)

    else:
        create_label(master, f"{label_text} (non géré)").grid(row=row, column=0, columnspan=2, sticky="w", padx=5, pady=5)

    return row + 1



if __name__ == "__main__":
    class Makefile:
        def __init__(self):
            self.debug = VarBool(default_value=True, description="Mode debug")
            self.threads = VarInt(default_value=4, description="Nombre de threads")
            self.pi = VarFloat(default_value=3.14, description="Constante PI")
            self.name = VarString(default_value="ProjectX", description="Nom du projet")
            self.level = VarEnum(enum_values=["info", "warning", "error"], default_value="info", description="Niveau de log")
            self.texture = VarList(VarPath("", "(*.png, *.jpg)"), [], "Texture files")
            self.test = VarDict({
                        "path": VarPath("", "(*.png, *.jpg)"),
                        "reference_path": VarString("", "Reference path"),
                    })
            self.inputs = VarFreeDict(
                    VarString("<key>", "Key name"),
                    VarEnum(["UP", "DOWN", "LEFT", "RIGHT"], 'UP', "Input key"),
                    {},
                    "Input keys to be mapped"
                )
            self.assets_files = VarDict({
            "textures": VarList(VarDict({
                "path": VarPath("","(*.png, *.jpg)"),
                "reference_path": VarString("","(*.*)"),
                "format": VarEnum(["rgb565", "p8", "p4"], "rgb565", "Texture format"),
                "alpha": VarBool(False, "Enable alpha channel"),
                "external": VarBool(False, "Use external texture"),
                "external_path": VarString("","(*.img)"),
            }),[], "Texture files"),
            "fonts": VarList(VarDict({
                "path": VarPath("","(*.ttf, *.otf)"),
                "reference_path": VarString("","(*.font)"),
                "resolution": VarInt(8, "Resolution of the font"),
                "charset":  VarString("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz","Charset of the font"),
            }),[], "Font files"),
            "other": VarList(VarDict({
                "path": VarPath("","(*.*)"),
                "reference_path": VarString("","(*.*)"),
            }),[], "Other files")
        }, None, "Assets files to be included in the build")
    ctk.set_appearance_mode("System")
    ctk.set_default_color_theme("blue")

    root = ctk.CTk()
    root.geometry("800x700")
    root.title("Makefile GUI")

    mf = Makefile()
    scrollframe = ctk.CTkScrollableFrame(root, label_text="Makefile Configuration")
    scrollframe.pack(fill="both", expand=True)
    frame = makefile_to_frame(mf, scrollframe, visibility=VarVisibility.SHOW_BOTH)
    frame.pack(padx=20, pady=20, fill="both", expand=True)

    root.mainloop()
