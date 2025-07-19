import copy
from typing import Dict, Any
from ..core.distribution_base import Distribution
from .distribution_manager import Distributions

class MakefileParser:
    """
    MakefileParser gère un ensemble de MakeConfigs (App ou Lib)
    pour toutes les distributions connues.
    """

    def __init__(self, is_library: bool) -> None:
        self.makefiles: Dict[str, Distribution] = {}
        self.is_library = is_library

        self.common = Distribution("common", is_library)

        for dist_name, classes in Distributions.items():
            make_cls = classes["MakeLibConfig"] if is_library else classes["MakeAppConfig"]
            self.makefiles[dist_name] = make_cls()

    def to_data(self) -> Dict[str, Any]:
        """
        Exporte toutes les données des configs,
        en séparant common et spécifiques.
        """
        outs = {}
        common_data = self.common.to_data(exclude_common=False).copy()
        common_data.pop("distribution", None)
        outs[self.common.distribution.to_data()] = common_data

        for dist_name, makefile in self.makefiles.items():
            dist_data = makefile.to_data(exclude_common=True).copy()
            dist_data.pop("distribution", None)
            outs[makefile.distribution.to_data()] = dist_data

        return outs

    def to_config(self) -> Dict[str, Any]:
        """
        Exporte la structure de config pour la sérialisation
        (utile pour générer un JSON/template).
        """
        outs = {}
        common_config = self.common.to_config(exclude_common=False).copy()
        common_config.pop("distribution", None)
        outs[self.common.distribution.to_data()] = common_config

        for dist_name, makefile in self.makefiles.items():
            dist_config = makefile.to_config(exclude_common=True).copy()
            dist_config.pop("distribution", None)
            outs[makefile.distribution.to_data()] = dist_config

        return outs

    @staticmethod
    def load_data(config: Dict[str, Any]) -> 'MakefileParser':
        """
        Recharge un MakefileParser depuis un JSON-like.
        Reconstitue `common` + chaque distribution à partir du template.
        """
        # Déclencher un deep copy sûr
        config_copy = copy.deepcopy(config)

        is_library = config_copy["common"]["is_library"]
        parser = MakefileParser(is_library)

        parser.common.load(config_copy["common"])
        common_base = parser.common.to_data(exclude_common=False)

        # Supprime la partie common pour traiter chaque distri
        config_copy.pop("common")

        for dist_name, dist_data in config_copy.items():
            if dist_name not in Distributions:
                raise ValueError(f"[MakefileParser] Unknown distribution '{dist_name}'")

            merged_data = copy.deepcopy(common_base)
            merged_data["distribution"] = dist_name
            merged_data.update(dist_data)

            make_cls = Distributions[dist_name]["MakeLibConfig"] if is_library else Distributions[dist_name]["MakeAppConfig"]
            dist_makefile = make_cls()
            dist_makefile.load(merged_data)

            parser.makefiles[dist_name] = dist_makefile

        return parser
