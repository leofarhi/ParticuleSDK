import os
import importlib.util
import inspect
import shutil
from typing import Any, Dict, List, Type
from ..config.base import Config
from ..config.fields import *
from .custom_distribution_base import CustomDistribution
from .package_base import Package
from .builder_base import Builder
from ..system import working_dirs as wd


class Distribution(Config):
    """
    BaseDistribution est la configuration de base pour une distribution
    ou une librairie compilable.
    Elle étend BaseConfig et ajoute des champs standards pour un projet compilé.

    Exemple :
        distrib = BaseDistribution(distribution="Win", is_library=False)
        distrib.load(json_config)
        distrib.call_custom("generate_makefile")
    """

    def __init__(self, distribution: str, is_library: bool) -> None:
        super().__init__()
        self.__config_path__ = inspect.getfile(self.__class__)
        self.distribution = VarString(distribution, "Distribution name", common=False, private=True)
        self.is_library = VarBool(is_library, "Is this a library?", common=True, private=True)
        self.clean = VarBool(True, "Clean the build directory before building", common=True)
        self.debug = VarBool(False, "Enable debug mode", common=True)
        self.packages = VarList(
            VarString("", "Package name"), [],
            "Packages to include in the build", common=True
        )

        self.source_files = VarList(
            VarPath("", "(*.c, *.cpp, *.h)", filetypes=[("C/C++ Source Files", "*.c;*.cpp;*.h")]), [],
            "Source files to be compiled", common=True
        )
        self.include_paths = VarList(
            VarPath("", "Path of directory", is_dir=True), [],
            "Include paths for the compiler", common=True
        )
        self.library_paths = VarList(
            VarPath("", "Path of directory", is_dir=True), [],
            "Library paths for the linker", common=True
        )
        self.defines = VarFreeDict(
            VarString("", "MACRO"),
            VarString("", "Value"),
            {},
            "Preprocessor defines", common=True
        )

        self.output_file = VarString("output", "Output file name (without extension)", common=False)
        self.build_dir = VarPath("build", "Directory to store build files", common=True, is_dir=True)
        self.bin_dir = VarPath("bin", "Directory to store the final binary", common=True, is_dir=True)
        self.output_assets_dir = VarString("assets", "Output assets directory", common=False)
        self.project_name = VarString("MyApp", "Project name", common=True)
        self.custom_makeconfig = VarList(
            VarPath("", "(*.py)", filetypes=[("Python Files", "*.py")]), [],
            "Custom make configuration files", common=True
        )

        self.custom_config: List[Any] = []

    def load(self, config: Dict[str, Any]) -> None:
        """
        Charge la configuration depuis un dictionnaire.
        Puis charge dynamiquement les modules Python listés dans custom_makeconfig.
        """
        super().load(config)
        for custom_config_path in self.custom_makeconfig.to_data():
            if not os.path.exists(custom_config_path):
                print(f"[WARNING] Custom make config '{custom_config_path}' not found. Skipped.")
                continue

            module_name = os.path.splitext(os.path.basename(custom_config_path))[0]
            spec = importlib.util.spec_from_file_location(module_name, custom_config_path)

            if spec is None or spec.loader is None:
                print(f"[ERROR] Failed to load spec for '{custom_config_path}'.")
                continue

            module = importlib.util.module_from_spec(spec)
            try:
                spec.loader.exec_module(module)  # type: ignore
            except Exception as e:
                print(f"[ERROR] Failed to import '{custom_config_path}': {e}")
                continue

            # Trouve toutes les classes qui héritent de BaseCustomDistribution
            for _, obj in inspect.getmembers(module, inspect.isclass):
                if (
                    obj.__module__ == module.__name__
                    and issubclass(obj, CustomDistribution)
                ):
                    instance = obj()
                    if (
                        instance.distribution == self.distribution.value
                        and instance.is_library == self.is_library.value
                    ):
                        instance.set_builder(self, config)
                        self.custom_config.append(instance)

    def load_packages(self, builder) -> None:
        package_availables = []
        for package in os.listdir(wd.packages_path):
            if os.path.isdir(os.path.join(wd.packages_path, package)):
                if os.path.exists(os.path.join(wd.packages_path, package, "Package.py")):
                    package_availables.append(package)
        loaded_packages = []
        for package_name in self.packages.to_data():
            if package_name not in package_availables:
                print(f"[WARNING] Package '{package_name}' not found.")
                continue
            package_path = os.path.join(wd.packages_path, package_name, "Package.py")
            spec = importlib.util.spec_from_file_location(package_name, package_path)
            if spec is None or spec.loader is None:
                print(f"[ERROR] Failed to load spec for '{package_path}'.")
                continue

            module = importlib.util.module_from_spec(spec)
            try:
                spec.loader.exec_module(module)  # type: ignore
            except Exception as e:
                print(f"[ERROR] Failed to import '{package_path}': {e}")
                continue

            # Trouve toutes les classes qui héritent de Package
            for _, obj in inspect.getmembers(module, inspect.isclass):
                if (obj.__module__ == module.__name__ and issubclass(obj, Package)):
                    instance = obj(builder)
                    loaded_packages.append(instance)
        return loaded_packages

    def call_custom(self, method_name: str, list_instances: list) -> None:
        """
        Appelle une méthode donnée sur toutes les configs custom chargées.
        """
        for instance in list_instances:
            method = getattr(instance, method_name, None)
            if callable(method):
                method()
            else:
                print(
                    f"[WARNING] Méthode '{method_name}' non trouvée dans {type(instance).__name__}"
                )
    # # Méthodes à implémenter dans les sous-classes
    def validate(self) -> bool:
        """
        Vérifie la cohérence des données.
        """
        raise NotImplementedError

    def generate_builder(self, makefile_path: str) -> Builder:
        """
        Génère un constructeur de build spécifique à la distribution.
        """
        raise NotImplementedError
    
    def build(self, makefile_path: str) -> None:
        """
        Orchestration générique : appel les phases dans l'ordre.
        """
        if not self.validate():
            raise ValueError("Validation failed for the distribution configuration.")
        builder = self.generate_builder(makefile_path)
        if not isinstance(builder, Builder):
            raise TypeError("The builder must be an instance of BaseBuilder.")
        packages = self.load_packages(builder)
        for package in packages:
            builder.config_data["source_files"].extend(package.src_files)
            builder.config_data["include_paths"].extend(package.include_paths)
        list_instances = self.custom_config + packages
        self.call_custom("before_build", list_instances)
        builder.prepare_build()
        self.call_custom("prepare_build", list_instances)
        builder.prepare_assets()
        self.call_custom("prepare_assets", list_instances)
        builder.prepare_headers()
        self.call_custom("prepare_headers", list_instances)
        builder.prepare_makefile()
        self.call_custom("prepare_makefile", list_instances)
        builder.run_compilation()
        self.call_custom("after_compilation", list_instances)
        builder.post_build()
        self.call_custom("post_build", list_instances)
        return builder.success_check()
