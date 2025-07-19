import os
import shutil

class Builder:
    def __init__(self, config, makefile_path) -> None:
        self.config = config
        self.config_data = config.to_data()
        self.cwd = os.getcwd()
        self.makefile_path = makefile_path
        self.project_path = os.path.abspath(os.path.dirname(makefile_path))
        os.chdir(self.project_path)
        self.build_dir = os.path.join(self.project_path, self.config_data["build_dir"])
        self.bin_dir = os.path.join(self.project_path, self.config_data["bin_dir"])

    def prepare_build(self) -> None:
        """
        Prépare le répertoire de build et les configurations nécessaires.
        """
        if self.config_data["clean"] and os.path.exists(self.build_dir):
            shutil.rmtree(self.build_dir)
        os.makedirs(self.build_dir, exist_ok=True)
        os.makedirs(self.bin_dir, exist_ok=True)

    def prepare_assets(self) -> None:
        """
        Génère les assets nécessaires.
        """
        raise NotImplementedError

    def prepare_headers(self) -> None:
        """
        Génère les fichiers d'en-tête ou métadonnées.
        """
        raise NotImplementedError

    def prepare_makefile(self) -> None:
        """
        Génère le contenu du fichier CMakeLists.txt ou Makefile.
        Renvoie le contenu à écrire.
        """
        raise NotImplementedError

    def run_compilation(self) -> None:
        """
        Exécute la compilation effective.
        """
        raise NotImplementedError

    def post_build(self) -> None:
        """
        Étapes finales : copier binaires, assets externes...
        """
        raise NotImplementedError

    def success_check(self) -> bool:
        """
        Vérifie si la compilation a réussi.
        """
        raise NotImplementedError
