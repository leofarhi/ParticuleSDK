# 🔤 `Particule::Core::Font` & `Character`

La classe `Font` permet d’afficher du texte bitmap à l’écran dans n’importe quelle fenêtre `Particule::Core::Window`.  
Les polices sont chargées depuis un format binaire propriétaire via `Font::Load()`, et chaque caractère est représenté par une structure `Character`.

---

## 🧱 Structure : `Character`

La structure `Character` représente un glyphe isolé dans une police.

### Attributs :

| Champ     | Type    | Description                                        |
|-----------|---------|----------------------------------------------------|
| `left`    | `int8_t` | Offset gauche du dessin par rapport à l’origine    |
| `top`     | `int8_t` | Offset haut du dessin                             |
| `right`   | `int8_t` | Offset droite                                     |
| `bottom`  | `int8_t` | Offset bas                                        |

### Fonctions membres :

#### `int width() const`
> Renvoie la largeur du caractère en pixels.

#### `int height() const`
> Renvoie la hauteur du caractère en pixels.

#### `bool get(int x, int y) const`
> Retourne `true` si le pixel local `(x, y)` est actif (fait partie du glyphe).

#### `int draw(int x, int y, const Color& color, fixed12_32 ratio) const`
> Dessine le caractère à l'écran à la position `(x, y)` avec une couleur et un ratio d’échelle.

---

## ✍️ Classe : `Font`

La classe `Font` permet d’afficher du texte entier et de gérer le rendu typographique.

### Constructeurs / Destructeurs :

- `Font()` — Constructeur privé (polices créées uniquement via `Load()`).
- `Font(const Font&)` — Copie privée.
- `~Font()` — Libère la ressource.

---

## 📚 Méthodes statiques

> ⚠️ **Ne pas utiliser directement `Font::Load()` ni `Font::Unload()`**.  
> Utilisez plutôt le système d’assets :

```cpp
Asset<Font> fontAsset(GetResourceID("assets/fonts/Arial.ttf"));
```

ou bien :

```cpp
Font* font = AssetManager::Load<Font>(GetResourceID("assets/fonts/Arial.ttf"));
```

Cela garantit un chargement sécurisé, une gestion de la mémoire automatique et une intégration avec le système de cache.

### `static Font* Load(std::string path)`
> Charge une police depuis un fichier `.pft` (format de police binaire de Particule).  
> ⚠️ Cette méthode est réservée aux cas avancés ou internes. Utilisez `Asset<Font>` de préférence.

### `static void Unload(Font* font)`
> Libère proprement une police chargée.  
> ⚠️ Réservée aux cas où `Font::Load()` est utilisé manuellement. À éviter dans un usage standard.

---

## 🛠️ Méthodes publiques

### `int GetResolution()`
> Retourne la résolution native de la police (hauteur maximale en pixels pour `size = 1`).

### `void DrawText(const std::string& text, int x, int y, const Color& color, int size)`
> Dessine une chaîne de caractères à l’écran à la position `(x, y)`, avec une taille relative (`size = 1` = taille native), et une `Color`.

### `Vector2<int> GetTextSize(const std::string& text, int size)`
> Retourne la largeur et hauteur finale du texte affiché en fonction de la chaîne et de la taille.

---

## 🔗 Voir aussi

- [`Color`](../graphics/Color.md)
- [`Window`](../system/Window.md)
- [`Vector2`](../types/Vector2.md)
- [`fixed12_32`](../types/Fixed.md)