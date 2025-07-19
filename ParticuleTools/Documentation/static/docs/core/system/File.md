# 📁 File

Le module `File` du Particule SDK fournit une API portable, homogène et endianness-safe pour la lecture et l'écriture de fichiers binaires et texte, quel que soit le système (Windows, Linux, Casio, etc).

Il assure une manipulation cohérente des fichiers sur toutes les plateformes en gérant notamment les différences d'endian (“ordre des octets”).

---

## 🎯 Objectif

* Simplifier l'accès aux fichiers (texte ou binaire)
* Gérer automatiquement l'endian (Big/Little)
* Offrir une interface unifiée avec fermeture automatique grâce à `OpenWith`

---

## 📖 Utilisation recommandée

```cpp
File::OpenWith("data.bin", FileMode::Read, Endian::LittleEndian, [](File& f) {
    int myValue;
    f.Read(myValue);
    // Utiliser myValue...
});
```

Cette forme garantit la fermeture du fichier, même en cas d'erreur.

---

## 📂 Modes d'ouverture

```cpp
enum class FileMode {
    Read,      // Lecture seule
    Write,     // Écriture (remplace le fichier)
    Append     // Ajout à la fin du fichier
};
```

---

## 🔁 Gestion de l'Endianness

```cpp
enum class Endian {
    BigEndian,
    LittleEndian
};
```

Vous pouvez préciser l'endian attendu à l'ouverture. Il sera respecté pour toutes les lectures/écritures binaires (ex: `int`, `float`, etc).

---

## 🔧 Fonctions principales

### `OpenWith()` ✅ **recommandé**

```cpp
template<typename Func>
static bool OpenWith(const std::string& path, FileMode mode, Endian endian, Func&& func);
```

Ouvre un fichier, exécute la fonction fournie avec une référence sur le fichier, puis le referme automatiquement.

### `Open()`

```cpp
static File* Open(const std::string& path, FileMode mode, Endian endian);
```

Retourne un pointeur vers un objet `File` qu'il faudra **penser à refermer manuellement** avec `Close()`.

### `Close()`

```cpp
static void Close(File* file);
```

Ferme un fichier ouvert **manuellement**.

---

## ✍️ Lecture / Écriture

### Lecture

```cpp
template<typename T>
void Read(T& data);

std::string ReadLine(); // jusqu'au '\n'
std::string ReadAll();  // lit tout le fichier
```

### Écriture

```cpp
template<typename T>
void Write(const T& data);
```

Vous pouvez aussi lire/écrire plusieurs éléments d'un coup :

```cpp
T tableau[64];
f.Write(tableau, 64);
f.Read(tableau, 64);
```

L'endian sera respecté automatiquement pour chaque élément.

---

## 🚫 Restrictions

* L'utilisation de `Open()` est déconseillée. Elle est présente uniquement pour les cas particuliers où la durée de vie du fichier doit être contrôlée manuellement.
* Le module ne permet pas encore de parcourir des répertoires ou de gérer les permissions.

---

## 📌 Exemple complet

```cpp
struct Vec3 { float x, y, z; };
Vec3 v1 = {1.0f, 2.0f, 3.0f};

// Écriture
File::OpenWith("vec.bin", FileMode::Write, Endian::BigEndian, [&](File& f) {
    f.Write(v1);
});

// Lecture
File::OpenWith("vec.bin", FileMode::Read, Endian::BigEndian, [&](File& f) {
    Vec3 v2;
    f.Read(v2);
});
```

---

## 🧠 Fonctionnement interne (info uniquement)

* Le fichier est encapsulé via `FILE*` standard C.
* L'endian est géré en inspectant le type et inversant les octets si besoin.
* `OpenWith` encapsule `try/finally` et garantit la fermeture.
