from ParticuleCraft.core.distribution_base import *
from ParticuleCraft.config.fields import *
from ParticuleCraft.utils.multi_platform import *

class MakeLibConfig(Distribution):
    """
    MakeLibConfig est une configuration spécifique pour les bibliothèques compilées.
    Elle hérite de BaseDistribution et ajoute des champs spécifiques aux bibliothèques.
    """

    def __init__(self) -> None:
        super().__init__("Casio.CG", is_library=True)
        pass

    def validate(self) -> None:
        detect_wsl()
        if not ("wsl" in platform_available or "linux" in platform_available):
            raise RuntimeError("Must run under WSL or Linux.")