#ifndef TIME_HPP
#define TIME_HPP

#include <ctime>
#include <cstdint>
#include <chrono>
#include <thread>

namespace Particule::Core
{
    class Time
    {
    private:
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;

        TimePoint m_startTime;
        TimePoint m_lastTime;
        uint32_t m_deltaTimeUs;
    public:
        Time();
        Time(const Time& other);
        Time& operator=(const Time& other);
        ~Time();

        void Update();

        uint32_t DeltaTime();        // en microsecondes
        uint32_t TimeSinceStart();   // en microsecondes

        static void Delay(double time);        // en secondes
        static void DelayMs(uint32_t ms);      // en millisecondes
        static void DelayUs(uint32_t us);      // en microsecondes
    };


    class Timer
    {
    private:
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;

        TimePoint startTime;
        uint32_t durationUs;
        bool active;
    public:
        Timer();
        void start(uint32_t duration_us); // en microsecondes
        void reset();
        bool isFinished();
        bool isRunning();
        uint32_t timeLeft();              // en microsecondes
    };
}

#endif // TIME_HPP