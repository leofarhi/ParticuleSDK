# 🛠️ Guide de configuration du Makefile
> Ce guide vous explique comment structurer et configurer proprement un Makefile pour ParticuleCraft. Il ne couvre **pas** les options spécifiques à une distribution, sauf si elles sont redondantes et communes.

## 🎯 Objectif du Makefile

Le Makefile est un fichier JSON qui décrit comment construire votre projet pour chaque distribution cible. Il regroupe deux types de blocs :

- **Bloc `common`** : paramètres génériques partagés par toutes les distributions.
- **Blocs de distribution (`Windows`, `Linux`, `Casio.CG90`, etc.)** : paramètres spécifiques à chaque environnement de compilation.

💡 Pour une configuration plus ergonomique et visuelle, **l’éditeur graphique ParticuleCraftUI** est disponible.

---

## 🧱 Structure globale

```json
{
  "common": { ... },
  "Windows": { ... },
  "Linux": { ... },
  "Casio.CG90": { ... }
}
```

Chaque bloc peut définir des clés comme `output_file`, `assets_files`, `inputs`, etc.

---

## 🔁 Bloc `common` (obligatoire)

Ce bloc contient la configuration de base partagée entre toutes les distributions.

| Clé                | Type         | Description |
|--------------------|--------------|-------------|
| `is_library`       | `bool`       | Compile le projet comme une bibliothèque. |
| `clean`            | `bool`       | Vide le dossier de build avant compilation. |
| `debug`            | `bool`       | Active le mode debug (si supporté). |
| `packages`         | `list`       | Intègre automatiquement des paquets (`["ParticuleEngine"]`). |
| `source_files`     | `list`       | Liste des fichiers source à compiler. |
| `include_paths`    | `list`       | Répertoires à inclure pour le compilateur. |
| `library_paths`    | `list`       | Répertoires à inclure pour l’éditeur de liens. |
| `defines`          | `dict`       | Dictionnaire de macros (`MACRO: valeur`). |
| `output_file`      | `string`     | Nom de sortie du binaire (sans extension). |
| `build_dir`        | `string`     | Répertoire pour les fichiers temporaires. |
| `bin_dir`          | `string`     | Répertoire pour le binaire final. |
| `project_name`     | `string`     | Nom du projet (affiché parfois en GUI). |
| `custom_makeconfig`| `list`       | Fichiers `.py` contenant du build personnalisé. |

### 📦 Paquets

L’attribut `packages` permet d’intégrer automatiquement des bibliothèques supplémentaires (ex: `ParticuleEngine`, `ParticuleTools`, etc.). Ces paquets sont configurés automatiquement pour votre plateforme.

Par défaut, seul le **ParticuleCore** est compilé. Pour bénéficier du système de scènes, GameObjects et composants, ajoutez `ParticuleEngine`.

---

## 🎮 Inputs typés

Chaque distribution peut définir un bloc `inputs` permettant de configurer finement les contrôles clavier, souris, manette, etc.

Un `Input` typé est défini par :

- Le **device** (`"Keyboard"`, `"Mouse"`, etc.)
- Le **type d’entrée** (`"Button"`, `"Axis1D"`, `"Axis2D"`, `"Pointer"`, etc.)
- Les **paramètres spécifiques** à ce couple device/type.

Exemple pour Windows (clavier) :

```json
"inputs": {
  "UP": [
    "Keyboard",
    ["Button", { "keycode": "SDLK_UP" }]
  ],
  "DOWN": [
    "Keyboard",
    ["Button", { "keycode": "SDLK_DOWN" }]
  ]
}
```

ℹ️ Chaque **distribution** possède ses propres devices, donc ce bloc doit être placé **dans le bloc de distribution**, pas dans `common`.

Vous pouvez lister et modifier dynamiquement les entrées disponibles avec :

```bash
ParticuleCraft configure --target Windows
```

Ou utiliser **ParticuleCraftUI** pour une interface plus ergonomique.

---

## 🎨 Gestion des assets (blocs de distribution)

Chaque distribution peut intégrer des assets en compilant à l’avance :

```json
"assets_files": {
  "textures": [],
  "fonts": [],
  "audio": [],
  "other": []
}
```

Tous les fichiers possèdent une `path` (chemin source) et un `reference_path` (où sera disponible l’asset à l’exécution).

Les **fonts** doivent aussi spécifier :
- `resolution` : hauteur du bitmap généré.
- `charset` : caractères utilisés dans la police.

**Important** : Ces champs sont disponibles dans toutes les distributions, même si leur traitement peut différer.

---

## 💡 Bonnes pratiques

- Utilisez des **chemins relatifs** pour plus de portabilité.
- Déclarez uniquement les assets nécessaires pour chaque distribution (évite le surpoids).
- Ne déclarez pas les attributs **spécifiques** de plateforme dans le bloc `common`.
- Privilégiez la centralisation des fichiers source dans `common.source_files`.
- Utilisez `custom_makeconfig` pour automatiser des comportements avancés.

---

## ✅ Exemple minimal

```json
{
  "common": {
    "is_library": false,
    "clean": true,
    "debug": false,
    "packages": ["ParticuleEngine"],
    "source_files": ["Sources/main.cpp"],
    "include_paths": ["Sources/include"],
    "project_name": "MyApp",
    "custom_makeconfig": []
  },
  "Windows": {
    "output_file": "my_app",
    "assets_files": {
      "textures": [],
      "fonts": [],
      "audio": [],
      "other": []
    },
    "inputs": {
      "UP": ["Keyboard", ["Button", { "keycode": "SDLK_UP" }]]
    }
  }
}
```

---

## 🔧 Conclusion

Le Makefile est un pont entre la configuration projet et les outils de build multi-plateformes. Prenez le temps de le structurer proprement pour éviter les erreurs de compilation.
