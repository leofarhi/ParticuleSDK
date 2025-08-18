#ifndef SOUND_HPP
#define SOUND_HPP
#include <string>
#include <Particule/Core/Audio/Audio.hpp>
#include <Particule/Core/System/AssetManager.hpp>

namespace Particule::Core
{
    class Sound
    {
    private:
        Asset<Audio> audio; // Référence à l'Audio sous-jacent
    public:
        // Construction : fenêtre optionnelle [start, length] relative à l'Audio
        Sound() noexcept = default;
        
        Sound(Asset<Audio> asset_audio,
              Seconds start = Seconds{0},
              std::optional<Seconds> length = std::nullopt) noexcept;

        Sound(Audio* asset_audio,
              Seconds start = Seconds{0},
              std::optional<Seconds> length = std::nullopt) noexcept;

        Sound(uint32_t assetID,
              Seconds start = Seconds{0},
              std::optional<Seconds> length = std::nullopt) noexcept;

        // Copiable (duplique l’instance de lecture), déplaçable
        Sound(const Sound&);
        Sound& operator=(const Sound&);
        Sound(Sound&&) noexcept;
        Sound& operator=(Sound&&) noexcept;

        ~Sound();

        // Contrôles de lecture sur la fenêtre
        void Play()  noexcept;
        void Pause() noexcept;
        void Stop()  noexcept;

        void SetLooping(bool loop) noexcept;
        [[nodiscard]] bool IsLooping() const noexcept;

        // Fenêtre locale (début & durée) relative à l'Audio
        void SetRegion(Seconds start, std::optional<Seconds> length = std::nullopt) noexcept;
        [[nodiscard]] Seconds   RegionStart()    const noexcept;
        [[nodiscard]] Seconds   RegionDuration() const noexcept;

        // Positionnement/état RELATIFS à la région
        void SetPlaybackPosition(Seconds positionInRegion) noexcept;
        [[nodiscard]] Seconds GetPosition() const noexcept;  // [0..RegionDuration]
        [[nodiscard]] Seconds GetDuration() const noexcept;  // durée de la région

        // Paramètres locaux (indépendants de l'Audio ou des autres Sound)
        void SetVolume(float volume01) noexcept; // [0..1]
        void SetPitch(float pitch)     noexcept; // ratio > 0

        [[nodiscard]] float GetVolume() const noexcept;
        [[nodiscard]] float GetPitch()  const noexcept;

        [[nodiscard]] Audio::State GetState() const noexcept;

        // Accès à la ressource sous‑jacente
        [[nodiscard]] Audio* GetAudio() const noexcept;
    };

}
#endif // SOUND_HPP