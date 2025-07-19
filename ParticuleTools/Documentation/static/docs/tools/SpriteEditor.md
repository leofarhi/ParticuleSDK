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
  * `Importer JSON` : charge un fichier `.json` de sprites existants
  * `Sauvegarder JSON` : exporte les sprites et l'image sous forme de fichier `.json`
  * `Quitter` : ferme l'éditeur

* **Couleur de fond** : change la couleur d'arrière-plan de la zone d'édition

* **Aide** : ouvre une fenêtre explicative avec les contrôles (disponible en `fr` et `en`)

---

## 🌈 Couleur de fond

Vous pouvez personnaliser la couleur de fond via le menu. Cela permet de mieux distinguer les sprites selon l'image chargée.

Couleurs disponibles : `dark`, `light`, `blue`, `green`, `red`, `yellow`, `purple`

---

## 💾 Format JSON exporté

L'export contient :

```json
{
  "image": "chemin/vers/image.png",
  "sprites": [
    {"name": "NomDuSprite", "x": 10, "y": 20, "w": 32, "h": 32},
    ...
  ]
}
```

* `image` : chemin relatif de l'image utilisée
* `sprites` : liste des rectangles nommés (en pixels)

---

## 🧠 Bonnes pratiques

* Nommez chaque sprite avec soin : le nom est utilisé pour l'import dans le moteur.
* Utilisez la touche **Shift** pour accéder aux fonctions avancées (redimensionnement, renommage, info de debug).
* Conservez le `.json` et l'image dans le même dossier pour éviter les chemins cassés.

---

## 📌 Exemple d'intégration dans le moteur

Après avoir créé vos sprites et exporté le `.json`, vous pouvez charger un sprite dans votre jeu avec :

```cpp
Asset<Sprite> sprite(GetResourceID("assets/image.png:NomDuSprite"));
sprite->Draw(100, 100);
```

> ⚠️ Actuellement, le chargement par nom de sprite (`image.png:Nom`) **n'est pas encore géré par l'AssetManager**. Cette fonctionnalité sera ajoutée ultérieurement.
