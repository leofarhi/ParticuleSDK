#include <Particule/Core/Audio/Audio.hpp>
#include <Particule/Core/Audio/Sound.hpp>
#include <stdexcept>
#include <cmath>
#include <cstring> 

using namespace Particule::Core;
using namespace sdl2;

static inline double toSeconds(Uint64 frames, int sampleRate) noexcept {
    return (sampleRate > 0) ? (double(frames) / double(sampleRate)) : 0.0;
}

Audio::Audio() noexcept = default;

Audio::~Audio() {
    _CloseDevice();
    if (_wavBuffer) {
        SDL_FreeWAV(_wavBuffer);
        _wavBuffer = nullptr;
    }
}

Audio::Audio(Audio&& other) noexcept {
    *this = std::move(other);
}

Audio& Audio::operator=(Audio&& other) noexcept {
    if (this == &other) return *this;

    // Fermer ressources actuelles
    _CloseDevice();
    if (_wavBuffer) {
        SDL_FreeWAV(_wavBuffer);
        _wavBuffer = nullptr;
    }

    // Move trivials
    _state         = other._state;
    _loop          = other._loop;
    _volume        = other._volume;
    _pitch         = other._pitch;
    _srcSpec       = other._srcSpec;
    _wavBuffer     = other._wavBuffer;    other._wavBuffer = nullptr;
    _wavLength     = other._wavLength;    other._wavLength = 0;
    _device        = other._device;       other._device = 0;
    _devSpec       = other._devSpec;
    _cursorFrames  = other._cursorFrames;
    _bytesPerFrame = other._bytesPerFrame;
    _totalFrames   = other._totalFrames;
    _lock          = 0; // spinlock remis à zéro

    return *this;
}

Audio* Audio::Load(std::string path) {
    if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
            throw std::runtime_error(std::string("SDL_INIT_AUDIO failed: ") + SDL_GetError());
        }
    }

    SDL_AudioSpec spec{};
    Uint8* buf = nullptr;
    Uint32 len = 0;
    if (!SDL_LoadWAV(path.c_str(), &spec, &buf, &len)) {
        throw std::runtime_error(std::string("SDL_LoadWAV failed: ") + SDL_GetError());
    }

    // Créer l’objet
    Audio* a = new Audio();
    a->_srcSpec   = spec;
    a->_wavBuffer = buf;
    a->_wavLength = len;

    // Calculs dérivés
    const Uint16 bytesPerSample = SDL_AUDIO_BITSIZE(spec.format) / 8;
    a->_bytesPerFrame = Uint32(bytesPerSample) * Uint32(spec.channels);
    if (a->_bytesPerFrame == 0) {
        Audio::Unload(a);
        throw std::runtime_error("Invalid audio frame size.");
    }
    a->_totalFrames = len / a->_bytesPerFrame;
    a->_cursorFrames = 0.0;
    a->_volume = 1.0f;
    a->_pitch = 1.0f;
    a->_loop = false;
    a->_state = State::Stopped;

    // Ouvrir device
    a->_OpenDeviceOrThrow();
    return a;
}

void Audio::Unload(Audio* audio) {
    delete audio;
}

void Audio::_OpenDeviceOrThrow() {
    SDL_AudioSpec want{};
    want.freq     = _srcSpec.freq;
    want.format   = AUDIO_S16;         // on normalise en S16 pour simplicité
    want.channels = _srcSpec.channels; // même nb de canaux que la source
    want.samples  = 2048;              // buffer du callback
    want.callback = &Audio::_AudioCallback;
    want.userdata = this;

    SDL_AudioSpec have{};
    _device = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if (_device == 0) {
        throw std::runtime_error(std::string("SDL_OpenAudioDevice failed: ") + SDL_GetError());
    }

    _devSpec = have;

    // Si le device n'est pas en S16/channels attendus, on pourrait convertir
    // à la volée dans _FillStream (ici on suppose match ou we adapt below).
    // Pour robustesse, on force le traitement en S16, et on mixe au format device si différent.
    if (_devSpec.format != AUDIO_S16) {
        // On ferme et ré-ouvre en S16 si possible
        SDL_CloseAudioDevice(_device);
        want.format = AUDIO_S16;
        _device = SDL_OpenAudioDevice(nullptr, 0, &want, &_devSpec, 0);
        if (_device == 0)
            throw std::runtime_error(std::string("SDL_OpenAudioDevice (S16) failed: ") + SDL_GetError());
    }

    // Démarrer en pause
    SDL_PauseAudioDevice(_device, 1);
}

void Audio::_CloseDevice() noexcept {
    if (_device != 0) {
        SDL_CloseAudioDevice(_device);
        _device = 0;
    }
}

void Audio::_ResetCursor() noexcept {
    SDL_AtomicLock(&_lock);
    _cursorFrames = 0.0;
    SDL_AtomicUnlock(&_lock);
}

void Audio::_EnsurePitchValid() noexcept {
    if (!std::isfinite(_pitch) || _pitch <= 0.0001f) _pitch = 0.0001f;
    if (_pitch > 8.0f) _pitch = 8.0f; // garde-fou
}

void SDLCALL Audio::_AudioCallback(void* userdata, Uint8* stream, int len) noexcept {
    static_cast<Audio*>(userdata)->_FillStream(stream, len);
}

void Audio::_FillStream(Uint8* out, int len) noexcept {
    // Sortie device supposée en S16 interleaved
    std::memset(out, 0, len);
    if (_state != State::Playing || !_wavBuffer || _devSpec.format != AUDIO_S16)
        return;

    const int outSamples = len / sizeof(int16_t); // nombre d'échantillons (tous canaux confondus)
    const int channels = _devSpec.channels;
    if (channels <= 0) return;

    const Uint16 srcBps = SDL_AUDIO_BITSIZE(_srcSpec.format) / 8;
    if (srcBps != 2) {
        // Pour simplicité, on gère ici seulement source S16. Si WAV n'est pas S16, on convertit à la volée.
        // Conversion minimale: utiliser SDL_AudioCVT (deprecated) ou SDL_AudioStream pour la source.
        // Ici, fallback silencieux (ou on pourrait passer en Stop).
        return;
    }

    const int16_t* src = reinterpret_cast<const int16_t*>(_wavBuffer);
    const Uint64 totalFrames = _totalFrames;
    const Uint32 bytesPerFrame = _bytesPerFrame;
    const int srcChannels = _srcSpec.channels;

    // Pas de conversion de canaux ici: on suppose channels == srcChannels
    if (srcChannels != channels) {
        // fallback silencieux si mismatch (à toi d’étendre pour convertir si besoin)
        return;
    }

    SDL_AtomicLock(&_lock);
    const double pitch = _pitch;
    const float volume = _volume;
    double cursor = _cursorFrames;
    SDL_AtomicUnlock(&_lock);

    if (!std::isfinite(cursor) || !std::isfinite(pitch) || pitch <= 0.0)
        return;

    int16_t* dst = reinterpret_cast<int16_t*>(out);
    int framesToWrite = outSamples / channels;

    while (framesToWrite-- > 0) {
        Uint64 i0 = (Uint64)cursor;
        Uint64 i1 = i0 + 1;

        if (i0 >= totalFrames) {
            if (_loop) {
                cursor = std::fmod(cursor, (double)totalFrames);
                i0 = (Uint64)cursor;
                i1 = i0 + 1;
                if (i0 >= totalFrames) {
                    // source trop courte
                    break;
                }
            } else {
                // Fin de piste
                _state = State::Stopped;
                _cursorFrames = 0.0;
                break;
            }
        }

        const double frac = cursor - double(i0);

        // Lecture interleaved: frame -> channels samples
        const int16_t* f0 = reinterpret_cast<const int16_t*>(
            reinterpret_cast<const Uint8*>(_wavBuffer) + i0 * bytesPerFrame);
        const int16_t* f1 = (i1 < totalFrames)
            ? reinterpret_cast<const int16_t*>(
                reinterpret_cast<const Uint8*>(_wavBuffer) + i1 * bytesPerFrame)
            : f0; // si on dépasse d’un frame, répéter f0 (évite lecture hors borne)

        // interpolation linéaire + volume
        for (int c = 0; c < channels; ++c) {
            const int s0 = f0[c];
            const int s1 = f1[c];
            const double s = (1.0 - frac) * double(s0) + frac * double(s1);
            const int scaled = int(std::lround(s * double(volume)));
            *dst++ = _ClampS16(scaled);
        }

        cursor += pitch; // avance selon le pitch
    }

    // Mettre à jour le curseur
    SDL_AtomicLock(&_lock);
    _cursorFrames = cursor;
    SDL_AtomicUnlock(&_lock);
}

// ----------- API publique -----------

void Audio::Play() noexcept {
    if (_device == 0) return;
    _EnsurePitchValid();
    _state = State::Playing;
    SDL_PauseAudioDevice(_device, 0);
}

void Audio::Pause() noexcept {
    if (_device == 0) return;
    _state = State::Paused;
    SDL_PauseAudioDevice(_device, 1);
}

void Audio::Stop() noexcept {
    if (_device == 0) return;
    _state = State::Stopped;
    _ResetCursor();
    SDL_PauseAudioDevice(_device, 1);
}

void Audio::SetLooping(bool loop) noexcept { _loop = loop; }
bool Audio::IsLooping() const noexcept { return _loop; }

void Audio::SetPlaybackPosition(Seconds position) noexcept {
    if (_srcSpec.freq <= 0) return;
    const double posSec = position.count();
    double frames = posSec * double(_srcSpec.freq);
    if (frames < 0.0) frames = 0.0;
    if ((Uint64)frames > _totalFrames) frames = double(_totalFrames);
    SDL_AtomicLock(&_lock);
    _cursorFrames = frames;
    SDL_AtomicUnlock(&_lock);
}

Seconds Audio::GetPosition() const noexcept {
    if (_srcSpec.freq <= 0) return Seconds{0};
    double frames;
    SDL_AtomicLock(const_cast<SDL_SpinLock*>(&_lock));
    frames = _cursorFrames;
    SDL_AtomicUnlock(const_cast<SDL_SpinLock*>(&_lock));
    return Seconds{ frames / double(_srcSpec.freq) };
}

Seconds Audio::GetDuration() const noexcept {
    if (_srcSpec.freq <= 0) return Seconds{0};
    return Seconds{ double(_totalFrames) / double(_srcSpec.freq) };
}

Seconds Audio::Duration() const noexcept {
    return GetDuration();
}

uint32_t Audio::SampleRate() const noexcept { return _srcSpec.freq; }
uint32_t Audio::ChannelCount() const noexcept { return _srcSpec.channels; }
bool     Audio::IsStreamed() const noexcept { return false; } // WAV entièrement en mémoire ici

void Audio::SetVolume(float volume01) noexcept {
    if (!std::isfinite(volume01)) return;
    if (volume01 < 0.0f) volume01 = 0.0f;
    if (volume01 > 1.0f) volume01 = 1.0f;
    _volume = volume01;
}

void Audio::SetPitch(float pitch) noexcept {
    _pitch = pitch;
    _EnsurePitchValid();
}

float Audio::GetVolume() const noexcept { return _volume; }
float Audio::GetPitch()  const noexcept { return _pitch;  }

Audio::State Audio::GetState() const noexcept { return _state; }


Sound Audio::CreateSound(Seconds start, std::optional<Seconds> length) const noexcept {
    return Sound(const_cast<Audio*>(this), start, std::move(length));
}
