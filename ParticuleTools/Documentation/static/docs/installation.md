
# 📦 Installation du SDK Particule Engine

Ce guide explique comment installer le SDK **Particule Engine** grâce à l’installateur graphique `Setup.py` disponible dans le dossier `ParticuleTools/Setup`.

## ✅ Prérequis

Avant de lancer l’installation, assurez-vous que vous avez :
- Python 3.10 installé (autres versions possibles mais déconseillées)
- Le module `tkinter` (généralement installé avec Python)
- Le module `customtkinter` (`pip install customtkinter` si besoin)

---

## 🚀 Lancer l’installation

1. **Téléchargez ou clonez le projet ParticuleSDK.**
2. Ouvrez un terminal dans le dossier `ParticuleTools/Setup`.
3. Lancez l’installateur avec la commande suivante :
   ```bash
   python Setup.py
   ```

Cela ouvrira une interface graphique pour vous guider dans le processus.

---

## 🧭 Étapes de l'installation

L’installateur suit les étapes suivantes :

### 1. Bienvenue
Une page d’accueil vous souhaite la bienvenue et vous invite à commencer l’installation.

### 2. Sélection de la version de Python
- L’installateur détecte automatiquement les versions de Python disponibles.
- Vous pouvez également sélectionner manuellement l’exécutable Python à utiliser.

### 3. Installation de l’environnement virtuel
- Création automatique d’un environnement virtuel (`.venv`).
- Installation des dépendances nécessaires :
  - `pillow`
  - `customtkinter`
  - `flask`
  - `dataclasses`
  - `tkinter` (ou `python3-tk` sur Linux)
- Ajout d’un alias ou du chemin vers les outils dans votre `PATH` système.

⚠️ L’ajout au `PATH` nécessite un redémarrage du terminal (ou de la session) pour être effectif.

### 4. Terminé !
Une fois l’installation réussie, un message vous indique que le SDK est prêt à l’emploi.

---

## ❌ En cas d’erreur

Si une erreur est détectée lors de l’installation, une page vous l’indiquera avec un bouton pour réessayer.

---

## ✅ Résultat attendu

À la fin de l’installation, vous pourrez utiliser toutes les fonctionnalités de **Particule SDK**, comme l’éditeur de Makefile, l’éditeur de sprite, ou la compilation multi-distribution.

