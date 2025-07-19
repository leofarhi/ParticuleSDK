import os
import json
import uuid
from typing import Any, Dict, Optional


class UUIDManager:
    def __init__(self, json_path: str = None) -> None:
        self.json_path = json_path
        self.tree: Dict[str, Any] = {}  # arbre principal { ... }
        self.uuid_to_path: Dict[str, str] = {}
        self.object_to_uuid: Dict[str, str] = {}
        if json_path != None:
            self.load()

    def load(self, json_path = None) -> None:
        """Charge la structure depuis le fichier JSON."""
        if json_path is not None:
            self.json_path = json_path
        if self.json_path is None:
            raise ValueError("JSON path is not set. Cannot load UUIDs.")
        self.tree.clear()
        self.uuid_to_path.clear()
        self.object_to_uuid.clear()

        if os.path.exists(self.json_path):
            with open(self.json_path, "r", encoding="utf-8") as f:
                self.tree = json.load(f)
                self._rebuild_maps()

    def save(self, json_path = None) -> None:
        """Sauvegarde la structure actuelle dans le fichier JSON."""
        if json_path is not None:
            self.json_path = json_path
        if self.json_path is None:
            raise ValueError("JSON path is not set. Cannot save UUIDs.")
        with open(self.json_path, "w", encoding="utf-8") as f:
            json.dump(self.tree, f, indent=4)

    def clear(self) -> None:
        self.tree.clear()
        self.uuid_to_path.clear()
        self.object_to_uuid.clear()

    def _rebuild_maps(self) -> None:
        """Reconstruit les dictionnaires uuid→path et object→uuid depuis l'arbre."""
        def recurse(node: Any, path: str):
            if isinstance(node, dict):
                for k, v in node.items():
                    if k.startswith("uuid_"):
                        self.uuid_to_path[k] = path
                        self.object_to_uuid[str(v)] = k
                    else:
                        recurse(v, f"{path}.{k}" if path else k)
        recurse(self.tree, "")

    def _generate_uuid(self) -> str:
        while True:
            uid = "uuid_" + str(uuid.uuid4()).replace("-", "")
            if uid not in self.uuid_to_path:
                return uid

    def _get_category_node(self, path: str, create: bool = False) -> Optional[Dict[str, Any]]:
        node = self.tree
        for part in path.split("."):
            if part not in node:
                if create:
                    node[part] = {}
                else:
                    return None
            node = node[part]
            if not isinstance(node, dict):
                return None
        return node

    def add(self, obj: Any, path: str = "") -> str:
        """Ajoute un objet dans la catégorie donnée. Retourne son UUID."""
        obj_str = str(obj)
        if obj_str in self.object_to_uuid:
            return self.object_to_uuid[obj_str]

        node = self._get_category_node(path, create=True)
        if node is None:
            raise ValueError(f"Invalid path: {path}")

        uid = self._generate_uuid()
        node[uid] = obj
        self.uuid_to_path[uid] = path
        self.object_to_uuid[obj_str] = uid
        return uid

    def get(self, uuid_str: str) -> Optional[Any]:
        """Renvoie l'objet à partir de son UUID."""
        path = self.uuid_to_path.get(uuid_str)
        if not path:
            return None
        node = self._get_category_node(path)
        return node.get(uuid_str) if node else None

    def get_path(self, uuid_str: str) -> Optional[str]:
        """Renvoie le chemin de catégories de l'UUID."""
        return self.uuid_to_path.get(uuid_str)

    def get_uuid(self, obj: Any) -> Optional[str]:
        """Renvoie l'UUID d'un objet déjà connu (via str(obj))."""
        return self.object_to_uuid.get(str(obj))

    def remove_uuid(self, uuid_str: str) -> bool:
        """Supprime un UUID et son objet associé."""
        path = self.uuid_to_path.pop(uuid_str, None)
        if not path:
            return False

        node = self._get_category_node(path)
        if node and uuid_str in node:
            obj = node.pop(uuid_str)
            self.object_to_uuid.pop(str(obj), None)
            return True
        return False

    def remove_object(self, obj: Any) -> bool:
        """Supprime un objet (via str)."""
        uid = self.get_uuid(obj)
        if uid:
            return self.remove_uuid(uid)
        return False

    def list_category(self, path: str) -> Dict[str, Any]:
        """Renvoie tous les UUIDs/objets sous une catégorie."""
        node = self._get_category_node(path)
        return node if node else {}
    
if __name__ == "__main__":
    # Exemple d'utilisation
    mgr = UUIDManager("uuids.json")

    uuid1 = mgr.add({"name": "1.mp3", "volume": 1.0}, "assets.sounds.voices")
    uuid2 = mgr.add("main.cpp", "code")
    uuid3 = mgr.add("img1.png", "assets.textures")

    print(mgr.get(uuid1))           # → {'name': '1.mp3', 'volume': 1.0}
    print(mgr.get_path(uuid1))      # → "assets.sounds.voices"
    print(mgr.get_uuid("main.cpp")) # → uuid_xxxx

    mgr.remove_uuid(uuid2)
    mgr.save()
