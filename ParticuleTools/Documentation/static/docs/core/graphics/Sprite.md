# 🖼️ `Particule::Core::Sprite`

La classe `Sprite` représente une **sous-région d'une texture** destinée à être rendue.  
Elle permet d'extraire une portion d'une `Texture` (sprite sheet, atlas, etc.) et de la dessiner facilement à l'écran.

---

## ✨ Construction

### Par texture et rectangle

```cpp
Asset<Texture> tex = AssetManager::Load<Texture>(GetResourceID("assets/sprites.png"));
Rect rect = {0, 0, 32, 32};
Sprite sprite(tex, rect);
```

### Via Asset ID

```cpp
Sprite sprite(GetResourceID("assets/sprites.png:PlayerIdle"));
```

---

## 🧱 Membres internes

| Attribut | Type             | Description                      |
|----------|------------------|----------------------------------|
| `texture` | `Asset<Texture>` | Texture source                   |
| `rect`    | `Rect`           | Zone de découpe dans la texture |

---

## 🖍️ Méthodes de dessin

Toutes les fonctions de dessin s’appliquent à la portion (`Rect`) de la texture.

### `void Draw(int x, int y)`
> Dessine le sprite à la position `(x, y)`.

### `void DrawSize(int x, int y, int w, int h)`
> Dessine le sprite redimensionné.

### `void DrawColor(int x, int y, const Color& color)`
> Dessine le sprite avec une teinte (`Color`).

### `void DrawSizeColor(int x, int y, int w, int h, const Color& color)`
> Dessine le sprite redimensionné et teinté.

---

## 🧰 Autres méthodes

### `void SetRect(Rect rect)`
> Définit une nouvelle zone de découpe.

### `void SetRect(int x, int y, int w, int h)`
> Définit une nouvelle zone de découpe via ses coordonnées.

### `Texture* GetTexture()`
> Retourne un pointeur vers la texture utilisée.

### `Rect GetRect() const`
> Retourne la zone de découpe du sprite.

---

## ➕ Création de sous-sprites

### `Sprite* CreateSubSprite(Rect rect)`
> Crée un nouveau `Sprite` basé sur une **sous-région** de ce sprite.  
> Utile pour manipuler des régions imbriquées dans une sprite sheet.

---

## 🔄 Méthodes obligatoires pour l’intégration avec `AssetManager`

```cpp
static Sprite* Load(std::string path);
static void Unload(Sprite* sprite);
```

> Ces méthodes sont présentes pour des raisons de compatibilité, mais ne doivent pas être utilisées directement.

---

## 📌 Exemple de création manuelle

```cpp
Asset<Texture> tex(GetResourceID("assets/spritesheet.png"));
Sprite player(tex, Rect(0, 0, 64, 64));
player.Draw(100, 100);
```
