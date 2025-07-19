# 📦 AssetManager

Le système `AssetManager` de Particule gère dynamiquement le chargement, le déchargement et le partage des ressources à travers le moteur.

## 🎯 Objectif

Permet de charger dynamiquement des ressources :
- **intégrées au binaire** (built-in)
- **externes connues** à l’avance avec ID fixe
- **externes dynamiques** (chargées manuellement sans ID prédéfini)

---

## 📁 `AssetManager`

### Fonctionnement général

- Les assets sont référencés soit via un ID fixe, soit via un pointeur direct.
- Les assets intégrés sont accessibles via un tableau statique.
- Les assets externes utilisent un tableau `externalAssets` avec fonctions de chargement personnalisées.

---

## 📤 Chargement & Référencement

```cpp
AssetManager::SetupLoaders<MyType>(id);
MyType* asset = AssetManager::Load<MyType>(id);
```

Chaque type de ressource externe doit fournir :
- une fonction `Load(path)`
- une fonction `Unload(ptr)`

Le système garde en cache le pointeur retourné, et incrémente un compteur de références pour éviter les doublons et faciliter le déchargement conditionnel.

> `Load<T>(id)` est autorisé mais il est recommandé d’utiliser `Asset<T>` à la place.

---

## 📦 Obtenir un ID d'asset

Pour charger un asset, il est **fortement recommandé** d’utiliser la fonction `GetResourceID("chemin/vers/le/fichier")`, qui convertit dynamiquement un chemin en ID unique :

```cpp
Asset<Texture> tex(GetResourceID("assets/image.png"));
```

Tu peux aussi l’utiliser avec `AssetManager::Load()` si besoin :

```cpp
Texture* tex = AssetManager::Load<Texture>(GetResourceID("assets/image.png"));
```

> Le chemin doit être défini dans le Makefile/configuration de build, afin d’être intégré dans le système d’assets.

---

## 🔁 Gestion de la vie

```cpp
AssetManager::LoadUsed();
AssetManager::UnloadUnused();
AssetManager::UnloadAll(); // ⚠ à éviter
AssetManager::Unload(id);  // ⚠ à éviter
```

---

## ✅ Vérification

```cpp
AssetManager::IsLoaded(id);
```

---

## 🧠 Classe `Asset<T>`

Une abstraction qui permet de manipuler un asset dynamiquement sans s’occuper de sa mémoire manuellement.

### Construction

```cpp
Asset<Texture> a(GetResourceID("assets/image.png"));      // via ID
Asset<Texture> b(texPtr);  // pointeur direct (non déchargeable automatiquement)
```

### Utilisation

```cpp
if (a.IsValid() && a.IsLoaded()) {
    a->Draw();   // équivalent à (*a).Draw()
}
```

### Chargement différé

```cpp
a.Load(); // charge l’asset si nécessaire
```

---

## 📌 Exemple

```cpp
Asset<Font> fontAsset(GetResourceID("assets/fonts/Arial.ttf"));
fontAsset->DrawText("Bonjour", 10, 10, Color::White, 12);
```

---

## 🔍 Détails internes (à titre informatif uniquement)

> Cette section explique le fonctionnement du système d’assets en profondeur.  
> Elle est **réservée à la documentation interne** et **n’est pas destinée aux utilisateurs finaux**.  
> ⚠️ **N’utilisez pas ces fonctions directement** sauf si vous développez une extension du moteur.

### Initialisation

```cpp
AssetManager::InitAssetManager();
```

Initialise le tableau d’assets intégrés et vide les pointeurs externes.

### Enregistrement des loaders

```cpp
AssetManager::SetupLoaders<MyType>(id);
```

Enregistre les fonctions `Load` / `Unload` pour un type donné, à un ID donné.

### Récupération du pointeur partagé

```cpp
MyType** ptr = AssetManager::Get<MyType>(id);
```

Renvoie un pointeur vers un pointeur (`T**`) pour un asset donné. Ce mécanisme permet la synchronisation automatique de toutes les instances.

### Référencement manuel

```cpp
AssetManager::IncrementRef(id);
AssetManager::DecrementRef(id);
```

Augmente ou diminue manuellement le compteur d’utilisation d’un asset.