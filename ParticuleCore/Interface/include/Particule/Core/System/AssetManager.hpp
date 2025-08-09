#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include <Particule/Core/System/Redefine.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <cassert>
#include <type_traits>
#include <atomic>
#include <memory>

namespace Particule::Core {
    extern void* __builtInAssetsRaw[];


struct AssetEntry {
    void* ptr = nullptr;
    void* (*load_fn)(uint32_t) = nullptr;
    void (*unload_fn)(void*) = nullptr;
    int refCount;
};


// --- AssetManager ---
class AssetManager {
private:
    inline static void** builtInAssets = nullptr;
    inline static uint32_t builtInAssetCount = 0;
    static inline AssetEntry externalAssets[EXTERNAL_ASSET_COUNT+1];
    static inline const char* externalAssetPath = EXTERNAL_ASSET_PATH;

    AssetManager() = default;

    template<typename T>
    static void* load_external_asset(uint32_t id) {
        std::string path = std::string(externalAssetPath) + "/" + std::to_string(id) + ".asset";
        return static_cast<void*>(T::Load(path));
    }

    template<typename T>
    static void unload_external_asset(void* ptr) {
        T::Unload(static_cast<T*>(ptr));
    }

public:
    static void InitAssetManager() {
        builtInAssets = __builtInAssetsRaw;
        builtInAssetCount = 0;
        while (builtInAssets[builtInAssetCount] != nullptr)
            builtInAssetCount++;
        for (auto& e : externalAssets) {
            e.ptr = nullptr;
            e.unload_fn = nullptr;
            e.refCount = 0;
        }
    }

    template<typename T>
    static void SetupLoaders(uint32_t id)
    {
        assert(id < builtInAssetCount + EXTERNAL_ASSET_COUNT && "Asset ID out of range");
        if (id < builtInAssetCount)
            return;
        size_t extID = id - builtInAssetCount;
        assert(extID < EXTERNAL_ASSET_COUNT && "External Asset ID out of range");
        AssetEntry* entry = &externalAssets[extID];
        entry->load_fn = &load_external_asset<T>;
        entry->unload_fn = &unload_external_asset<T>;
    }

    template<typename T>
    static T* Load(uint32_t id) {
        if (id == uint32_t(-1)) return nullptr;
        if (id < builtInAssetCount)
            return static_cast<T*>(builtInAssets[id]);

        size_t extID = id - builtInAssetCount;
        auto& entry = externalAssets[extID];
        if (!entry.ptr)
        {
            SetupLoaders<T>(id);
            entry.ptr = entry.load_fn ? entry.load_fn(id) : nullptr;
        }
        return static_cast<T*>(entry.ptr);
    }

    template<typename T>
    static T** Get(uint32_t id) {
        if (id == uint32_t(-1)) return nullptr;
        if (id < builtInAssetCount)
            return reinterpret_cast<T**>(&builtInAssets[id]);
        size_t extID = id - builtInAssetCount;
        return reinterpret_cast<T**>(&externalAssets[extID].ptr);
    }

    static void IncrementRef(uint32_t id) {
        if (id < builtInAssetCount) return;
        externalAssets[id - builtInAssetCount].refCount++;
    }

    static void DecrementRef(uint32_t id) {
        if (id < builtInAssetCount) return;
        auto& entry = externalAssets[id - builtInAssetCount];
        --entry.refCount;
    }

    static void Unload(uint32_t id)
    {
        if (id < builtInAssetCount) return;
        auto& entry = externalAssets[id - builtInAssetCount];
        if (entry.ptr && entry.unload_fn) {
            entry.unload_fn(entry.ptr);
            entry.ptr = nullptr;
        }
    }

    static void LoadUsed() {
        uint32_t extID = builtInAssetCount;
        for (auto& entry : externalAssets) {
            if (entry.refCount > 0 && !entry.ptr && entry.load_fn)
                entry.ptr = entry.load_fn(extID);
            extID++;
            assert(extID < builtInAssetCount + EXTERNAL_ASSET_COUNT);
        }
    }

    static void UnloadUnused() {
        for (auto& entry : externalAssets) {
            if (entry.refCount <= 0 && entry.ptr && entry.unload_fn) {
                entry.unload_fn(entry.ptr);
                entry.ptr = nullptr;
            }
        }
    }

    static void UnloadAll() {
        for (auto& entry : externalAssets) {
            if (entry.ptr && entry.unload_fn) {
                entry.unload_fn(entry.ptr);
                entry.ptr = nullptr;
            }
            entry.refCount = 0;
        }
    }

    static bool IsLoaded(uint32_t id) {
        if (id < builtInAssetCount) return true;
        return externalAssets[id - builtInAssetCount].ptr != nullptr;
    }
};

// --- Classe Asset<T> ---
template<typename T>
class Asset {
public:
    constexpr Asset() : id(-1), external(false), ptr(nullptr) {}

    // Asset avec ID
    constexpr Asset(uint32_t id_) : id(id_), external(false) {
        if (id != uint32_t(-1)) {
            AssetManager::IncrementRef(id);
            AssetManager::SetupLoaders<T>(id);
            ptr = AssetManager::Get<T>(id);
        }
    }

    // Asset direct (non géré par AssetManager)
    constexpr Asset(T* directPtr) : id(-1), external(true) {
        ptr = new T*(directPtr);  // on simule le T** pour uniformité
    }

    Asset(const Asset& other) : id(other.id), external(other.external), ptr(other.ptr) {
        if (!external && id != uint32_t(-1)) AssetManager::IncrementRef(id);
    }

    Asset& operator=(const Asset& other) {
        if (this != &other) {
            if (!external && id != uint32_t(-1)) AssetManager::DecrementRef(id);
            id = other.id;
            external = other.external;
            ptr = other.ptr;
            if (!external && id != uint32_t(-1)) AssetManager::IncrementRef(id);
        }
        return *this;
    }

    ~Asset() {
        if (!external && id != uint32_t(-1)) AssetManager::DecrementRef(id);
        if (external && ptr) {
            delete ptr; // libère le T**
            ptr = nullptr;
        }
    }

    inline bool operator==(const Asset& other) const { return ptr == other.ptr; }
    inline bool operator!=(const Asset& other) const { return ptr != other.ptr; }

    inline bool IsValid() const { return ptr != nullptr; }
    inline bool IsLoaded() const { return ptr && *ptr != nullptr; }

    inline void Load() {
        if (!external && !*ptr && id != uint32_t(-1)) {
            AssetManager::Load<T>(id);
        }
    }

    inline T* operator->() const { return *ptr; }
    inline T& operator* () const { return **ptr; }

    constexpr uint32_t GetID() const { return id; }
    inline T* Get() const { return *ptr; }

private:
    uint32_t id;
    bool external;
    T** ptr;
};


} // namespace Particule::Core

#endif