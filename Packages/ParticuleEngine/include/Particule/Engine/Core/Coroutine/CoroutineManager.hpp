#pragma once
#include <Particule/Engine/Core/Coroutine/Coroutine.hpp>
#include <vector>
#include <memory>

namespace Particule::Engine {

    class CoroutineManager {
    public:
        Coroutine* start(Coroutine&& co);
        void stop(Coroutine& coroutine);
        void update();

        static CoroutineManager& instance();

    private:
        std::vector<std::unique_ptr<Coroutine>> coroutines;
    };

}