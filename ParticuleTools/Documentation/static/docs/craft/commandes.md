# ⚙️ Commandes ParticuleCraft

Cette page liste toutes les commandes disponibles avec `ParticuleCraft`, l'outil de compilation modulaire de ParticuleEngine. Pour utiliser ces commandes, exécute simplement :

```bash
ParticuleCraft <commande> [options]
```

---

## 📦 `install`

Installe une distribution spécifique (ex: Windows, Linux, Casio...).

```bash
ParticuleCraft install --target <nom>
```

**Exemple :**

```bash
ParticuleCraft install --target Windows
```

---

## 🔁 `update`

Met à jour une distribution déjà installée.

```bash
ParticuleCraft update --target <nom>
```

**Exemple :**

```bash
ParticuleCraft update --target CasioCG
```

---

## ⚙️ `configure`

Configure une distribution avant compilation (préparation des fichiers, paths, etc).

```bash
ParticuleCraft configure --target <nom>
```

**Exemple :**

```bash
ParticuleCraft configure --target Linux
```

---

## ❌ `uninstall`

Supprime une distribution installée.

```bash
ParticuleCraft uninstall --target <nom>
```

**Exemple :**

```bash
ParticuleCraft uninstall --target CasioCG
```

---

## 🛠️ `build`

Compile le projet pour une distribution donnée.

```bash
ParticuleCraft build --target <nom> [--makefile <fichier>] [--debug]
```

**Options :**

* `--makefile` : Permet d'utiliser un Makefile spécifique (sinon celui par défaut est utilisé).
* `--debug` : Active le mode debug (drapeaux de compilation).

**Exemple :**

```bash
ParticuleCraft build --target Windows --debug
```

---

## 🧹 `clean`

Supprime les fichiers temporaires et les objets de compilation.

```bash
ParticuleCraft clean [--makefile <fichier>]
```

**Exemple :**

```bash
ParticuleCraft clean
```

---

## 🆕 `create`

Crée un nouveau projet compatible avec ParticuleCraft.

```bash
ParticuleCraft create [--makefile <fichier>] [--library]
```

**Options :**

* `--library` : Initialise un projet sous forme de bibliothèque (au lieu d'un exécutable).

**Exemple :**

```bash
ParticuleCraft create --library
```

---

## 🔍 `config`

Affiche les informations de configuration d'une distribution ou du projet courant.

```bash
ParticuleCraft config [--target <nom>] [--library]
```

**Exemple :**

```bash
ParticuleCraft config --target Linux
```

---

## 🧰 `tools`

Liste ou exécute un outil fourni par une distribution.

```bash
ParticuleCraft tools [<nom_de_l_outil>]
```

**Exemples :**

```bash
ParticuleCraft tools                    # Liste tous les outils
ParticuleCraft tools ParticuleCraftUI   # Lance l'outil "Export"
```

---

## 💡 Bonnes pratiques

* Chaque distribution peut avoir sa propre logique de compilation.
* L'utilisation de `configure` est souvent indispensable avant un `build`.
* Les distributions sont modularisées, vous pouvez en ajouter facilement dans le dossier `Distributions/`.
