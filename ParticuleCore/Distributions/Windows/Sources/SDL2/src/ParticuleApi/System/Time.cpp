#include <Particule/Core/System/Time.hpp>

namespace Particule::Core
{
    Time::Time()
        : m_startTime(Clock::now()), m_lastTime(m_startTime), m_deltaTimeUs(0)
    {}

    Time::Time(const Time& other)
        : m_startTime(other.m_startTime),
        m_lastTime(other.m_lastTime),
        m_deltaTimeUs(other.m_deltaTimeUs)
    {}

    Time& Time::operator=(const Time& other)
    {
        if (this != &other)
        {
            m_startTime = other.m_startTime;
            m_lastTime = other.m_lastTime;
            m_deltaTimeUs = other.m_deltaTimeUs;
        }
        return *this;
    }

    Time::~Time() {}

    void Time::Update()
    {
        TimePoint now = Clock::now();
        m_deltaTimeUs = static_cast<uint32_t>(
            std::chrono::duration_cast<std::chrono::microseconds>(now - m_lastTime).count()
        );
        m_lastTime = now;
    }

    uint32_t Time::DeltaTime()
    {
        return m_deltaTimeUs;
    }

    uint32_t Time::TimeSinceStart()
    {
        return static_cast<uint32_t>(
            std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - m_startTime).count()
        );
    }

    void Time::Delay(double time)
    {
        std::this_thread::sleep_for(std::chrono::duration<double>(time));
    }

    void Time::DelayMs(uint32_t ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    void Time::DelayUs(uint32_t us)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(us));
    }



    Timer::Timer() : durationUs(0), active(false) {}

    void Timer::start(uint32_t duration_us)
    {
        startTime = Clock::now();
        durationUs = duration_us;
        active = true;
    }

    void Timer::reset()
    {
        active = false;
    }

    bool Timer::isFinished()
    {
        if (!active) return false;
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - startTime).count();
        return elapsed >= durationUs;
    }

    bool Timer::isRunning()
    {
        return active && !isFinished();
    }

    uint32_t Timer::timeLeft()
    {
        if (!active) return 0;
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - startTime).count();
        return (elapsed >= durationUs) ? 0 : (durationUs - static_cast<uint32_t>(elapsed));
    }
}