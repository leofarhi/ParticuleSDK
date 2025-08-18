#ifndef SOUND_HPP
#define SOUND_HPP

#include <string>
#include <optional>
#include <utility>
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
              Seconds /*start*/ = Seconds{0},
              std::optional<Seconds> /*length*/ = std::nullopt) noexcept
            : audio(std::move(asset_audio)) {}

        Sound(Audio* asset_audio,
              Seconds /*start*/ = Seconds{0},
              std::optional<Seconds> /*length*/ = std::nullopt) noexcept
            : audio(asset_audio) {}

        Sound(uint32_t assetID,
              Seconds /*start*/ = Seconds{0},
              std::optional<Seconds> /*length*/ = std::nullopt) noexcept
            : audio(assetID) {}

        // Copiable (duplique l’instance de lecture), déplaçable
        Sound(const Sound& other)            = default;
        Sound& operator=(const Sound& other) = default;
        Sound(Sound&& other) noexcept        = default;
        Sound& operator=(Sound&& other) noexcept = default;

        ~Sound() = default;

        // Contrôles de lecture sur la fenêtre — no-op
        inline void Play()  noexcept {}
        inline void Pause() noexcept {}
        inline void Stop()  noexcept {}

        inline void SetLooping(bool /*loop*/) noexcept {}
        [[nodiscard]] inline bool IsLooping() const noexcept { return false; }

        // Fenêtre locale — no-op / retours par défaut
        inline void SetRegion(Seconds /*start*/, std::optional<Seconds> /*length*/ = std::nullopt) noexcept {}
        [[nodiscard]] inline Seconds   RegionStart()    const noexcept { return Seconds{0}; }
        [[nodiscard]] inline Seconds   RegionDuration() const noexcept { return Seconds{0}; }

        // Positionnement relatif — no-op / retours par défaut
        inline void SetPlaybackPosition(Seconds /*positionInRegion*/) noexcept {}
        [[nodiscard]] inline Seconds GetPosition() const noexcept { return Seconds{0}; }
        [[nodiscard]] inline Seconds GetDuration() const noexcept { return Seconds{0}; }

        // Paramètres locaux — no-op / valeurs par défaut
        inline void SetVolume(float /*volume01*/) noexcept {}
        inline void SetPitch(float /*pitch*/)     noexcept {}

        [[nodiscard]] inline float GetVolume() const noexcept { return 1.0f; }
        [[nodiscard]] inline float GetPitch()  const noexcept { return 1.0f; }

        [[nodiscard]] inline Audio::State GetState() const noexcept { return Audio::State::Stopped; }

        // Accès à la ressource sous-jacente
        [[nodiscard]] inline Audio* GetAudio() const noexcept {
            // Asset<Audio> se comporte comme un pointeur (audio->...).
            // On renvoie simplement son pointeur brut ; selon l'implémentation,
            // operator-> peut retourner nullptr si l'asset est vide.
            return audio.operator->();
        }
    };

    inline Sound Audio::CreateSound(Seconds start, std::optional<Seconds> length) const noexcept {
        (void)start;
        (void)length;
        return Sound();
    }

} // namespace Particule::Core

#endif // SOUND_HPP
