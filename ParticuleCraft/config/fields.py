import os
import copy
from typing import Any, Dict, List, Optional, Union


class Var:
    """
    Base class for all configurable variables.
    Example:
        debug = VarInt(default_value=1, description="Enable debug mode")
    """

    def __init__(
        self,
        var_type: str,
        default_value: Any = None,
        description: Optional[str] = None,
        common: bool = False,
        private: bool = False
    ) -> None:
        self.var_type: str = var_type
        self.default_value: Any = default_value
        self.description: Optional[str] = description
        self.common: bool = common
        self.private: bool = private
        self.load(default_value)

    def __deepcopy__(self, memo) -> "Var":
        cls = self.__class__
        result = cls.__new__(cls)
        memo[id(self)] = result
        for k, v in self.__dict__.items():
            setattr(result, k, copy.deepcopy(v, memo))
        return result

    def clone(self) -> "Var":
        return copy.deepcopy(self)

    def load(self, value: Any) -> None:
        raise NotImplementedError("load() must be implemented in subclasses.")

    def to_data(self) -> Any:
        return self.value

    def to_config(self) -> Dict[str, Any]:
        return {
            "type": self.var_type,
            "description": self.description,
        }


class VarInt(Var):
    """
    Integer configuration variable.
    Example:
        max_threads = VarInt(default_value=4, description="Number of threads")
    """

    def __init__(self, default_value: int = 0, *args, **kwargs) -> None:
        super().__init__("int", default_value, *args, **kwargs)

    def load(self, value: Any) -> None:
        try:
            self.value = int(value)
        except (ValueError, TypeError):
            raise ValueError(f"Invalid value for {self.description}. Expected an integer.")


class VarFloat(Var):
    """
    Float configuration variable.
    Example:
        learning_rate = VarFloat(default_value=0.01, description="Learning rate for optimizer")
    """

    def __init__(self, default_value: float = 0.0, *args, **kwargs) -> None:
        super().__init__("float", default_value, *args, **kwargs)

    def load(self, value: Any) -> None:
        try:
            self.value = float(value)
        except (ValueError, TypeError):
            raise ValueError(f"Invalid value for {self.description}. Expected a float.")


class VarString(Var):
    """
    String configuration variable.
    Example:
        project_name = VarString(default_value="MyProject", description="Project name")
    """

    def __init__(self, default_value: str = "", *args, **kwargs) -> None:
        super().__init__("string", default_value, *args, **kwargs)

    def load(self, value: Any) -> None:
        self.value = str(value)


class VarBool(Var):
    """
    Boolean configuration variable.
    Example:
        use_gpu = VarBool(default_value=True, description="Use GPU acceleration")
    """

    def __init__(self, default_value: bool = False, *args, **kwargs) -> None:
        super().__init__("bool", default_value, *args, **kwargs)

    def load(self, value: Any) -> None:
        if isinstance(value, bool):
            self.value = value
        elif isinstance(value, int):
            self.value = bool(value)
        elif isinstance(value, str):
            self.value = value.strip().lower() in ("true", "1", "yes", "on")
        else:
            raise ValueError(f"Invalid value for {self.description}. Expected a boolean.")


class VarPath(VarString):
    """
    Path configuration variable.
    Automatically normalizes file system paths.
    Example:
        output_dir = VarPath(default_value="./build", description="Output directory")
    """

    def __init__(self, default_value: str = "", description: str = "", is_dir: bool = False, filetypes: list[tuple[str, str | list[str] | tuple[str, ...]]] = None, *args, **kwargs) -> None:
        super().__init__(default_value, description, *args, **kwargs)
        self.var_type = "path"
        self.is_dir: bool = is_dir
        self.filetypes: list[tuple[str, str | list[str] | tuple[str, ...]]] = filetypes if filetypes is not None else [("*", "*")]

    def load(self, value: Any) -> None:
        super().load(value)
        self.value = os.path.normpath(self.value).replace("\\", "/")


class VarEnum(Var):
    """
    Enum configuration variable.
    Must be one of the allowed enum_values.
    Example:
        log_level = VarEnum(enum_values=["debug", "info", "warning"], default_value="info",
                            description="Logging level")
    """

    def __init__(
        self,
        enum_values: List[Any],
        default_value: Optional[Any] = None,
        *args,
        **kwargs,
    ) -> None:
        if default_value is None:
            default_value = enum_values[0] if enum_values else None
        self.enum_values: List[Any] = enum_values
        super().__init__("enum", default_value, *args, **kwargs)

    def load(self, value: Any) -> None:
        if value in self.enum_values:
            self.value = value
        else:
            raise ValueError(
                f"Invalid value for {self.description}. Expected one of {self.enum_values}."
            )

    def to_config(self) -> Dict[str, Any]:
        config = super().to_config()
        config["enum_values"] = self.enum_values
        return config


class VarList(Var):
    """
    List of configuration variables of the same type.
    Example:
        include_dirs = VarList(type_instance=VarPath(), description="Include directories")
    """

    def __init__(
        self,
        type_instance: Var,
        default_value: Optional[List[Any]] = None,
        *args,
        **kwargs,
    ) -> None:
        if default_value is None:
            default_value = []
        if not isinstance(type_instance, Var):
            raise ValueError("type_instance must be an instance of Var")
        self.type_instance: Var = type_instance
        super().__init__("list", default_value, *args, **kwargs)

    def load(self, value: Any) -> None:
        if not isinstance(value, list):
            raise ValueError(f"Invalid value for {self.description}. Expected a list.")
        self.value = []
        for item in value:
            item_instance = self.type_instance.clone()
            item_instance.load(item)
            self.value.append(item_instance)

    def to_data(self) -> List[Any]:
        return [item.to_data() for item in self.value]

    def to_config(self) -> Dict[str, Any]:
        config = super().to_config()
        config["item_type"] = self.type_instance.to_config()
        return config


class VarDict(Var):
    """
    Dictionary with fixed keys, each mapped to a specific Var type.
    Example:
        compiler_flags = VarDict({
            "debug": VarBool(default_value=True),
            "optimization": VarEnum(enum_values=["O0", "O1", "O2"], default_value="O2")
        }, description="Compiler flags")
    """

    def __init__(
        self,
        dict_of_instance: Dict[str, Var],
        default_value: Optional[Dict[str, Var]] = None,
        *args,
        **kwargs,
    ) -> None:
        if not isinstance(dict_of_instance, dict):
            raise ValueError("dict_of_instance must be a dictionary of Var instances")
        for key, value in dict_of_instance.items():
            if not isinstance(value, Var):
                raise ValueError(f"Value for key '{key}' must be an instance of Var")
        if default_value is None:
            default_value = {k: v.to_data() for k, v in dict_of_instance.items()}
        self.dict_of_instance: Dict[str, Var] = dict_of_instance
        super().__init__("dict", default_value, *args, **kwargs)

    def load(self, value: Any) -> None:
        if not isinstance(value, dict):
            raise ValueError(f"Invalid value for {self.description}. Expected a dictionary.")
        self.value = {}
        for key, var in self.dict_of_instance.items():
            var_clone = var.clone()
            var_clone.load(value.get(key, var.default_value))
            self.value[key] = var_clone

    def to_data(self) -> Dict[str, Any]:
        return {key: var.to_data() for key, var in self.value.items()}

    def to_config(self) -> Dict[str, Any]:
        config = super().to_config()
        config["template"] = {key: var.to_config() for key, var in self.dict_of_instance.items()}
        return config


class VarFreeDict(Var):
    """
    Dictionary with arbitrary keys and values.
    Each key and value are validated and converted using given Var types.
    Example:
        env_vars = VarFreeDict(key_type=VarString(), value_type=VarString(),
                               description="Environment variables")
    """

    def __init__(
        self,
        key_type: VarString,
        value_type: Var,
        default_value: Optional[Dict[Any, Any]] = None,
        *args,
        **kwargs,
    ) -> None:
        if not isinstance(key_type, VarString) or not isinstance(value_type, Var):
            raise ValueError("key_type and value_type must be instances of Var")
        if default_value is None:
            default_value = {}
        self.key_type: VarString = key_type
        self.value_type: Var = value_type
        super().__init__("dict", default_value, *args, **kwargs)

    def load(self, value: Any) -> None:
        if not isinstance(value, dict):
            raise ValueError(f"Invalid value for {self.description}. Expected a dictionary.")
        self.value = {}
        for key, val in value.items():
            key_instance = self.key_type.clone()
            key_instance.load(key)
            val_instance = self.value_type.clone()
            val_instance.load(val)
            self.value[key_instance] = val_instance

    def to_data(self) -> Dict[Any, Any]:
        return {key.to_data(): val.to_data() for key, val in self.value.items()}

    def to_config(self) -> Dict[str, Any]:
        config = super().to_config()
        config["key_type"] = self.key_type.to_config()
        config["value_type"] = self.value_type.to_config()
        return config
