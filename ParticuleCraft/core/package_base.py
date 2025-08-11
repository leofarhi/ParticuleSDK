from .builder_base import Builder
import inspect

class Package:
    """
    Package est une classe de  pour les configurations de paquets.
    """
    def __init__(self, name: str, builder: Builder) -> None:
        self.name = name
        self.builder = builder
        self.src_files = []
        self.include_paths = []

    def before_build(self):
        pass

    def prepare_build(self):
        pass

    def prepare_assets(self):
        pass

    def prepare_headers(self):
        pass

    def prepare_makefile(self):
        pass

    def after_compilation(self):
        pass

    def post_build(self):
        pass