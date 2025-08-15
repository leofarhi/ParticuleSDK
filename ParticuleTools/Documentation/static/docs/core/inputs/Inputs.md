
# 🕹️ Gestion des Entrées (`Input<T>`) — Particule SDK

Le système d'entrée du **Particule SDK** permet une gestion **typée** et **multi-device** des entrées utilisateur (clavier, souris, manette, etc.) de manière **unifiée**, **modulaire**, et **portable**.

Chaque entrée est représentée sous la forme `Input<Type>`, où `Type` peut être :

- `Inputs::Button` — pour les boutons (clavier, manette, etc.)
- `Inputs::Axis1D` — pour les axes analogiques (stick gauche/droite, triggers...)
- `Inputs::Axis2D` — pour les directions bidimensionnelles (stick analogique, D-pad)
- `Inputs::Pointer` — pour les pointeurs (souris, tactile...)

---

## 🧠 Principe de fonctionnement

Chaque `Input<T>` représente un **lien abstrait** vers une source d'entrée réelle (appelée `Device<T>`), comme un clavier ou une souris.

> Chaque plateforme peut exposer des *devices* différents (ex: `Keyboard`, `Mouse`, etc.).  
> Le Makefile déclare quels `Input<T>` sont disponibles via des alias textuels accessibles avec `GetInput(T, "NAME")`.

Exemple d’utilisation :

```cpp
#include <Particule/Core/System/References/Input.hpp>
using namespace Particule::Core;

auto* input = GetInput(Inputs::Button, "UP");
if (input && input->IsPressed()) {
    // "UP" vient d'être pressé
}
```

---

## 🎛️ Types d'entrées disponibles

### 🔘 `Input<Inputs::Button>`

Pour gérer les boutons (clavier, manette, clic souris...)

Méthodes principales :

```cpp
bool IsPressed(); // Appui détecté cette frame uniquement (rising edge)
bool IsDown();    // Bouton maintenu
bool IsUp();      // Relâchement détecté cette frame uniquement (falling edge)
std::string GetKeyName(); // Nom de la touche si disponible
```

---

### 📈 `Input<Inputs::Axis1D>`

Pour des axes analogiques unidimensionnels :

```cpp
fixed12_32 Value(); // Valeur entre -1.0 et 1.0
```

Utilisable pour un stick horizontal ou un trigger analogique.

---

### 🧭 `Input<Inputs::Axis2D>`

Pour les directions en 2D (joystick, D-pad...) :

```cpp
Vector2<fixed12_32> Value(); // Valeur entre (-1, -1) et (1, 1)
```

---

### 🖱️ `Input<Inputs::Pointer>`

Pour des pointeurs (souris, tactile...) :

```cpp
Vector2<fixed12_32> Position(); // Position [0,1]²
Vector2<fixed12_32> Delta();    // Déplacement depuis la frame précédente
Vector2<fixed12_32> Scroll();   // Scroll horizontal/vertical
const Button& Button(PointerButton b); // Accès aux boutons (Left, Right, etc.)
```

---

## ⚙️ Récupération via `GetInput`

Le SDK génère automatiquement des alias d’entrée avec `GetInput(Type, "NAME")`, en se basant sur le Makefile.

Exemple :

```cpp
auto* jump = GetInput(Inputs::Button, "JUMP");
if (jump && jump->IsPressed()) { /* ... */ }
```

> Les noms disponibles (`"JUMP"`, `"UP"`, etc.) sont définis dans le Makefile.  
> Si un nom n'est pas reconnu, `GetInput(...)` retourne `nullptr`.

---

## 🧩 Architecture extensible

Chaque type d'entrée est spécialisé (`Input<Button>`, `Input<Axis2D>`, etc.) et peut être implémenté par n’importe quel périphérique.

### Exemple : `Keyboard<Inputs::Button>`

```cpp
Inputs::Devices::Keyboard<Inputs::Button> key(SDLK_SPACE);
if (key.IsPressed()) { ... }
```

Mais on peut aussi utiliser :

```cpp
Input<Inputs::Button>* input = &key;
```

> ⚠️ Les `Devices` (comme `Keyboard`) peuvent exposer des **méthodes supplémentaires**.  
> Pour y accéder, vous pouvez faire un `dynamic_cast` si nécessaire.

---

## 📦 Intégration par plateforme

Chaque **distribution** (Windows, Linux, Casio...) fournit ses propres `Device<T>` et implémente leurs comportements dans `Devices::`.

Les devices valides pour une plateforme sont précisés dans sa configuration (via le Makefile).

---

## 🛠️ Remarques

- Tous les types `Input<T>` sont **non modifiables** et peuvent être partagés.
- Vous pouvez créer vos propres `Device<T>` si vous ciblez une plateforme spécifique.
- `UpdateInput()` (sur `Window`) doit être appelé chaque frame pour que les états soient à jour.

---

## ✅ Exemple complet

```cpp
#include <Particule/Core/System/References/Input.hpp>
using namespace Particule::Core;

void Update()
{
    auto* fire = GetInput(Inputs::Button, "FIRE");
    if (fire && fire->IsPressed()) { /* tirer */ }

    auto* stick = GetInput(Inputs::Axis2D, "MOVE");
    if (stick) {
        Vector2<fixed12_32> dir = stick->Value();
        // utiliser dir.x, dir.y
    }
}
```