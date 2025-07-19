# 🚀 Guide de démarrage – Particule SDK

Bienvenue dans le guide de démarrage rapide de **Particule SDK**. Cette page vous aidera à configurer votre environnement de développement et à lancer votre premier projet en quelques étapes simples.

---

## ✅ Prérequis

Avant de commencer, assurez-vous d'avoir :

- Une version de **Python 3.10+** installée.
- Les bibliothèques suivantes : `tkinter`, `customtkinter`, `pillow`, `flask`, `dataclasses`.
- Un système compatible (Windows, Linux, macOS).

---

## 📦 Installation du SDK

La manière la plus simple d’installer le SDK est d’utiliser le programme d’installation graphique :

```bash
cd ParticuleTools/Setup
python Setup.py
```

L’installateur vous guidera dans :

1. La sélection de la version de Python.
2. La création d’un environnement virtuel.
3. L’installation automatique des dépendances.
4. L’ajout au PATH du système pour accéder aux outils en ligne de commande (`ParticuleCraft`, etc.).

---

## 🛠️ Installation d’une distribution

Une fois le SDK installé, vous devez installer la ou les distributions que vous souhaitez utiliser (ex. : Windows, Linux, etc.).
Chaque distribution contient les outils et la configuration nécessaires à la compilation pour une plateforme cible.

### 🔧 Commande d’installation

Pour installer une distribution, utilisez la commande suivante dans un terminal :

```bash
ParticuleCraft install --target Windows
```

> Remplacez `Windows` par le nom de la distribution que vous souhaitez installer (`Linux`,  etc.).

Cette commande :

* Télécharge et configure automatiquement les fichiers de la distribution dans le dossier `ParticuleCore/Distributions/<Nom>`.
* Prépare les outils de compilation pour cette distribution.
* Fusionne la configuration de la distribution avec le système de Makefile.

---

## 🏗️ Créer un projet

Une fois l'installation terminée, utilisez la CLI pour créer un nouveau projet :

```bash
ParticuleCraft create
```

Cela vous permet de générer un fichier `Makefile.json`.

---

## 🎮 Coder et importer vos assets

Une fois votre projet créé, vous pouvez immédiatement commencer à coder et à importer vos premiers assets.

### 🖼️ Importer vos textures

Utilisez l’éditeur de sprites intégré pour découper vos sprites depuis une image source, les nommer et les sauvegarder.  
Ce fichier pourra ensuite être référencé dans votre `Makefile.json` pour que les sprites soient disponibles dans votre projet.

### 📦 Ajouter des fichiers

Vous pouvez également ajouter directement d’autres fichiers (sons, polices, scripts) dans les répertoires de votre projet et les déclarer dans le `Makefile.json`.

---

## 🧱 Compiler un projet

```bash
ParticuleCraft build --target Windows
```

Les cibles disponibles dépendent de vos distributions configurées.

---

## 🧹 Nettoyer un projet

```bash
ParticuleCraft clean
```

Supprime les fichiers générés.

---

## 🔁 Réinstaller ou mettre à jour une distribution

```bash
ParticuleCraft update --target Windows
```

---

## 📖 Continuer

Explorez la documentation complète pour apprendre à :

- Utiliser le `Makefile Editor` en profondeur.
- Ajouter vos propres distributions personnalisées.
- Automatiser la compilation.

---

Bonne création 🎮 !
