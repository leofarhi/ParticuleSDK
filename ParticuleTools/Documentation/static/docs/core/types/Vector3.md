# `Vector3<T>`

`Vector3<T>` est une structure générique représentant un vecteur 3D. Elle est utilisée dans le moteur pour la 3D (position, rotation, normales, directions, etc.).

---

## Constructeurs

```cpp
Vector3();                         // Initialise à (0, 0, 0)
Vector3(T x, T y, T z);            // Initialise avec x, y, z
template<typename U>
Vector3(const Vector3<U>& other); // Conversion d’un autre type
```

---

## Propriétés

- `x`, `y`, `z` : Coordonnées spatiales

---

## Méthodes

```cpp
T length() const;
```
Renvoie la longueur du vecteur.

```cpp
Vector3 normalize();
```
Retourne une version normalisée du vecteur. Si la longueur est nulle, retourne (0, 0, 0).

```cpp
Vector3 cross(const Vector3& v) const;
```
Produit vectoriel entre deux vecteurs.

```cpp
T dot(const Vector3& v) const;
```
Produit scalaire entre deux vecteurs.

---

## Opérateurs

Même support que `Vector2<T>` :  
`+`, `-`, `*`, `/`, `==`, `!=`, `-`, `+=`, etc.
