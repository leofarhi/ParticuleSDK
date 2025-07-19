import customtkinter as ctk
import tkinter.filedialog as fd
import sys
import subprocess
import os
import threading
import shutil
import platform

os.chdir(os.path.dirname(__file__))

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

class InstallerApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Particule Engine SDK Installer")
        self.geometry("700x500")
        self.resizable(False, False)

        self.frames = []
        self.current_frame = 0
        self.venv_path = os.path.join(os.path.dirname(__file__), ".venv")
        self.python_path = None

        self.create_frames()
        self.show_frame(0)

    def create_frames(self):
        self.frames = [
            WelcomePage(self),
            PythonSelectionPage(self),
            VenvInstallPage(self),
            FinishPage(self),
            ErrorPage(self)
        ]

    def show_frame(self, index):
        self.current_frame = index
        for i, frame in enumerate(self.frames):
            frame.pack_forget()
            if i == index:
                frame.pack(fill="both", expand=True)

class WelcomePage(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master)
        ctk.CTkLabel(self, text="Bienvenue dans l'installateur du SDK Particule Engine", font=("Arial", 20)).pack(pady=40)
        ctk.CTkLabel(self, text="Cet installateur va vous guider dans l'installation du SDK.", wraplength=600).pack(pady=10)
        ctk.CTkButton(self, text="Suivant", command=self.next).pack(pady=30)

    def next(self):
        self.master.show_frame(1)

class PythonSelectionPage(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master)
        ctk.CTkLabel(self, text="Sélection de la version de Python", font=("Arial", 18)).pack(pady=20)

        self.recommended_label = ctk.CTkLabel(self, text="(Version 3.10 recommandée)")
        self.recommended_label.pack(pady=5)

        self.versions_listbox = ctk.CTkOptionMenu(self, values=self.find_pythons())
        self.versions_listbox.pack(pady=10)

        ctk.CTkLabel(self, text="Ou entrer un chemin personnalisé vers Python:").pack(pady=(20, 5))
        self.custom_path = ctk.CTkEntry(self, width=400)
        self.custom_path.pack(pady=5)

        ctk.CTkButton(self, text="Parcourir", command=self.browse).pack(pady=5)

        self.next_button = ctk.CTkButton(self, text="Suivant", command=self.validate_python)
        self.next_button.pack(pady=20)

    def browse(self):
        path = fd.askopenfilename(title="Sélectionner l'exécutable Python")
        if path:
            self.custom_path.delete(0, ctk.END)
            self.custom_path.insert(0, path)

    def find_pythons(self):
        found = []
        paths = os.environ.get("PATH", "").split(os.pathsep)
        for p in paths:
            try:
                for exe in os.listdir(p):
                    if exe.startswith("python") and (exe.endswith(".exe") or not platform.system() == "Windows"):
                        full = os.path.join(p, exe)
                        try:
                            version = subprocess.check_output([full, "--version"], stderr=subprocess.STDOUT).decode().strip()
                            found.append(full)
                        except:
                            pass
            except:
                continue
        return list(dict.fromkeys(found))

    def validate_python(self):
        path = self.custom_path.get() or self.versions_listbox.get()
        if not path:
            return
        try:
            output = subprocess.check_output([path, "--version"], stderr=subprocess.STDOUT).decode()
            if output.lower().startswith("python"):
                self.master.python_path = path
                self.master.show_frame(2)
                return
        except Exception as e:
            pass
        self.master.show_frame(4)

class VenvInstallPage(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master)
        self.progress = ctk.CTkProgressBar(self, width=500)
        self.progress.pack(pady=30)
        self.progress.set(0)

        self.log_box = ctk.CTkTextbox(self, width=600, height=250)
        self.log_box.pack(pady=10)

        self.next_button = ctk.CTkButton(self, text="Suivant", command=self.finish, state="disabled")
        self.next_button.pack(pady=20)

        self.started = False

    def log(self, msg):
        self.log_box.insert(ctk.END, msg + "\n")
        self.log_box.see(ctk.END)

    def start_installation(self):
        if self.started:
            return
        self.started = True
        threading.Thread(target=self.install_venv, daemon=True).start()

    def install_venv(self):
        try:
            is_windows = platform.system() == "Windows"
            pip = os.path.join(self.master.venv_path, "Scripts" if is_windows else "bin", "pip")
            python = os.path.join(self.master.venv_path, "Scripts" if is_windows else "bin", "python.exe" if is_windows else "python")

            steps = [
                ("Création de l'environnement virtuel...", lambda: subprocess.check_call([self.master.python_path, "-m", "venv", self.master.venv_path])),
                ("Mise à jour de pip...", lambda: subprocess.check_call([python, "-m", "pip", "install", "--upgrade", "pip"])),
                ("Installation de pillow...", lambda: subprocess.check_call([pip, "install", "pillow"])),
                ("Installation de customtkinter...", lambda: subprocess.check_call([pip, "install", "customtkinter"])),
                ("Installation de flask...", lambda: subprocess.check_call([pip, "install", "flask"])),
                ("Installation de dataclasses...", lambda: subprocess.check_call([pip, "install", "dataclasses"])),
                ("Ajout du chemin au PATH système...", lambda: self.add_to_path())
            ]
            if is_windows:
                steps.insert(2, ("Installation de tkinter...", lambda: subprocess.check_call([pip, "install", "tk"])))
            else:
                steps.insert(2, ("Installation de python-tk...", lambda: subprocess.check_call([pip, "install", "python3-tk"])))

            total = len(steps)
            for i, (message, func) in enumerate(steps):
                self.log(message)
                func()
                self.progress.set((i + 1) / total)

            self.log("Installation terminée avec succès.")
            self.next_button.configure(state="normal")
        except Exception as e:
            self.log("Erreur : " + str(e))
            self.master.show_frame(4)


    def add_to_path(self):
        path_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "Aliases"))
        if platform.system() == "Windows":
            import winreg
            key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Environment", 0, winreg.KEY_ALL_ACCESS)
            current_path, _ = winreg.QueryValueEx(key, "PATH")
            if path_dir not in current_path:
                new_path = current_path + ";" + path_dir
                winreg.SetValueEx(key, "PATH", 0, winreg.REG_EXPAND_SZ, new_path)
                self.log("Ajout au PATH (redémarrage requis pour prendre effet).")
            winreg.CloseKey(key)
        else:
            alias_command = f'alias ParticuleCraft="{path_dir}/ParticuleCraft.sh"'
            if platform.system() == "Darwin":
                os.system(f'echo "{alias_command}" >> ~/.zprofile')
            else:
                os.system(f'echo "{alias_command}" >> ~/.bashrc')

    def finish(self):
        self.master.show_frame(3)

    def pack(self, *args, **kwargs):
        super().pack(*args, **kwargs)
        self.after(100, self.start_installation)

class FinishPage(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master)
        ctk.CTkLabel(self, text="Installation terminée !", font=("Arial", 20)).pack(pady=40)
        ctk.CTkLabel(self, text="Le SDK Particule Engine est maintenant prêt à être utilisé.").pack(pady=10)
        ctk.CTkButton(self, text="Fermer", command=self.quit).pack(pady=30)

    def quit(self):
        self.master.destroy()

class ErrorPage(ctk.CTkFrame):
    def __init__(self, master):
        super().__init__(master)
        ctk.CTkLabel(self, text="Une erreur est survenue", font=("Arial", 20), text_color="red").pack(pady=40)
        ctk.CTkLabel(self, text="L'installation a échoué. Veuillez réessayer ou vérifier les chemins.").pack(pady=10)
        ctk.CTkButton(self, text="Revenir", command=self.retry).pack(pady=20)

    def retry(self):
        self.master.show_frame(1)

if __name__ == "__main__":
    app = InstallerApp()
    app.mainloop()