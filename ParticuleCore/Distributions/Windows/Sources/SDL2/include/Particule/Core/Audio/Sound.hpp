#ifndef SOUND_HPP
#define SOUND_HPP
#include <string>
#include <Particule/Core/Audio/Audio.hpp>
#include <Particule/Core/System/AssetManager.hpp>
#include <Particule/Core/System/sdl2.hpp>

namespace Particule::Core
{
    class Sound
    {
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

    private:
        Asset<Audio> audio; // Référence à l'Audio sous-jacent
        // Fenêtre (en secondes)
        Seconds                    _regionStartSec { Seconds{0} };
        std::optional<Seconds>     _regionLenSec   { std::nullopt };

        // Fenêtre (en frames absolues dans l'Audio)
        sdl2::Uint64 _regionStartFrames { 0 };
        sdl2::Uint64 _regionEndFrames   { 0 }; // exclusif

        // Lecture locale
        Audio::State   _state   { Audio::State::Stopped };
        bool           _loop    { false };
        float          _volume  { 1.0f }; // [0..1]
        float          _pitch   { 1.0f }; // > 0
        double         _cursorFrames { 0.0 }; // position relative à la région (0..RegionLengthFrames)

        // Device audio propre à ce Sound
        sdl2::SDL_AudioDeviceID _device { 0 };
        sdl2::SDL_AudioSpec     _devSpec{};

        // Verrou (callback <-> API)
        sdl2::SDL_SpinLock _lock { 0 };

        // Méthodes internes
        static void SDLCALL _AudioCallback(void* userdata, sdl2::Uint8* stream, int len) noexcept;
        void _FillStream(sdl2::Uint8* out, int len) noexcept;

        void _OpenDeviceOrThrow();
        void _CloseDevice() noexcept;

        void _RecomputeRegion() noexcept;
        void _ResetCursor() noexcept;
        void _EnsurePitchValid() noexcept;
    };

}
#endif // SOUND_HPP