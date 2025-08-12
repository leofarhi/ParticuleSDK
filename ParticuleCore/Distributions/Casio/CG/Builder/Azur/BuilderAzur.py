from ..Gint.BuilderGint import *
from .MakefileGeneratorAzur import MakefileGeneratorAzur


class BuilderAzur(BuilderGint):
    def prepare_makefile(self) -> None:
        generator = MakefileGeneratorAzur(self)
        generator.generate_makefile()