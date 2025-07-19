#ifndef TIME_HPP
#define TIME_HPP

#include <ctime>
#include <cstdint>

namespace Particule::Core
{
    class Time
    {
    public:
        Time();
        Time(const Time& other);
        Time& operator=(const Time& other);
        ~Time();

        void Update();

        uint32_t DeltaTime();        // en microsecondes
        uint32_t TimeSinceStart();   // en microsecondes

        static void Delay(double time);  // en secondes
        static void DelayMs(uint32_t ms);     // en millisecondes
        static void DelayUs(uint32_t us);     // en microsecondes
    };

    class Timer
    {
    public:
        Timer();
        void start(uint32_t duration_us);
        void reset();
        bool isFinished();
        bool isRunning();
        uint32_t timeLeft(); // en microsecondes
    };
}

#endif // TIME_HPP