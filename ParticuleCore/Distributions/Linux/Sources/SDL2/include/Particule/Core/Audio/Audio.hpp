#ifndef AUDIO_HPP
#define AUDIO_HPP
#include <cstddef>
#include <cstdint>
#include <memory>
#include <chrono>
#include <Particule/Core/System/sdl2.hpp>

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
        friend class Sound;
        Audio() noexcept; // construction via Load

        struct SdlDeleter {
            void operator()(sdl2::SDL_AudioStream* s) const noexcept { if (s) sdl2::SDL_FreeAudioStream(s); }
        };

        // État / paramètres
        State      _state { State::Stopped };
        bool       _loop  { false };
        float      _volume{ 1.0f };   // [0..1]
        float      _pitch { 1.0f };   // >0

        // Source décodée (WAV)
        sdl2::SDL_AudioSpec _srcSpec{};     // format de la source
        sdl2::Uint8*        _wavBuffer{nullptr};
        sdl2::Uint32        _wavLength{0};  // en octets

        // Lecture
        sdl2::SDL_AudioDeviceID _device{0};
        sdl2::SDL_AudioSpec     _devSpec{}; // format du device
        double            _cursorFrames{0.0}; // position de lecture (en frames, donc "échantillon par canal")
        sdl2::Uint32            _bytesPerFrame{0};  // sizeof(sample) * channels
        sdl2::Uint64            _totalFrames{0};    // longueur totale en frames

        // Protection thread callback / API
        sdl2::SDL_SpinLock _lock{0};

        static void SDLCALL _AudioCallback(void* userdata, sdl2::Uint8* stream, int len) noexcept;
        void _FillStream(sdl2::Uint8* stream, int len) noexcept;

        // Utilitaires
        static inline int16_t _ClampS16(int v) noexcept {
            if (v >  32767) return  32767;
            if (v < -32768) return -32768;
            return (int16_t)v;
        }

        void _OpenDeviceOrThrow();
        void _CloseDevice() noexcept;
        void _ResetCursor() noexcept;
        void _EnsurePitchValid() noexcept;
    };
}

#endif // AUDIO_HPP