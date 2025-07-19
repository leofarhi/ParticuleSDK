# `Vector2<T>`

`Vector2<T>` est une structure générique représentant un vecteur 2D. Elle est utilisée dans tout le moteur pour des positions, des tailles, des directions ou des vitesses. Elle fonctionne avec tous les types numériques (`int`, `float`, `fixed_t`, etc.).

---

## Constructeurs

```cpp
Vector2();                  // Initialise à (0, 0)
Vector2(T x, T y);          // Initialise avec x et y
template<typename U>
Vector2(const Vector2<U>& other); // Conversion d’un autre type
```

---

## Propriétés

- `x` : Coordonnée horizontale
- `y` : Coordonnée verticale

---

## Méthodes

```cpp
T length() const;
```
Renvoie la longueur du vecteur.

```cpp
Vector2 normalize();
```
Renvoie une version normalisée du vecteur (de longueur 1). Si la longueur est 0, retourne (0, 0).

---

## Opérateurs

Tous les opérateurs classiques sont définis :

- `+`, `-`, `*`, `/` avec un autre `Vector2` ou un scalaire
- `+=`, `-=`, `*=`, `/=`
- `==`, `!=`
- `-` (négation unitaire)
