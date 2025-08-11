#ifndef SOUND_HPP
#define SOUND_HPP
#include <string>

namespace Particule::Core
{
    class Sound
    {
    public:
        Sound() {};
        Sound(const Sound& other){ (void)other; }
        Sound& operator=(const Sound& other){ (void)other; return *this; }
        ~Sound(){};

        constexpr inline void Play(){};
        constexpr inline void Pause(){};
        constexpr inline void Stop(){};
        constexpr inline void SetVolume(float volume){ (void)volume; };
        constexpr inline void SetPitch(float pitch){ (void)pitch; };
        constexpr inline void SetLoop(bool loop){ (void)loop; };
        constexpr inline void SetPlaybackPosition(float time){ (void)time; };
        
        constexpr inline float GetVolume(){ return 0.0f; };
        constexpr inline float GetPitch(){ return 0.0f; };
        constexpr inline float GetPosition(){ return 0.0f; };
        constexpr inline float GetDuration(){ return 0.0f; };

        constexpr inline bool IsPlaying(){ return false; };
        constexpr inline bool IsPaused(){ return false; };
        constexpr inline bool IsStopped(){ return false; };
        constexpr inline bool IsLooping(){ return false; };
    };
}
#endif // SOUND_HPP