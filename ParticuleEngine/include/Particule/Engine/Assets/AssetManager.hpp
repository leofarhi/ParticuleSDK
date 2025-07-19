#pragma once
#include <Particule/Core/ParticuleCore.hpp>
using namespace Particule::Core;
#include "Asset.hpp"
#include <unordered_map>

class AssetManager {
public:
    template<typename T>
    Asset<T> Load(const Resource& resource) {
        auto& map = GetMap<T>();
        auto it = map.find(resource);
        if (it != map.end()) {
            return Asset<T>(it->second.lock());
        }

        std::shared_ptr<T> asset = T::Load(resource);
        map[resource] = asset;
        return Asset<T>(asset);
    }

    template<typename T>
    void UnloadUnused() {
        auto& map = GetMap<T>();
        for (auto it = map.begin(); it != map.end(); ) {
            if (it->second.expired())
            {
                T* asset = static_cast<T*>(it->second.lock().get());
                it = map.erase(it);
                T::Free(asset);
            }
            else
                ++it;
        }
    }

private:
    template<typename T>
    std::unordered_map<Resource, std::weak_ptr<T>>& GetMap() {
        static std::unordered_map<Resource, std::weak_ptr<T>> map;
        return map;
    }
};
