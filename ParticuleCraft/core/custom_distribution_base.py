from ..config.base import Config

class CustomDistribution(Config):
    def __init__(self, distribution, is_library):
        super().__init__()
        self.distribution = distribution
        self.is_library = is_library
        self.builder = None

    def SetBuilder(self, builder, config):
        self.builder = builder
        super().Load(config)

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