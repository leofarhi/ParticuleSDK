# 🚀 Classe `App`

La classe `App` représente l'application principale dans Particule SDK. Elle centralise la gestion des fenêtres, du temps et du cycle de vie global de l'application.

> 📌 **Important** : La fonction `MainApp(App* app, int argc, char* argv[])` remplace `main` sur toutes les plateformes. Elle est appelée **une seule fois au démarrage** et permet de configurer les fenêtres et ressources de l'application.

---

## 🧩 Fonction `MainApp`

```cpp
int MainApp(App* app, int argc, char* argv[]);
```

Cette fonction est automatiquement appelée au lancement. Elle permet d'initialiser l'application, de créer les fenêtres, et de lancer les éléments nécessaires.

### Exemple :

```cpp
int MainApp(App* app, int argc, char* argv[])
{
    BasicWindow* window = new BasicWindow(396*2, 224*2, "Titre");
    window->SetResizable(true);
    app->AddWindow(window);
    return EXIT_SUCCESS;
}
```

---

> ℹ️ **Remarque**
> La fonction `MainApp(App* app, int argc, char* argv[])` est appelée **automatiquement au démarrage du programme**, avant toute initialisation du système de rendu ou des fenêtres.
> L’application **ne démarre que si** cette fonction retourne `EXIT_SUCCESS`.
> Elle sert à configurer les fenêtres de l’application en appelant `app->AddWindow(...)`, et à effectuer toute initialisation personnalisée nécessaire.


---

## 🧠 Attributs statiques

* `App::instance` : Instance unique de l'application.
* `App::time` : Gestion globale du temps.

---

## ⚙️ Fonctions principales

### `App()`

!> ⚠️ Cette fonction est appelée automatiquement par le moteur. Vous ne devez **jamais** créer vous-même une instance de `App`.

### `~App()`

!> ⚠️ Ne pas appeler manuellement. La destruction de l'application est gérée automatiquement par le moteur.

### `void Update()`

!> ⚠️ Cette fonction est appelée automatiquement par le moteur à chaque frame.  
 Vous ne devez **pas** l'appeler manuellement, sauf cas très spécifique.

### `int FPS()`

Retourne le nombre d'images par seconde (FPS).

---

## 📁 Répertoire de travail

### `void ResetWorkingDirectory()`

Réinitialise le répertoire de travail au dossier de départ de l'application.

> Cette opération est **automatiquement effectuée au démarrage**, mais peut être relancée manuellement si besoin.

### `void SetWorkingDirectory(std::string& path)`

Permet de changer le répertoire de travail manuellement.

> ⚠️ Cette méthode est **déconseillée**, car les assets sont chargés relativement au répertoire de travail initial. Un changement peut rendre les assets inaccessibles sans reset.

### `std::string GetWorkingDirectory()`

Retourne le chemin du répertoire de travail actuel.

---

## 🪟 Fenêtres

### `void AddWindow(Window* window)`

Ajoute une fenêtre à l'application. Elle sera mise à jour et affichée à chaque frame.

### `void RemoveWindow(Window* window)`

Supprime une fenêtre précédemment ajoutée.

### `std::vector<Window*> GetWindows()`

Retourne la liste des fenêtres actuellement ajoutées à l'application.

### `Window* GetWindow(size_t index)`

Accède à une fenêtre spécifique selon son index dans la liste.

---

## 🌟 Fenêtre principale

### `static void SetMainWindow(Window* window)`

Définit la fenêtre principale (celle qui gère les entrées et certains comportements globaux).

### `static Window* GetMainWindow()`

Récupère la fenêtre principale.

---

## 🖼️ Icône de l'application

### `void SetIcon(std::string path)`

Permet théoriquement de définir une icône pour l'application.

> ℹ️ **Remarque** : Cette fonctionnalité n'est **pas encore disponible** sur toutes les plateformes. Elle est ignorée actuellement.

---
