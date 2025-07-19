# 🟦 Dessin de Formes (`Shapes`)

Le Particule SDK fournit des fonctions simples et rapides pour dessiner des formes primitives à l’écran. Ces fonctions opèrent toujours dans le contexte de la fenêtre actuellement active (`Window::GetCurrentWindow()`), et doivent être appelées dans la méthode `OnDraw()`.

---

## ✏️ Dessiner un pixel

```cpp
DrawPixel(x, y, color);
DrawPixelUnsafe(x, y, color);
DrawRawPixelUnsafe(x, y, rawColor);
```

### 📌 Détail des fonctions

- `DrawPixel(x, y, color)` : Dessine un pixel en vérifiant les limites de la fenêtre.
- `DrawPixelUnsafe(x, y, color)` : Dessine un pixel **sans** vérification des limites (plus rapide, mais potentiellement dangereux).
- `DrawRawPixelUnsafe(x, y, rawColor)` : Même que `DrawPixelUnsafe` mais en utilisant une couleur brute (`ColorRaw`), utile pour les rendus optimisés.

---

## 📏 Dessiner une ligne

```cpp
DrawLine(x1, y1, x2, y2, color);
DrawHLine(y, color);
```

- `DrawLine(...)` : Trace une ligne entre deux points.
- `DrawHLine(y, color)` : Remplit la ligne horizontale `y` de gauche à droite avec la couleur spécifiée.

---

## 🟥 Dessiner un rectangle

```cpp
DrawRectOutline(x, y, width, height, color);
DrawRectFilled(x, y, width, height, color);
```

- `DrawRectOutline(...)` : Dessine uniquement le contour du rectangle.
- `DrawRectFilled(...)` : Remplit entièrement le rectangle de couleur.

> ⚠️ Ces fonctions **n’utilisent pas** la classe `Rect` du namespace `Types`, elles prennent directement des coordonnées.

---

## 📌 Exemple dans `OnDraw()`

```cpp
void OnDraw() override {
    Clear();
    DrawPixel(10, 10, Color::Red);
    DrawLine(20, 20, 100, 40, Color::Green);
    DrawRectOutline(50, 50, 60, 40, Color::Blue);
    DrawRectFilled(120, 50, 60, 40, Color::Yellow);
    Display();
}
```

---

## 📦 Remarques

- Toutes les formes sont dessinées **directement** sur le framebuffer actif.
- Pour des performances optimales, privilégiez `DrawPixelUnsafe` et `DrawRawPixelUnsafe` dans les rendus en masse.
- Ces fonctions sont **indépendantes du système**, avec des implémentations adaptées selon la plateforme cible.