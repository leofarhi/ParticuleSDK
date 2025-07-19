from ParticuleCraft.core.distribution_base import *
from ParticuleCraft.config.fields import *

class MakeLibConfig(Distribution):
    """
    MakeLibConfig est une configuration spécifique pour les bibliothèques compilées.
    Elle hérite de BaseDistribution et ajoute des champs spécifiques aux bibliothèques.
    """

    def __init__(self) -> None:
        super().__init__("Windows", is_library=True)
        pass