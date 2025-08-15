# 🧩 Sprite Editor — Outil de création et gestion de sprites

`Sprite Editor` est un outil graphique inclus dans `ParticuleTools` permettant de sélectionner, nommer et manipuler des sprites directement à partir d'une image. Il gère les coordonnées, les noms, l'export JSON, et prépare les ressources visuelles pour les intégrer dans un projet `ParticuleEngine`.

---

## 🎮 Contrôles de base

| Action                   | Contrôle                                                 |
| ------------------------ | -------------------------------------------------------- |
| Créer un sprite          | **Clic gauche + glisser** sur l'image                    |
| Supprimer un sprite      | **Clic droit** sur un sprite                             |
| Renommer un sprite       | **Shift + clic droit** sur un sprite                     |
| Redimensionner un sprite | **Shift + clic** sur un des 4 coins de redimensionnement |
| Zoom avant/arrière       | **Molette de la souris**                                 |
| Déplacer la vue          | **Clic molette** + glisser                               |

---

## 📂 Menu principal

* **Fichier**

  * `Charger Image` : importe une image (.png, .jpg, .bmp)
  * `Importer Sprites` : charge un fichier `.sprites` de sprites existants
  * `Sauvegarder Sprites` : exporte automatiquement un fichier `.sprites` (format JSON interne) à côté de l'image
  * `Quitter` : ferme l'éditeur

* **Couleur de fond** : change la couleur d'arrière-plan de la zone d'édition

* **Aide** : ouvre une fenêtre explicative avec les contrôles (disponible en `fr` et `en`)

---

## 🌈 Couleur de fond

Vous pouvez personnaliser la couleur de fond via le menu. Cela permet de mieux distinguer les sprites selon l'image chargée.

Couleurs disponibles : `dark`, `light`, `blue`, `green`, `red`, `yellow`, `purple`

---

## 💾 Format `.sprites` exporté

Le fichier `.sprites` est un fichier JSON masqué, automatiquement enregistré avec le même nom que l’image et l’extension `.sprites`, placé **dans le même dossier** que l’image source.

Exemple : si vous éditez `image.png`, un fichier `image.png.sprites` sera généré à côté.

Il contient :

```json
{
  "image": "image.png",
  "sprites": [
    {"name": "NomDuSprite", "x": 10, "y": 20, "w": 32, "h": 32},
    ...
  ]
}
```

* `image` : nom de l'image associée (avec extension, sans chemin)
* `sprites` : liste des rectangles nommés (en pixels)

---

## 🧠 Bonnes pratiques

* Nommez chaque sprite avec soin : le nom est utilisé pour l'import dans le moteur.
* Utilisez la touche **Shift** pour accéder aux fonctions avancées (redimensionnement, renommage, info de debug).
* Le fichier `.sprites` doit toujours être dans le **même dossier** que l’image associée.

---

## 📎 Intégration avec le Makefile

Pour que les sprites exportés via le **Sprite Editor** soient automatiquement pris en compte lors de la compilation, vous devez ajouter l'attribut `"include_sprites": true` à chaque texture concernée dans le bloc `assets_files.textures` de votre Makefile.

Cela permet à ParticuleCraft d’inclure directement les données de découpes depuis le fichier `.sprites` associé à l’image.

### ✅ Exemple :

```json
"assets_files": {
  "textures": [
    {
      "path": "Assets/common/entities/players.png",
      "reference_path": "assets/entities/players.png",
      "include_sprites": true
    }
  ],
  "fonts": [],
  "audio": [],
  "other": []
}
```

### 🔎 Comportement

* Lorsqu’`include_sprites` est activé, le fichier `players.png.sprites` est automatiquement recherché dans le **même dossier** que l’image.
* Toutes les découpes nommées seront intégrées au build et accessibles via `GetResourceID(<reference_path>:<NomDuSprite>)`.


## 📌 Exemple d'intégration dans le moteur

Après avoir créé vos sprites et exporté le `.sprites`, vous pouvez charger un sprite dans votre jeu avec :

```cpp
Asset<Sprite> sprite(GetResourceID("assets/image.png:icon_ui"));
sprite->Draw(100, 100);
```
