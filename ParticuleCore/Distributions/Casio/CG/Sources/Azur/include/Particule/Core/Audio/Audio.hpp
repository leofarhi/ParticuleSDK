#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <chrono>
#include <string>
#include <optional>

namespace Particule::Core {

    typedef double Seconds;

    class Sound; // fwd

    class Audio
    {
    public:
        enum class State { Stopped, Playing, Paused };

        // Stub: ne charge rien, renvoie toujours nullptr
        static inline Audio* Load(std::string /*path*/) { return nullptr; }

        // Stub: accepte nullptr sans broncher
        static inline void Unload(Audio* audio) { delete audio; }

        // Non copiable, déplaçable
        Audio(const Audio&) = delete;
        Audio& operator=(const Audio&) = delete;

        inline Audio(Audio&&) noexcept {}
        inline Audio& operator=(Audio&&) noexcept { return *this; }

        inline ~Audio() {}

        // Métadonnées (immutables) — valeurs par défaut
        [[nodiscard]] inline Seconds   Duration()     const noexcept { return Seconds{0}; }
        [[nodiscard]] inline uint32_t  SampleRate()   const noexcept { return 0; }
        [[nodiscard]] inline uint32_t  ChannelCount() const noexcept { return 0; }
        [[nodiscard]] inline bool      IsStreamed()   const noexcept { return false; }

        // Contrôles de lecture "plein format" — no-op
        inline void Play()  noexcept {}
        inline void Pause() noexcept {}
        inline void Stop()  noexcept {}

        inline void SetLooping(bool /*loop*/) noexcept {}
        [[nodiscard]] inline bool  IsLooping() const noexcept { return false; }

        inline void SetPlaybackPosition(Seconds /*position*/) noexcept {}
        [[nodiscard]] inline Seconds GetPosition() const noexcept { return Seconds{0}; }
        [[nodiscard]] inline Seconds GetDuration() const noexcept { return Seconds{0}; }

        // Paramètres locaux — no-op, valeurs par défaut
        inline void SetVolume(float /*volume01*/) noexcept {}
        inline void SetPitch(float /*pitch*/)     noexcept {}

        [[nodiscard]] inline float GetVolume() const noexcept { return 1.0f; }
        [[nodiscard]] inline float GetPitch()  const noexcept { return 1.0f; }

        [[nodiscard]] inline State GetState() const noexcept { return State::Stopped; }

        // Fabrique de sous-lectures (fenêtres)
        [[nodiscard]] Sound CreateSound(Seconds start = Seconds{0},
                                               std::optional<Seconds> length = std::nullopt) const noexcept;

    private:
        inline Audio() noexcept {} // construction via Load (stub: non utilisée)
    };

} // namespace Particule::Core

#endif // AUDIO_HPP
