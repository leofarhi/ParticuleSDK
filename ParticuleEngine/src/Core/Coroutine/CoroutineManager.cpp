#include <Particule/Engine/Core/Coroutine/CoroutineManager.hpp>
#include <Particule/Engine/Core/Coroutine/Coroutine.hpp>

Coroutine* CoroutineManager::start(Coroutine&& co) {
    coroutines.emplace_back(std::make_unique<Coroutine>(std::move(co)));
    return coroutines.back().get();
}

void CoroutineManager::stop(Coroutine& coroutine) {
    coroutine.stop();
}

void CoroutineManager::update() {
    for (auto it = coroutines.begin(); it != coroutines.end(); ) {
        if ((*it)->is_done())
            it = coroutines.erase(it);
        else {
            (*it)->update();
            ++it;
        }
    }
}

CoroutineManager& CoroutineManager::instance() {
    static CoroutineManager mgr;
    return mgr;
}