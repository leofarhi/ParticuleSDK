# 🎨 `Particule::Core::Color`

La classe Color représente une couleur RGBA avec un accès direct aux canaux individuels (`R`, `G`, `B`, `A`) ainsi qu'à une version brute (`ColorRaw`).Le format exact de `ColorRaw` dépend de la plateforme (16, 32, ou 64 bits), mais l'interface Color permet de manipuler toutes les couleurs de façon unifiée. Elle est utilisée pour dessiner des formes, du texte, des sprites, etc., dans tout le SDK.

---

## 🎨 Constructeurs

| Signature | Description |
|----------|-------------|
| `Color()` | Crée une couleur noire opaque `(0, 0, 0, 255)` |
| `Color(unsigned char r, g, b, a = 255)` | Crée une couleur à partir de canaux RGBA |
| `Color(ColorRaw raw)` | Construit une couleur à partir d'une valeur brute compressée |
| `Color(const Color &other)` | Constructeur par copie |
| `~Color()` | Destructeur de la couleur |

---

## 🔧 Accesseurs

### `unsigned char R(), G(), B(), A() const`
> Retourne respectivement la composante Rouge, Verte, Bleue ou Alpha de la couleur.

### `void SetR(), SetG(), SetB(), SetA()`
> Modifie une composante individuelle.

### `void Get(unsigned char& r, g, b, a) const`
> Remplit quatre variables avec les composantes actuelles.

---

## 🔁 Conversion

### `ColorRaw Raw() const`
> Retourne une version compressée de la couleur sous forme de `unsigned long long` (format interne optimisé).

### `static ColorRaw MultiplyColorRaw(ColorRaw a, ColorRaw b)`
> Multiplie deux couleurs compressées ensemble (multiplication des canaux).

---

## 🔄 Opérateurs

### `bool operator==(const Color& other) const`
> Vérifie si deux couleurs sont identiques.

### `bool operator!=(const Color& other) const`
> Vérifie si deux couleurs sont différentes.

---

## 🎨 Couleurs prédéfinies

| Couleur | Valeur |
|--------|--------|
| `Color::Black`   | `(0, 0, 0, 255)` |
| `Color::White`   | `(255, 255, 255, 255)` |
| `Color::Alpha`   | `(0, 0, 0, 0)` |
| `Color::Red`     | `(255, 0, 0, 255)` |
| `Color::Green`   | `(0, 255, 0, 255)` |
| `Color::Blue`    | `(0, 0, 255, 255)` |
| `Color::Yellow`  | `(255, 255, 0, 255)` |
| `Color::Cyan`    | `(0, 255, 255, 255)` |
| `Color::Magenta` | `(255, 0, 255, 255)` |

---

## 🔗 Voir aussi

- [`DrawRect`](Shapes_Rect.md)
- [`DrawPixel`](Shapes_Pixel.md)
- [`Font`](../font/Font.md)