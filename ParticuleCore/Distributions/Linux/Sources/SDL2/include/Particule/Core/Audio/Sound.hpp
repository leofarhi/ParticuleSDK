#ifndef SOUND_HPP
#define SOUND_HPP
#include <string>

namespace Particule::Core
{
    class Sound
    {
    public:
        Sound();
        Sound(const Sound& other);
        Sound& operator=(const Sound& other);
        ~Sound();

        void Play();
        void Pause();
        void Stop();
        void SetVolume(float volume);
        void SetPitch(float pitch);
        void SetLooping(bool loop);
        void SetPlaybackPosition(float time);
        
        float GetVolume();
        float GetPitch();
        float GetPosition();
        float GetDuration();

        bool IsPlaying();
        bool IsPaused();
        bool IsStopped();
        bool IsLooping();
    };
}
#endif // SOUND_HPP