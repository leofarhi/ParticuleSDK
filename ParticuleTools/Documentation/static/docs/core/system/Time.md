# ⏱️ Gestion du Temps (`Time` et `Timer`)

Le module `Time` du Particule SDK fournit des outils pour suivre le temps d'exécution, mesurer le temps écoulé entre les frames, et effectuer des temporisations précises.  
Il est utilisé pour animer des objets, réguler les FPS, ou déclencher des événements temporisés.

---

## 🕒 Classe `Time`

### Accès global
L'instance principale de `Time` est accessible via `App::time` :

```cpp
uint32_t delta = App::time.DeltaTime();
```

---

### `Time()`

Crée une instance de suivi du temps.

### `void Update()`

Met à jour les valeurs internes du temps. Cette fonction est appelée automatiquement à chaque frame.  
> ⚠️ Vous **n’avez pas besoin** de l’appeler manuellement (pour l'instance principale).

---

### `uint32_t DeltaTime()`

Renvoie le temps écoulé depuis la dernière mise à jour (`Update`), en **microsecondes**.

---

### `uint32_t TimeSinceStart()`

Renvoie le temps total écoulé depuis la création de l’objet `Time`, en **microsecondes**.

---

### Temporisations

| Méthode | Description |
|--------|-------------|
| `Delay(double time)` | Fait une pause pendant un certain temps (en **secondes**) |
| `DelayMs(uint32_t ms)` | Pause en **millisecondes** |
| `DelayUs(uint32_t us)` | Pause en **microsecondes** |

---

## ⏳ Classe `Timer`

Un minuteur indépendant utile pour créer des délais ou des cooldowns.

---

### `Timer()`

Crée un minuteur désactivé.

---

### `void start(uint32_t duration_us)`

Lance le minuteur pour une durée donnée (en microsecondes).

---

### `void reset()`

Réinitialise le minuteur, sans le relancer.

---

### `bool isFinished()`

Renvoie `true` si la durée est écoulée.

---

### `bool isRunning()`

Renvoie `true` si le minuteur est en cours.

---

### `uint32_t timeLeft()`

Renvoie le temps restant (en microsecondes) avant la fin du minuteur.

---

## 📌 Exemple d’utilisation

```cpp
Timer t;
t.start(500000); // 500ms

if (t.isFinished()) {
    // exécuter une action une fois le temps écoulé
}
```

> ℹ️ **Remarque** : Il est **déconseillé d’utiliser une boucle `while` bloquante**, car `OnUpdate` est appelée automatiquement à chaque cycle du moteur.  
> Préférez utiliser un `if` dans `OnUpdate`, une fonction asynchrone ou une coroutine si vous souhaitez attendre un temps sans bloquer l’application.