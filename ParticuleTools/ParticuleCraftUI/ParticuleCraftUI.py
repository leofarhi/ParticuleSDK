from widgets import *
from ParticuleCraft.system.makefile_parser import MakefileParser
from ParticuleCraft.system.distribution_manager import load_distributions
import tkinter.filedialog as fd
import tkinter.messagebox as mbox
import json
load_distributions()


def choisir_fichier_json() -> str | None:
    filepath = fd.askopenfilename(
        title="Sélectionner un fichier Makefile JSON",
        filetypes=[("Fichiers JSON", "*.json")],
        defaultextension=".json"
    )
    return filepath if filepath else None

def save_makefile_path(default_name="Makefile.json") -> str:
    file_path = fd.asksaveasfilename(
        defaultextension=".json",
        filetypes=[("Fichier JSON", "*.json")],
        initialfile=default_name,
        title="Enregistrer le Makefile"
    )
    return file_path  # Retourne une chaîne vide "" si l'utilisateur annule


class HubChoice(Enum):
    NONE = 0
    IMPORT = 1
    APP = 2
    LIB = 3

makefile_p = None
path = None

ctk.set_appearance_mode("System")
ctk.set_default_color_theme("blue")

app = ctk.CTk()
app.geometry("800x600")
app.title("Makefile Editor")

def on_closing():
    if mbox.askokcancel("Quitter", "Les données non sauvegardées seront perdues.\nÊtes-vous sûr de vouloir quitter ?"):
        app.destroy()


hub_frame = ctk.CTkFrame(app)
hub_frame.pack(fill="both", expand=True)

def open_project(makefile_p: MakefileParser):
    global path
    app.protocol("WM_DELETE_WINDOW", on_closing)
    tab_frames = navtabs_from_distributions(["common"]+list(makefile_p.makefiles.keys()), app)
    for name, frame in tab_frames.items():
        scrollframe = ctk.CTkScrollableFrame(frame)
        scrollframe.pack(fill="both", expand=True)
        if name == "common":
            distribution = makefile_p.common
            mf_frame = makefile_to_frame(distribution, scrollframe, visibility=VarVisibility.SHOW_COMMON, makefile_path=path)
        else:
            distribution = makefile_p.makefiles[name]
            mf_frame = makefile_to_frame(distribution, scrollframe, visibility=VarVisibility.SHOW_INDIVIDUAL, makefile_path=path)
        mf_frame.pack(fill="both", expand=True)
    #add a button to save the makefile on the bottom of the app
    def save_makefile(makefile_p: MakefileParser):
        global path
        if not path:
            print("Aucun chemin de sauvegarde sélectionné.")
            return
        with open(path, 'w', encoding="utf-8") as f:
            json.dump(makefile_p.to_data(), f, indent=4, ensure_ascii=False)
        print(f"Makefile sauvegardé avec succès à {path}")
    save_button = ctk.CTkButton(app, text="Enregistrer le Makefile", command=lambda: save_makefile(makefile_p))
    save_button.pack(side="bottom", fill="x")

def select(choice: HubChoice):
    global makefile_p, path
    hub_frame.destroy()
    if choice is HubChoice.NONE:
        quit()
    if choice == HubChoice.IMPORT:
        path = choisir_fichier_json()
        if path:
            try:
                with open(path, 'r', encoding="utf-8") as f:
                    config = json.load(f)
                makefile_p = MakefileParser.load_data(config)
                print("Makefile chargé avec succès.")
            except Exception as e:
                print(f"Erreur lors du chargement du Makefile : {e}")
                quit()
        else:
            print("Aucun fichier sélectionné.")
            quit()
    else:
        path = save_makefile_path()
        if not path:
            print("Aucun chemin de sauvegarde sélectionné.")
            quit()
        makefile_p = MakefileParser(choice == HubChoice.LIB)
    if makefile_p is None:
        print("Erreur : Makefile non chargé.")
        quit()
    open_project(makefile_p)

ctk.CTkLabel(hub_frame, text="Bienvenue dans le Makefile Editor", font=("Arial", 20)).pack(pady=20)
ctk.CTkButton(hub_frame, text="Importer un Makefile", command=lambda: select(HubChoice.IMPORT)).pack(pady=10, fill="x", padx=60)
ctk.CTkButton(hub_frame, text="Créer une App", command=lambda: select(HubChoice.APP)).pack(pady=10, fill="x", padx=60)
ctk.CTkButton(hub_frame, text="Créer une Lib", command=lambda: select(HubChoice.LIB)).pack(pady=10, fill="x", padx=60)



app.mainloop()
