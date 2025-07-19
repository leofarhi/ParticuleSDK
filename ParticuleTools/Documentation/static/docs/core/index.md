# 🔧 ParticuleCore

**ParticuleCore** est le cœur bas niveau du SDK. Il fournit toutes les abstractions système nécessaires à la création d'applications multi-distribution.  
Il sert d'interface entre la machine et le moteur `ParticuleEngine`, en uniformisant les entrées, le rendu, les fichiers, les types numériques et les assets.

---

## 📦 Modules inclus

| Catégorie       | Composants clés                                                                                                                                 |
|-----------------|--------------------------------------------------------------------------------------------------------------------------------------------------|
| 🎧 Audio        | [`Sound`](core/audio/Sound.md) — Gestion des sons (lecture, pause, volume, pitch, looping...)                                                        |
| 🔤 Texte        | [`Font`](core/font/Font.md), `Character` — Chargement et rendu de texte bitmap                                                                       |
| 🎨 Graphismes   | [`Texture`](core/graphics/Texture.md), [`Sprite`](core/graphics/Sprite.md), [`Color`](core/graphics/Color.md), [`Rect`](core/types/Rect.md)                         |
| ✏️ Primitives   | [`DrawPixel`](core/graphics/Shapes.md), [`DrawLine`](core/graphics/Shapes.md), [`DrawRect`](core/graphics/Shapes.md)                          |
| 🖼️ Fenêtres     | [`Window`](core/system/Window.md), [`App`](core/system/App.md) — Création de fenêtres, boucle d'application, cycle de vie                                 |
| 🕹️ Entrées      | [`Input`](core/system/Input.md) — Gestion du clavier et des axes                                                                                     |
| 📁 Fichiers     | [`File`](core/system/File.md) — Lecture/écriture binaire, gestion des fichiers                                                                        |
| ⏱️ Temps        | [`Time`](core/system/Time.md), `Timer` — Gestion du deltaTime et des délais                                                                           |
| 🧠 AssetSystem  | [`Asset<T>`](core/system/AssetManager.md), [`AssetManager`](core/system/AssetManager.md) — Système de ressources intelligent, avec références et chargement différé |
| ➕ Types         | [`fixed_t`](core/types/Fixed.md), [`Rect`](core/types/Rect.md), [`Vector2`](core/types/Vector2.md), [`Vector3`](core/types/Vector3.md)                       |


---

## 🧠 Philosophie de conception

- Tous les composants sont **indépendants** mais conçus pour interagir efficacement.
- La logique est **séparée du rendu** : chaque module se concentre sur sa tâche.
- L’objectif est de **garantir la portabilité** sans compromettre les performances.

---

## 🔎 Pour aller plus loin

Consultez chaque sous-section pour découvrir les API, les structures, et des exemples concrets d’utilisation de ParticuleCore.