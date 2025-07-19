# 🖐️ Gestion des Entrées (`Input`)

La classe `Input` du Particule SDK permet de gérer les entrées utilisateur (clavier, manette, etc.) sous une forme unifiée.

Chaque objet `Input` correspond à une touche, un bouton, ou un axe, et permet de vérifier son état ou de récupérer des valeurs analogiques.


!> **ℹ️ Remarque !**
 Actuellement, seul le **clavier** est pris en charge par le système d'entrée. La **souris** et les **manettes** ne sont pas encore supportées.

---

## 🤔 Création

```cpp
Input input;                        // Entrée vide par défaut
Input input2 = input;               // Copie
Input input3 = Input(input2);       // Constructeur de copie
Input input4 = Input(GetInput(str));// Constructeur via ID
```

---

## 🎮 Association d'une entrée

Pour récupérer un `Input` lié à une touche spécifique, utilise la fonction `GetInput("NOM")` :

```cpp
#include <Particule/Core/System/Redefine.hpp>
#include <Particule/Core/System/Input.hpp>

Input moveUp = Input(GetInput("UP"));
```
Les noms d’entrées ("UP", "JUMP", etc.) doivent être définis dans le Makefile.
Ce système permet d’associer dynamiquement un identifiant textuel à une touche réelle.


---

## 🔢 Fonctions principales

### `bool IsKeyPressed()`

Retourne `true` uniquement au moment de l'appui ("rising edge").

### `bool IsKeyDown()`

Retourne `true` tant que la touche ou l'entrée est maintenue.

### `bool IsKeyUp()`

Retourne `true` uniquement au moment du relâchement.

### `std::string GetKeyName()`

Retourne une chaîne de caractères identifiant la touche/bouton concerné.

---

## ↕️ Valeurs analogiques

### `fixed12_32 Get1DAxis()`

Retourne une valeur analogique sur un axe (par exemple joystick vertical) entre `-1.0` et `1.0`.

### `Vector2<fixed12_32> Get2DAxis()`

Retourne un vecteur bidimensionnel représentant une direction (stick analogique, pavé directionnel...)

---

## 🔗 Intégration

Les objets `Input` sont généralement fournis par la classe `Window` via sa méthode `GetKey()` après un `UpdateInput()`.

```cpp
window.UpdateInput();
Input in = window.GetKey();
if (in.IsKeyPressed())
{
    std::cout << "Touche appuyée : " << in.GetKeyName() << std::endl;
}
```

> Remarque : Pour que les états d'entrées soient mis à jour, `UpdateInput()` doit être appelé à chaque frame.

---

## ⚠️ Attention

* L'objet `Input` n'est pas une entrée unique (ex: touche A), mais une instance d'état liée à une touche identifiée.
* `Input` peut être copié sans problème.

---
