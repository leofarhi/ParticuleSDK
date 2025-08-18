#ifndef AUDIO_HPP
#define AUDIO_HPP
#include <cstddef>
#include <cstdint>
#include <memory>
#include <chrono>

namespace Particule::Core {
    using Seconds = std::chrono::duration<double>;

    class Sound; // fwd

    class Audio
    {
    public:
        enum class State { Stopped, Playing, Paused };

        static Audio* Load(std::string path);
        static void Unload(Audio* audio);

        // Non copiable, déplaçable
        Audio(const Audio&) = delete;
        Audio& operator=(const Audio&) = delete;
        Audio(Audio&&) noexcept;
        Audio& operator=(Audio&&) noexcept;

        ~Audio();

        // Métadonnées (immutables)
        [[nodiscard]] Seconds   Duration()     const noexcept;
        [[nodiscard]] uint32_t  SampleRate()   const noexcept;
        [[nodiscard]] uint32_t  ChannelCount() const noexcept;
        [[nodiscard]] bool      IsStreamed()   const noexcept;

        // Contrôles de lecture "plein format" (sur l'intégralité de l'audio)
        void Play()  noexcept;
        void Pause() noexcept;
        void Stop()  noexcept;

        void SetLooping(bool loop) noexcept;
        [[nodiscard]] bool  IsLooping() const noexcept;

        void SetPlaybackPosition(Seconds position) noexcept; // relatif au média complet
        [[nodiscard]] Seconds GetPosition() const noexcept;  // relatif au média complet
        [[nodiscard]] Seconds GetDuration() const noexcept;  // alias de Duration()

        // Paramètres de lecture "globaux" à cette instance lecteur d'Audio
        void SetVolume(float volume01) noexcept; // [0..1]
        void SetPitch(float pitch)     noexcept; // ratio > 0

        [[nodiscard]] float GetVolume() const noexcept;
        [[nodiscard]] float GetPitch()  const noexcept;

        [[nodiscard]] State GetState() const noexcept;

        // Fabrique de sous‑lectures (fenêtres)
        [[nodiscard]] Sound CreateSound(Seconds start = Seconds{0},
                                        std::optional<Seconds> length = std::nullopt) const noexcept;

    private:
        Audio() noexcept; // construction via Load
    };
}

#endif // AUDIO_HPP