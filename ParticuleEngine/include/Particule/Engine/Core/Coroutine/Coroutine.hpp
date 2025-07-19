#pragma once
#include <coroutine>
#include <memory>
#include <chrono>
#include <optional>
#include <stack>
#include <ctime>
#include <Particule/Core/System/Time.hpp>

using Clock = std::chrono::steady_clock;

struct YieldInstruction {
    virtual bool keepWaiting() = 0;
    virtual ~YieldInstruction() = default;
};

struct WaitForSeconds : public YieldInstruction {
    Particule::Core::Timer timer;
    WaitForSeconds(float seconds) {
        timer.start(static_cast<uint32_t>(seconds * 1000000)); // Convert seconds to microseconds
    }
    bool keepWaiting() override {
        return !timer.isFinished();
    }
};

struct WaitForMilliseconds : public YieldInstruction {
    Particule::Core::Timer timer;

    WaitForMilliseconds(uint32_t milliseconds) {
        timer.start(milliseconds * 1000); // Convert milliseconds to microseconds
    }

    bool keepWaiting() override {
        return !timer.isFinished();
    }
};


class CoroutineManager; // Forward declaration

class Coroutine {
public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    Coroutine(handle_type h);
    Coroutine(Coroutine&& other) noexcept;
    Coroutine& operator=(Coroutine&& other) noexcept;
    ~Coroutine();

    bool is_done() const;
    void update();

    void stop(); // marquer comme terminé
    friend class CoroutineManager;

private:
    handle_type handle = nullptr;
    bool manuallyStopped = false;
};

struct Coroutine::promise_type {
    std::optional<Coroutine> yieldedCoroutine;
    std::unique_ptr<YieldInstruction> yieldInstr;

    Coroutine get_return_object() {
        return Coroutine{handle_type::from_promise(*this)};
    }

    std::suspend_always initial_suspend() { return {}; }

    std::suspend_always final_suspend() noexcept { return {}; }

    std::suspend_always yield_value(std::unique_ptr<YieldInstruction> instr) {
        yieldInstr = std::move(instr);
        return {};
    }

    std::suspend_always yield_value(Coroutine subCoroutine) {
        yieldedCoroutine = std::move(subCoroutine);
        return {};
    }

    void return_void() {}
    void unhandled_exception() { std::terminate(); }
};

inline Coroutine::Coroutine(handle_type h) : handle(h) {}
inline Coroutine::Coroutine(Coroutine&& other) noexcept : handle(other.handle), manuallyStopped(other.manuallyStopped) {
    other.handle = nullptr;
}
inline Coroutine& Coroutine::operator=(Coroutine&& other) noexcept {
    if (this != &other) {
        if (handle) handle.destroy();
        handle = other.handle;
        manuallyStopped = other.manuallyStopped;
        other.handle = nullptr;
    }
    return *this;
}
inline Coroutine::~Coroutine() {
    if (handle) handle.destroy();
}

inline bool Coroutine::is_done() const {
    return manuallyStopped || !handle || handle.done();
}

inline void Coroutine::stop() {
    manuallyStopped = true;
}

inline void Coroutine::update() {
    if (is_done()) return;

    auto& promise = handle.promise();

    if (promise.yieldInstr) {
        if (promise.yieldInstr->keepWaiting()) return;
        promise.yieldInstr = nullptr;
    }

    if (promise.yieldedCoroutine) {
        promise.yieldedCoroutine->update();
        if (!promise.yieldedCoroutine->is_done()) return;
        promise.yieldedCoroutine.reset();
    }

    handle.resume();
}
