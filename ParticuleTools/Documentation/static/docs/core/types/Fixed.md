# 🧮 `fixed_t` — Arithmétique à virgule fixe

La classe `fixed_t` fournit un système de nombre à virgule fixe ultra-optimisé pour remplacer `float` ou `double` dans les environnements à faibles ressources, ou dans les moteurs nécessitant un comportement déterministe.

> Elle est particulièrement utile pour le rendu logiciel, les calculs graphiques 2D/3D, ou sur des plateformes embarquées (comme Casio, microcontrôleurs, etc.).

---

## ✅ Présentation

`fixed_t<PRECISION, T>` permet de représenter un nombre réel à l’aide d’un entier de type `T` (ex : `int32_t`) dont `PRECISION` bits sont utilisés pour la partie fractionnaire.

```cpp
using fixed12_32 = fixed_t<12, int32_t>;  // 12 bits de fraction, 20 de partie entière
```

---

## ⚙️ Constructeurs

| Constructeur                              | Description |
|------------------------------------------|-------------|
| `fixed_t()`                              | Crée une valeur nulle |
| `fixed_t(int)`                           | Conversion depuis un entier |
| `fixed_t(float)` / `fixed_t(double)`     | Conversion depuis un flottant |
| `fixed_t(T raw, true)`                   | Création depuis une valeur brute |
| `fixed_t<P2, T2>(fixed_t<P2, T2>)`       | Conversion depuis un autre `fixed_t` |

---

## 🔢 Accès et conversion

| Méthode                        | Description |
|--------------------------------|-------------|
| `raw()`                        | Retourne la valeur brute |
| `operator int/float/double()`  | Conversion implicite |
| `floor()` / `ceil()` / `round()` | Arrondi vers le bas/haut/plus proche |
| `abs()`                        | Valeur absolue |
| `from_raw(T)`                  | Crée un fixed à partir d'une valeur brute |

---

## ➕ Opérateurs supportés

- `+`, `-`, `*`, `/` avec :
  - d'autres `fixed_t`
  - des `int`, `float`, `double`

- Comparaisons `==`, `<`, `>`, etc.

---

## 🧮 Fonctions mathématiques

| Méthode                     | Description |
|-----------------------------|-------------|
| `sqrt(x)`                  | Racine carrée |
| `inv_sqrt(x)`              | Inverse rapide √ (méthode de Quake III) |
| `inv(x)`                   | Inverse (1 / x) |
| `sin(x)` / `cos(x)`        | Sinus / Cosinus optimisés |
| `atan2(y, x)`              | Arctangente optimisée |
| `asin(x)`                  | Arcsinus |
| `lerp(a, b, t)`            | Interpolation linéaire |
| `ease(x)` / `ease_in(x)`   | Fonctions d’interpolation |

---

## 🔁 Équivalents prédéfinis

| Nom             | Définition |
|----------------|------------|
| `fixed12_32`   | 12 bits de fraction, 32 bits total |
| `fixed16_64`   | 16 bits de fraction, 64 bits total |
| *(et d'autres variantes)* | |

---

## 🎯 Utilisation typique

```cpp
using namespace Particule::Core;

fixed12_32 x = 1.5f;
fixed12_32 y = 2;
fixed12_32 z = x * y;

if (z > fixed12_32(3.0f)) {
    // ...
}
```

---

## 📌 Remarques

- Les calculs sont **déterministes**, parfaits pour les systèmes à logique partagée (ex : synchronisation réseau).
- Évite les erreurs d'arrondi flottant sur les petites plateformes.
- Peut être utilisé à la place des floats dans tous les calculs internes du moteur.

