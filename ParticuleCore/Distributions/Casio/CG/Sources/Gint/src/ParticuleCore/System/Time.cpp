#include <Particule/Core/System/Time.hpp>

namespace Particule::Core
{
    prof_t Time::m_startTime;

    Time::Time()
    {
        m_timer = prof_make();
        prof_enter_norec(m_timer);
    }

    Time::Time(const Time& other)
        : m_timer(other.m_timer)
    {}

    Time& Time::operator=(const Time& other)
    {
        if (this != &other) {
            m_timer = other.m_timer;
        }
        return *this;
    }

    Time::~Time() {
        prof_leave_norec(m_timer);
    }

    void Time::Update()
    {
        prof_leave_norec(m_timer);
        deltaTime = prof_time(m_timer);
        this->m_timer = prof_make();
        prof_enter_norec(m_timer);
    }

    uint32_t Time::DeltaTime()
    {
        return deltaTime;
    }

    uint32_t Time::TimeSinceStart()
    {
        prof_leave_norec(m_startTime);
        uint32_t elapsed = prof_time(m_startTime);
        prof_enter_norec(m_startTime);
        return elapsed;
    }

    void Time::Delay(double time)
    {
        uint32_t durationUs = static_cast<uint32_t>(time * 1000000.0);
        DelayUs(durationUs);
    }

    void Time::DelayMs(uint32_t ms)
    {
        DelayUs(ms * 1000);
    }

    void Time::DelayUs(uint32_t us)
    {
        prof_t t = prof_make();
        prof_enter_norec(t);
        while (1) {
            prof_leave_norec(t);
            uint32_t elapsed = prof_time(t);
            if (elapsed >= us) {
                break;
            }
            prof_enter_norec(t);
        }
    }


    Timer::Timer() : durationUs(0), active(false) {}
    Timer::~Timer() {
        if (active) {
            prof_leave_norec(startTime);
        }
    }

    void Timer::start(uint32_t duration_us) {
        durationUs = duration_us;
        startTime = prof_make();
        prof_enter_norec(startTime);
        active = true;
    }

    void Timer::reset() {
        active = false;
    }

    bool Timer::isFinished() {
        if (!active) return false;
        prof_leave_norec(startTime);
        uint32_t elapsed = prof_time(startTime);
        if (elapsed >= durationUs)
        {
            active = false;
            return true;
        }
        else
            prof_enter_norec(startTime);
        return false;
    }

    bool Timer::isRunning() {
        return active && !isFinished();
    }

    uint32_t Timer::timeLeft() {
        if (!active) return 0;
        prof_leave_norec(startTime);
        uint32_t elapsed = prof_time(startTime);
        prof_enter_norec(startTime);
        return (elapsed >= durationUs) ? 0 : (durationUs - elapsed);
    }

}