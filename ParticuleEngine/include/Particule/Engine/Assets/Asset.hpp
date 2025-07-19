#pragma once
#include <Particule/Core/ParticuleCore.hpp>
using namespace Particule::Core;
#include <memory>

template<typename T>
class Asset {
public:
    Asset() = default;
    Asset(const std::shared_ptr<T>& ptr) : ptr(ptr) {}

    T* operator->() { return ptr.get(); }
    T& operator*()  { return *ptr; }
    operator bool() const { return ptr != nullptr; }

private:
    std::shared_ptr<T> ptr; // Partagée avec AssetManager
};