# 🪟 Classe `Window`

La classe `Window` représente une fenêtre d'affichage dans le moteur Particule.  
Elle est conçue pour être héritée par l'utilisateur, afin de surcharger les méthodes de cycle de vie `OnStart`, `OnUpdate`, `OnDraw`, et `OnClose`.

---

## 🎯 Objectif

Permet à l'utilisateur de créer une fenêtre interactive, de gérer les entrées, d'effectuer un rendu, et de contrôler la taille, le titre, ou le comportement plein écran.

---

## 🧬 Héritage recommandé

```cpp
class MyGame : public Window {
public:
    void OnStart() override {
        // Initialisation
    }

    void OnUpdate() override {
        // Logique de jeu
    }

    void OnDraw() override {
        Clear();
        // Dessin des éléments à l'écran
        Display();
    }

    void OnClose() override {
        // Nettoyage
    }
};
```

---

## 🧱 Constructeurs

| Signature | Description |
|----------|-------------|
| `Window()` | Crée une fenêtre vide. |
| `Window(int width, int height, const std::string& title)` | Crée une fenêtre avec dimensions et titre. |

> Remarque : La classe ne peut pas être copiée (`delete` sur constructeur de copie et opérateur d'affectation).

---

## 🖼️ Méthodes principales

| Méthode | Description |
|--------|-------------|
| `void Display()` | Affiche à l'écran ce qui a été dessiné. À appeler **à la fin de `OnDraw()`**. |
| `void Clear()` | Efface le contenu affiché à l'écran. |
| `void Clear(Color color)` | Efface l'écran avec une couleur spécifique. |
| `Input GetKey()` | Retourne la dernière touche appuyée (voir `Input`). |
| `void UpdateInput()` | Met à jour les entrées clavier. |

---

## 📏 Gestion de la fenêtre

| Méthode | Description |
|--------|-------------|
| `int Width()` / `int Height()` | Retourne la taille actuelle. |
| `void SetWidth(int)` / `void SetHeight(int)` / `void SetSize(int, int)` | Change la taille. |
| `void SetTitle(const std::string&)` | Change le titre de la fenêtre. |
| `void SetPosition(int x, int y)` | Positionne la fenêtre. |
| `void SetFullscreen(bool)` | Active ou désactive le plein écran. |
| `void SetResizable(bool)` | Rend la fenêtre redimensionnable ou non. |
| `void Close()` | Ferme proprement la fenêtre (libère les ressources). |

---

## 🪄 Méthodes virtuelles

Ces méthodes doivent être **surchargées** dans vos propres classes dérivées pour créer le comportement du programme :

| Méthode | Quand elle est appelée |
|--------|-------------------------|
| `OnStart()` | Une seule fois au démarrage. |
| `OnUpdate()` | À chaque frame, logique du jeu. |
| `OnDraw()` | À chaque frame après `OnUpdate()`, pour dessiner. |
| `OnClose()` | Une seule fois avant la fermeture. |

---

## 🧷 Fonctions système internes

Ces méthodes sont utilisées par le moteur pour gérer la fenêtre active.  
> 🛑 **Ne pas appeler manuellement**.

- `Window::BindWindow(Window*)`  
- `Window::UnbindWindow()`  
- `Window::GetCurrentWindow()`

---

## ✅ Exemple minimal

```cpp
class BasicWindow : public Window {
public:
    void OnDraw() override {
        Clear(Color::Black);
        // exemple de rendu
        Display();
    }
};
```

---