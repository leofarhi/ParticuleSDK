# 🚀 Introduction à ParticuleCraft

**ParticuleCraft** est le module de compilation officiel du ParticuleEngine. Il fournit un système unifié pour gérer toutes les **distributions** de votre projet, qu'elles soient destinées à Windows, Linux, Casio, ou toute autre plateforme cible.

Son objectif est simple : **vous permettre de compiler pour plusieurs cibles à partir d'une base de code unique**, tout en adaptant automatiquement le comportement et les fichiers à chaque environnement de distribution.

[Commandes](craft/commandes.md) • [Guide](craft/makefile_guide.md)

---

## 🧩 Principe de fonctionnement

ParticuleCraft repose sur une architecture modulaire par **distribution** :

* Chaque distribution correspond à un **dossier** contenant :

  * Une version personnalisée du **ParticuleCore** adaptée à cette plateforme.
  * Un système de compilation propre (Makefile, script Python, json, etc).
  * Une configuration locale (compiler, flags, output, etc).

ParticuleCraft se charge de :

1. **Lister et gérer** toutes les distributions installables.
2. **Fusionner intelligemment** les composants communs et spécifiques dans un Makefile global.
3. **Centraliser la compilation** avec une interface en ligne de commande (ou via une interface, voir [ParticuleTools](tools/index.md)).

---

## 🛠️ Commandes disponibles

Voici les principales commandes que vous pourrez utiliser avec `particulecraft` (via `python3 -m particulecraft ...`) :

| Commande    | Description                                                                |
| ----------- | -------------------------------------------------------------------------- |
| `install`   | Installe une distribution (à partir de son nom)                            |
| `update`    | Met à jour une distribution installée                                      |
| `configure` | Configure une distribution après son installation                          |
| `uninstall` | Supprime proprement une distribution (coming soon 🔜)                     |
| `build`     | Compile un projet pour une distribution cible                              |
| `clean`     | Nettoie les fichiers temporaires de compilation (coming soon 🔜)          |
| `create`    | Génère un projet ou une bibliothèque compatible avec ParticuleEngine       |
| `config`    | Affiche les informations de configuration d'une distribution               |
| `tools`     | Liste les outils intégrés à la distribution ou exécute un outil spécifique |

---

## 📁 Exemple d'utilisation

Voici quelques commandes typiques qu'on peut exécuter depuis le terminal :

```bash
Particulecraft install --target CasioCG
Particulecraft configure --target CasioCG
Particulecraft build --target CasioCG --debug
```

> Ces exemples seront détaillés dans la prochaine page : [Utilisation de ParticuleCraft](craft/commandes.md).

---

## 🧠 Pourquoi ce système ?

Certaines plateformes n'ont pas la même organisation binaire, endianess, ou structure de compilation. ParticuleCraft est conçu pour **harmoniser** tout cela, en s'assurant que chaque distribution dispose de sa propre API spécifique, tout en permettant au développeur de conserver un code homogène et portable.

En d'autres termes, **vous codez une fois**, et **ParticuleCraft s'occupe du reste**.

---

## 🔗 Prochaine étape

Passez à la page suivante pour apprendre comment utiliser ParticuleCraft dans vos projets :

👉 [Utilisation de ParticuleCraft](/craft/commandes.md)
