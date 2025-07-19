# 🎧 `Particule::Core::Sound`

!> **Statut** : ⚠️ Cette fonctionnalité est en cours de développement dans l’API.  
 Cependant, voici un aperçu complet de son interface et de ce qu’elle permettra de faire.

---

La classe `Sound` permet de gérer des sons dans une application avec des contrôles simples : lecture, pause, arrêt, volume, pitch, boucle, et navigation dans la piste.

---

## 📦 Constructeurs

### `Sound()`
> Crée une instance vide de son.  
> Le son devra être chargé via le système d'assets ou une méthode future `Load`.

---

### `Sound(const Sound& other)`
> Constructeur par copie. Copie une instance de son existante.

---

### `Sound& operator=(const Sound& other)`
> Affectation par copie entre deux objets `Sound`.

---

## 🛠️ Fonctions de contrôle

### `void Play()`
> Lance ou reprend la lecture du son.

### `void Pause()`
> Met la lecture en pause.

### `void Stop()`
> Arrête complètement la lecture et remet la position au début.

---

## 🎚️ Paramètres dynamiques

### `void SetVolume(float volume)`
> Définit le volume du son, de `0.0f` (muet) à `1.0f` (volume normal).  
> Valeurs supérieures à 1.0 sont autorisées mais peuvent provoquer de la saturation.

### `void SetPitch(float pitch)`
> Définit la hauteur (pitch) du son.  
> `1.0f` = vitesse normale.  
> `0.5f` = moitié vitesse / grave.  
> `2.0f` = double vitesse / aigu.

### `void SetLooping(bool loop)`
> Active ou désactive la lecture en boucle.  
> Lorsque la fin du son est atteinte, il recommencera s’il est en mode boucle.

### `void SetPlaybackPosition(float time)`
> Change la position de lecture dans le son (en secondes).

---

## 📈 Accès aux propriétés

### `float GetVolume()`
> Récupère le volume actuel.

### `float GetPitch()`
> Récupère la hauteur actuelle.

### `float GetPosition()`
> Donne la position de lecture actuelle (en secondes).

### `float GetDuration()`
> Donne la durée totale du son (en secondes).

---

## 🔍 États de lecture

### `bool IsPlaying()`
> Retourne `true` si le son est actuellement en cours de lecture.

### `bool IsPaused()`
> Retourne `true` si le son est en pause.

### `bool IsStopped()`
> Retourne `true` si le son est arrêté (lecture non active).

### `bool IsLooping()`
> Retourne `true` si le son est configuré pour se répéter en boucle.

---

## 📌 Notes

- Le chargement des fichiers audio ne fait **pas encore partie** de cette API, mais sera intégré via un système `Asset<Sound>` à l'avenir.

---

👉 Revenez consulter cette page après les prochaines mises à jour pour accéder aux exemples d’utilisation et fonctions avancées.
