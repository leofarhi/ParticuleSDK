#pragma once
#include "Coroutine.hpp"
#include <vector>
#include <memory>

class CoroutineManager {
public:
    Coroutine* start(Coroutine&& co);
    void stop(Coroutine& coroutine);
    void update();

    static CoroutineManager& instance();

private:
    std::vector<std::unique_ptr<Coroutine>> coroutines;
};