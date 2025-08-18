#include <Particule/Core/Audio/Audio.hpp>
#include <Particule/Core/Audio/Sound.hpp>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <optional>
#include <stdexcept>

using namespace Particule::Core;
using namespace sdl2;

static inline double toSeconds(Uint64 frames, int sampleRate) noexcept {
    return (sampleRate > 0) ? (double(frames) / double(sampleRate)) : 0.0;
}

static inline Uint64 toFrames(Seconds s, int sampleRate) noexcept {
    const double sec = s.count();
    if (sampleRate <= 0) return 0;
    const double f = std::max(0.0, sec * double(sampleRate));
    return static_cast<Uint64>(f + 0.5);
}

static inline int16_t clampS16(int v) noexcept {
    if (v >  32767) return  32767;
    if (v < -32768) return -32768;
    return (int16_t)v;
}

Sound::Sound(Asset<Audio> asset_audio,
             Seconds start,
             std::optional<Seconds> length) noexcept
    : audio(std::move(asset_audio))
    , _regionStartSec(start)
    , _regionLenSec(length)
{
    try {
        _RecomputeRegion();
        _OpenDeviceOrThrow();
    } catch (...) {
        _CloseDevice();
        _state = Audio::State::Stopped;
    }
}

Sound::Sound(Audio* asset_audio,
             Seconds start,
             std::optional<Seconds> length) noexcept
    : audio(asset_audio)
    , _regionStartSec(start)
    , _regionLenSec(length)
{
    try {
        _RecomputeRegion();
        _OpenDeviceOrThrow();
    } catch (...) {
        _CloseDevice();
        _state = Audio::State::Stopped;
    }
}

Sound::Sound(uint32_t assetID,
             Seconds start,
             std::optional<Seconds> length) noexcept
    : audio(assetID)
    , _regionStartSec(start)
    , _regionLenSec(length)
{
    try {
        _RecomputeRegion();
        _OpenDeviceOrThrow();
    } catch (...) {
        _CloseDevice();
        _state = Audio::State::Stopped;
    }
}

Sound::Sound(const Sound& other)
{
    audio             = other.audio;
    _regionStartSec   = other._regionStartSec;
    _regionLenSec     = other._regionLenSec;
    _regionStartFrames= other._regionStartFrames;
    _regionEndFrames  = other._regionEndFrames;
    _loop             = other._loop;
    _volume           = other._volume;
    _pitch            = other._pitch;
    _cursorFrames     = other._cursorFrames;
    _state            = Audio::State::Stopped; // on démarre à l'arrêt pour éviter double lecture

    try {
        if (audio.IsValid()) _OpenDeviceOrThrow();
    } catch (...) {
        _CloseDevice();
        _state = Audio::State::Stopped;
    }
}

Sound& Sound::operator=(const Sound& other)
{
    if (this == &other) return *this;

    _CloseDevice();

    audio              = other.audio;
    _regionStartSec    = other._regionStartSec;
    _regionLenSec      = other._regionLenSec;
    _regionStartFrames = other._regionStartFrames;
    _regionEndFrames   = other._regionEndFrames;
    _loop              = other._loop;
    _volume            = other._volume;
    _pitch             = other._pitch;
    _cursorFrames      = other._cursorFrames;
    _state             = Audio::State::Stopped;

    try {
        if (audio.IsValid()) _OpenDeviceOrThrow();
    } catch (...) {
        _CloseDevice();
        _state = Audio::State::Stopped;
    }
    return *this;
}

Sound::Sound(Sound&& other) noexcept
{
    audio               = std::move(other.audio);
    _regionStartSec     = other._regionStartSec;
    _regionLenSec       = other._regionLenSec;
    _regionStartFrames  = other._regionStartFrames;
    _regionEndFrames    = other._regionEndFrames;
    _loop               = other._loop;
    _volume             = other._volume;
    _pitch              = other._pitch;
    _cursorFrames       = other._cursorFrames;
    _state              = other._state;
    _device             = other._device;      other._device = 0;
    _devSpec            = other._devSpec;
    _lock               = 0; // réinitialiser le spinlock
}

Sound& Sound::operator=(Sound&& other) noexcept
{
    if (this == &other) return *this;

    _CloseDevice();

    audio               = std::move(other.audio);
    _regionStartSec     = other._regionStartSec;
    _regionLenSec       = other._regionLenSec;
    _regionStartFrames  = other._regionStartFrames;
    _regionEndFrames    = other._regionEndFrames;
    _loop               = other._loop;
    _volume             = other._volume;
    _pitch              = other._pitch;
    _cursorFrames       = other._cursorFrames;
    _state              = other._state;
    _device             = other._device;      other._device = 0;
    _devSpec            = other._devSpec;
    _lock               = 0;

    return *this;
}

Sound::~Sound() {
    _CloseDevice();
}

// --- Contrôles ---

void Sound::Play() noexcept {
    if (!audio.IsValid() || _device == 0) return;
    _EnsurePitchValid();
    _state = Audio::State::Playing;
    SDL_PauseAudioDevice(_device, 0);
}

void Sound::Pause() noexcept {
    if (_device == 0) return;
    _state = Audio::State::Paused;
    SDL_PauseAudioDevice(_device, 1);
}

void Sound::Stop() noexcept {
    if (_device == 0) return;
    _state = Audio::State::Stopped;
    _ResetCursor();
    SDL_PauseAudioDevice(_device, 1);
}

void Sound::SetLooping(bool loop) noexcept { _loop = loop; }
bool Sound::IsLooping() const noexcept { return _loop; }

// --- Région ---

void Sound::SetRegion(Seconds start, std::optional<Seconds> length) noexcept {
    _regionStartSec = start;
    _regionLenSec   = length;
    _RecomputeRegion();
    _ResetCursor();
}

Seconds Sound::RegionStart() const noexcept { return _regionStartSec; }

Seconds Sound::RegionDuration() const noexcept {
    if (!audio.IsValid()) return Seconds{0};
    const int sr = audio->SampleRate();
    if (sr <= 0) return Seconds{0};
    const Uint64 lenFrames = (_regionEndFrames > _regionStartFrames)
                           ? (_regionEndFrames - _regionStartFrames)
                           : 0;
    return Seconds{ double(lenFrames) / double(sr) };
}

// --- Position relative à la région ---

void Sound::SetPlaybackPosition(Seconds positionInRegion) noexcept {
    if (!audio.IsValid()) return;
    const int sr = audio->SampleRate();
    if (sr <= 0) return;

    const double frames = std::max(0.0, positionInRegion.count() * double(sr));
    const Uint64 maxFrames = (_regionEndFrames > _regionStartFrames)
                           ? (_regionEndFrames - _regionStartFrames)
                           : 0;
    SDL_AtomicLock(&_lock);
    _cursorFrames = std::min<double>(frames, double(maxFrames));
    SDL_AtomicUnlock(&_lock);
}

Seconds Sound::GetPosition() const noexcept {
    if (!audio.IsValid()) return Seconds{0};
    const int sr = audio->SampleRate();
    if (sr <= 0) return Seconds{0};

    double cur;
    SDL_AtomicLock(const_cast<SDL_SpinLock*>(&_lock));
    cur = _cursorFrames;
    SDL_AtomicUnlock(const_cast<SDL_SpinLock*>(&_lock));

    return Seconds{ cur / double(sr) };
}

Seconds Sound::GetDuration() const noexcept {
    return RegionDuration();
}

// --- Paramètres locaux ---

void Sound::SetVolume(float volume01) noexcept {
    if (!std::isfinite(volume01)) return;
    _volume = std::clamp(volume01, 0.0f, 1.0f);
}

void Sound::SetPitch(float pitch) noexcept {
    _pitch = pitch;
    _EnsurePitchValid();
}

float Sound::GetVolume() const noexcept { return _volume; }
float Sound::GetPitch()  const noexcept { return _pitch;  }

Audio::State Sound::GetState() const noexcept { return _state; }

// --- Accès à la ressource ---

Audio* Sound::GetAudio() const noexcept {
    // Asset<Audio> agit comme un pointeur ; s’il a operator->(), on s’appuie dessus :
    return audio.IsValid() ? audio.operator->() : nullptr;
}

// --------------------- Internes ---------------------

void Sound::_EnsurePitchValid() noexcept {
    if (!std::isfinite(_pitch) || _pitch <= 0.0001f) _pitch = 0.0001f;
    if (_pitch > 8.0f) _pitch = 8.0f;
}

void Sound::_ResetCursor() noexcept {
    SDL_AtomicLock(&_lock);
    _cursorFrames = 0.0;
    SDL_AtomicUnlock(&_lock);
}

void Sound::_RecomputeRegion() noexcept {
    if (!audio.IsValid()) {
        _regionStartFrames = 0;
        _regionEndFrames   = 0;
        return;
    }

    const int    sr          = audio->SampleRate();
    const Uint64 totalFrames = audio->_totalFrames; // nécessite friend ou getter

    Uint64 startF = toFrames(_regionStartSec, sr);
    if (startF > totalFrames) startF = totalFrames;

    Uint64 lengthF = 0;
    if (_regionLenSec.has_value()) {
        lengthF = toFrames(*_regionLenSec, sr);
        // clamp pour ne pas dépasser la fin
        if (startF + lengthF > totalFrames) lengthF = (totalFrames - startF);
    } else {
        lengthF = (totalFrames - startF);
    }

    _regionStartFrames = startF;
    _regionEndFrames   = startF + lengthF; // exclusif
}

void Sound::_OpenDeviceOrThrow() {
    if (!audio.IsValid()) return;

    if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
            throw std::runtime_error(std::string("SDL_INIT_AUDIO failed: ") + SDL_GetError());
        }
    }

    SDL_AudioSpec want{};
    want.freq     = audio->_srcSpec.freq;     // même fréquence que la source
    want.format   = AUDIO_S16;                // sortie en S16 interleaved
    want.channels = audio->_srcSpec.channels; // mêmes canaux
    want.samples  = 2048;
    want.callback = &Sound::_AudioCallback;
    want.userdata = this;

    SDL_AudioSpec have{};
    _device = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if (_device == 0) {
        throw std::runtime_error(std::string("SDL_OpenAudioDevice failed: ") + SDL_GetError());
    }

    _devSpec = have;

    // Pausé par défaut
    SDL_PauseAudioDevice(_device, 1);
}

void Sound::_CloseDevice() noexcept {
    if (_device != 0) {
        SDL_CloseAudioDevice(_device);
        _device = 0;
    }
}

void SDLCALL Sound::_AudioCallback(void* userdata, Uint8* stream, int len) noexcept {
    static_cast<Sound*>(userdata)->_FillStream(stream, len);
}

void Sound::_FillStream(Uint8* out, int len) noexcept {
    // Sortie device: S16 interleaved
    std::memset(out, 0, len);
    if (!audio.IsValid()) return;
    if (_state != Audio::State::Playing) return;
    if (_devSpec.format != AUDIO_S16) return;

    // Source : on suppose WAV décodé en S16 (cf. impl Audio), interleaved
    const Uint16 srcBps = SDL_AUDIO_BITSIZE(audio->_srcSpec.format) / 8;
    if (srcBps != 2) {
        // Pour simplicité, gérer uniquement S16 source ici.
        return;
    }

    const int channels = _devSpec.channels;
    const int outSamples = len / int(sizeof(int16_t));
    if (channels <= 0 || outSamples <= 0) return;

    const Uint8*  raw      = audio->_wavBuffer;     // buffer source
    const Uint32  bpf      = audio->_bytesPerFrame; // bytes per frame
    const Uint64  startF   = _regionStartFrames;
    const Uint64  endF     = _regionEndFrames;

    if (!raw || bpf == 0 || endF <= startF) return;

    // Position courante RELATIVE à la région
    SDL_AtomicLock(&_lock);
    double cursorRel = _cursorFrames;
    const double pitch = _pitch;
    const float  vol   = _volume;
    SDL_AtomicUnlock(&_lock);

    int framesToWrite = outSamples / channels;
    int16_t* dst = reinterpret_cast<int16_t*>(out);

    while (framesToWrite-- > 0) {
        // Frame absolue dans l'audio
        const double absFrameD = double(startF) + cursorRel;

        if (absFrameD >= double(endF)) {
            if (_loop) {
                const double regionLenF = double(endF - startF);
                if (regionLenF <= 0.0) break;
                cursorRel = std::fmod(cursorRel, regionLenF);
            } else {
                _state = Audio::State::Stopped;
                _cursorFrames = 0.0;
                break;
            }
        }

        Uint64 i0 = static_cast<Uint64>(absFrameD);
        Uint64 i1 = i0 + 1;
        const double frac = absFrameD - double(i0);

        // Clamp dans la région
        if (i0 >= endF) i0 = endF - 1;
        if (i1 >= endF) i1 = endF - 1;

        const int16_t* f0 = reinterpret_cast<const int16_t*>(raw + i0 * bpf);
        const int16_t* f1 = reinterpret_cast<const int16_t*>(raw + i1 * bpf);

        // Interpolation + volume (par canal)
        for (int c = 0; c < channels; ++c) {
            const int s0 = f0[c];
            const int s1 = f1[c];
            const double s = (1.0 - frac) * double(s0) + frac * double(s1);
            const int scaled = int(std::lround(s * double(vol)));
            *dst++ = clampS16(scaled);
        }

        cursorRel += pitch; // avance au rythme du pitch
    }

    // MàJ position relative
    SDL_AtomicLock(&_lock);
    _cursorFrames = cursorRel;
    SDL_AtomicUnlock(&_lock);
}