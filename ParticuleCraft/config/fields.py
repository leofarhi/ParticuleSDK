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


class VarSelect(Var):
    """
    Selecteur de variables : une option parmi plusieurs, chaque option ayant son propre type Var.
    Exemple :
        s = VarSelect(
            choices={
                "Choix 1": VarInt(default_value=5, description="Un entier"),
                "Choix 2": VarBool(default_value=True, description="Un booléen"),
            },
            default_value=("Choix 1", VarInt(15))
        )

    Règles :
    - Si default_value est None :
        * s'il y a des choices : on prend le premier label et on clone son Var avec sa valeur par défaut
        * sinon : value = None
    - Lors du load :
        * value peut être un tuple (label, valeur_brule) ou (label, VarInstance)
        * le type du VarInstance doit correspondre au gabarit de `choices[label]`, sinon on lève une ValueError
    """

    def __init__(
        self,
        choices: Dict[str, Var],
        default_value: Optional[tuple[str, Any]] = None,
        *args,
        **kwargs,
    ) -> None:
        if not isinstance(choices, dict):
            raise ValueError("choices doit être un dictionnaire {label: Var}.")
        for k, v in choices.items():
            if not isinstance(v, Var):
                raise ValueError(f"choices['{k}'] doit être une instance de Var.")

        # On doit définir choices AVANT l'appel à super().__init__ car load() les utilise.
        self.choices: Dict[str, Var] = choices

        # Calcul du default_value à passer à super().__init__ (qui appellera load()).
        if default_value is None:
            if self.choices:
                first_key = next(iter(self.choices))
                # On passe une donnée 'brute' attendue par le Var cible (sa default_value)
                dv = (first_key, self.choices[first_key].default_value)
            else:
                dv = None
        else:
            # Laisser load() valider / convertir
            dv = default_value

        super().__init__("select", dv, *args, **kwargs)

    def load(self, value: Any) -> None:
        # Cas aucun choix défini
        if not self.choices:
            if value is None:
                self.value = None
                return
            raise ValueError(f"Invalid value for {self.description}. Aucun choix n'est disponible.")

        # None → pas de sélection
        if value is None:
            self.value = None
            return

        # On accepte soit un dict {"choice":..., "value":...}, soit un tuple (choice, value)
        if isinstance(value, dict):
            if "choice" not in value or "value" not in value:
                raise ValueError(
                    f"Invalid value for {self.description}. Attendu un dict {{'choice':..., 'value':...}}."
                )
            choice_key = value["choice"]
            raw_val = value["value"]
        else:
            # tuple attendu
            try:
                choice_key, raw_val = value
            except Exception:
                raise ValueError(
                    f"Invalid value for {self.description}. Attendu un tuple (choice, value)."
                )

        # Validation du label
        if choice_key not in self.choices:
            raise ValueError(
                f"Invalid value for {self.description}. Le choix '{choice_key}' n'existe pas. "
                f"Choix possibles: {list(self.choices.keys())}."
            )

        # On part d'un clone du gabarit
        template_var = self.choices[choice_key].clone()

        # Deux cas : raw_val est une instance de Var OU une donnée brute
        if isinstance(raw_val, Var):
            # Le type doit correspondre (même classe ou au moins même var_type)
            same_class = raw_val.__class__ is template_var.__class__
            same_vartype = getattr(raw_val, "var_type", None) == getattr(template_var, "var_type", None)
            if not (same_class or same_vartype):
                raise ValueError(
                    f"Invalid value for {self.description}. Le type de la valeur pour '{choice_key}' "
                    f"({raw_val.__class__.__name__}/{getattr(raw_val, 'var_type', None)}) ne correspond pas au type attendu "
                    f"({template_var.__class__.__name__}/{template_var.var_type})."
                )
            # Charger à partir des données de l'instance fournie
            template_var.load(raw_val.to_data())
        else:
            # Donnée brute → on laisse le Var cible faire la conversion/validation
            template_var.load(raw_val)

        # Stockage : (label, VarInstance porteur de la valeur)
        self.value = (choice_key, template_var)

    def to_data(self) -> Any:
        if self.value is None:
            return None
        choice_key, var_instance = self.value
        return (choice_key, var_instance.to_data())

    def to_config(self) -> Dict[str, Any]:
        config = super().to_config()
        config["choices"] = {k: v.to_config() for k, v in self.choices.items()}
        return config