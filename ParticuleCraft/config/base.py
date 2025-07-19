import json
from typing import Any, Dict
from .fields import Var

class Config:
    """
    BaseConfig is a base class for any configuration object.
    It manages Var instances, loading data and generating JSON-friendly output.

    Example:
        class MyConfig(BaseConfig):
            debug = VarBool(default_value=True, description="Enable debug")
            threads = VarInt(default_value=4, description="Number of threads")
    """

    def load(self, data: Dict[str, Any]) -> None:
        """
        Load values into the config from a dictionary.
        Only keys matching Var attributes will be loaded.
        """
        for name, var in self.__dict__.items():
            if isinstance(var, Var) and name in data:
                var.load(data[name])

    def to_data(self, exclude_common: bool = False) -> Dict[str, Any]:
        """
        Get the config values as a plain dictionary.
        """
        result: Dict[str, Any] = {}
        for name, var in self.__dict__.items():
            if isinstance(var, Var):
                if not exclude_common or not var.common:
                    result[name] = var.to_data()
        return result

    def to_config(self, exclude_common: bool = False) -> Dict[str, Any]:
        """
        Get the config schema as a dictionary.
        """
        result: Dict[str, Any] = {}
        for name, var in self.__dict__.items():
            if isinstance(var, Var):
                if not exclude_common or not var.common:
                    result[name] = var.to_config()
        return result

    def print_data(self, exclude_common: bool = False) -> None:
        """
        Pretty-print the config data as JSON.
        """
        print(json.dumps(self.to_data(exclude_common=exclude_common), indent=4))

    def print_config(self, exclude_common: bool = False) -> None:
        """
        Pretty-print the config schema as JSON.
        """
        print(json.dumps(self.to_config(exclude_common=exclude_common), indent=4))

    def __repr__(self) -> str:
        """
        Representation shows the current data for debugging.
        """
        return f"{self.__class__.__name__}({self.to_data()})"
