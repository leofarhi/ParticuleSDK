# 🖼️ Texture

La classe `Texture` du Particule SDK permet de manipuler des images 2D, que ce soit pour les afficher à l’écran, les modifier en mémoire, ou les découper en sprites.

---

## 📥 Chargement / Création

```cpp
#include <Particule/Core/System/References/Resource.hpp>
#include <Particule/Core/System/AssetManager.hpp>

Texture* t = Texture::Load("assets/img.asset");   // Charge une image (à éviter)
Texture::Unload(t);                               // Décharge proprement(à éviter)

Texture* tex = Texture::Create(128, 128);         // Crée une texture vide (écriture possible)
delete tex;

Asset<Texture> a(GetResourceID("assets/image.png")); //(recommandé)
Texture* b = AssetManager::Load<Texture>(GetResourceID("assets/image.png"));//(recommandé)
```

Voici une seconde remarque à ajouter dans ta documentation :

---

> 💡 **Bonne pratique recommandée**
>
> Il est **fortement conseillé d’utiliser** le système `Asset<Texture>` ou `AssetManager::Load<Texture>(id)` pour manipuler les textures dans le moteur.
> Cela permet une **gestion automatique** du cache, du chargement, du déchargement et du comptage de références.
>
> ⛔️ **Évite d’utiliser directement** `Texture::Load()` et `Texture::Unload()` sauf pour des cas très spécifiques non couverts par le système d’assets.

---

!> ❗ **Gestion de la mémoire**
 * Lorsque tu charges une texture avec `Texture::Load(path)`, tu dois **obligatoirement appeler** `Texture::Unload(texture)` une fois que tu n’en as plus besoin, pour libérer correctement la mémoire.
 * Si tu crées une texture manuellement avec `Texture::Create(width, height)`, tu dois **utiliser `delete`** sur le pointeur retourné lorsque tu as terminé.
 Cela garantit une gestion propre des ressources mémoire.

---


---

## 📐 Informations

```cpp
int width = tex->Width();
int height = tex->Height();
bool editable = tex->IsWritable(); // true si on peut écrire dans la texture
```

---

## 🖌️ Affichage

Les méthodes suivantes permettent de dessiner la texture à l’écran. Toutes ces méthodes doivent être utilisées entre `Clear()` et `Display()` dans `OnDraw()`.

```cpp
texture->Draw(x, y); // Affiche l’image à sa taille réelle

// 📏 Taille modifiée
texture->DrawSize(x, y, w, h); // Redimensionne

// 🟥 Découpe (Sub = portion de l’image)
texture->DrawSub(x, y, Rect(32, 32, 16, 16)); 

// 🎨 Teinte (multiplication de couleur)
texture->DrawColor(x, y, Color::Red); // Teinte l’image

// Combinaisons possibles :
texture->DrawSubSize(x, y, w, h, Rect(...));
texture->DrawSubColor(x, y, Rect(...), Color::White);
texture->DrawSizeColor(x, y, w, h, Color::White);
texture->DrawSubSizeColor(x, y, w, h, Rect(...), Color::White);
```

---

## 🎯 Pixel par pixel

> ⚠️ L’écriture ne fonctionne que si `IsWritable()` retourne `true`.  
> Après toute modification, **`UpdateTexture()`** est nécessaire pour actualiser.

### ✍️ Écriture

```cpp
tex->WritePixel(x, y, color);           // Standard (non sécurisé)
tex->WritePixelRaw(x, y, rawColor);     // Plus rapide, sans vérifications
tex->SetPixel(x, y, color);             // Sécurisé (vérifie les bornes)
```

### 📖 Lecture

```cpp
Color c = tex->ReadPixel(x, y);         // Standard (non sécurisé)
ColorRaw rc = tex->ReadPixelRaw(x, y);  // Plus rapide, sans vérifications
Color c2 = tex->GetPixel(x, y);         // Sécurisé (vérifie les bornes)
```

### 📤 Actualise l'image après écriture

```cpp
tex->UpdateTexture();
```

---

## 🧩 Autres fonctionnalités

### `bool PutPixel(int xTex, int yTex, int xScreen, int yScreen);`

Affiche directement un pixel depuis la texture vers l’écran.  
> ⚠️ Non sécurisé (ni bornes de texture ni de fenêtre vérifiées).

---

## 🧱 Sprites

```cpp
Sprite* s = tex->CreateSprite(Rect(0, 0, 32, 32));
```

Permet de découper une portion de texture sous forme de `Sprite`. Les sprites peuvent ensuite être animés ou affichés plus facilement.

---

## ℹ️ Récapitulatif terminologie

| Terme | Signification |
|------|---------------|
| `Sub` | Découpe une portion (`Rect`) de la texture |
| `Size` | Redimensionne l’image à l’affichage |
| `Color` | Multiplie les couleurs de la texture par une teinte |
| `PutPixel` | Copie un pixel de la texture vers l’écran (non sécurisé) |