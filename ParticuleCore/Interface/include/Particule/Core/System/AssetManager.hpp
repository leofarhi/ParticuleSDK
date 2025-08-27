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
#include <utility>

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
    inline static bool initialized = false;
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
        if (!initialized)
        {
            initialized = true;
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
    }

    template<typename T>
    static void SetupLoaders(uint32_t id)
    {
        assert(id != std::numeric_limits<uint32_t>::max() && "Invalid Asset ID");
        AssetManager::InitAssetManager();
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
        }
    }

    static void ResetRefCount(uint32_t id) {
        if (id < builtInAssetCount) return;
        externalAssets[id - builtInAssetCount].refCount = 0;
    }

    static void ResetAllRefCounts() {
        for (auto& entry : externalAssets)
            entry.refCount = 0;
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
    // Par défaut : invalide
    Asset() noexcept
        : id_(kInvalid), external_(false), ptr_(nullptr) {}

    // Asset géré par l'AssetManager via un ID
    Asset(uint32_t id)
        : id_(kInvalid), external_(false), ptr_(nullptr)
    {
        if (id != kInvalid) {
            AssetManager::InitAssetManager();
            AssetManager::IncrementRef(id);
            AssetManager::SetupLoaders<T>(id);
            id_  = id;
            ptr_ = AssetManager::Get<T>(id);
        }
    }

    // Asset "direct" (non géré) : on alloue notre propre T** qui pointe vers le T* fourni
    Asset(T* directPtr)
        : id_(kInvalid), external_(true), ptr_(nullptr)
    {
        ptr_ = new T*(directPtr); // T** privé et indépendant
    }

    // --- Copie ---
    Asset(const Asset& other)
        : id_(kInvalid), external_(other.external_), ptr_(nullptr)
    {
        if (external_) {
            // Deep-copy du T** (on duplique le conteneur, pas l'objet T)
            T* raw = other.Get();           // peut être nullptr
            ptr_ = new T*(raw);             // nouveau T** indépendant
        } else {
            // Copie "gérée"
            id_  = other.id_;
            ptr_ = other.ptr_;
            if (IsManaged()) AssetManager::IncrementRef(id_);
        }
    }

    // --- Move ---
    Asset(Asset&& other) noexcept
        : id_(other.id_), external_(other.external_), ptr_(other.ptr_)
    {
        // On neutralise l'autre pour éviter double free / double decrement
        other.id_      = kInvalid;
        other.external_= false;
        other.ptr_     = nullptr;
    }

    // --- Affectation par copie ---
    Asset& operator=(const Asset& other)
    {
        if (this == &other) return *this;

        // Libère ce que l'on détient actuellement
        release_();

        // Reprend depuis "other"
        external_ = other.external_;
        if (external_) {
            // Deep-copy de son T** (container), même si le T* à l'intérieur est nul
            T* raw = other.Get();
            ptr_ = new T*(raw);
            id_  = kInvalid;
        } else {
            id_  = other.id_;
            ptr_ = other.ptr_;
            if (IsManaged()) AssetManager::IncrementRef(id_);
        }
        return *this;
    }

    // --- Affectation par move ---
    Asset& operator=(Asset&& other) noexcept
    {
        if (this == &other) return *this;

        release_();

        id_       = other.id_;
        external_ = other.external_;
        ptr_      = other.ptr_;

        other.id_        = kInvalid;
        other.external_  = false;
        other.ptr_       = nullptr;

        return *this;
    }

    ~Asset()
    {
        release_();
    }

    // -------- API --------
    bool operator==(const Asset& other) const noexcept { return Get() == other.Get(); }
    bool operator!=(const Asset& other) const noexcept { return !(*this == other);   }

    bool IsValid()  const noexcept { return ptr_ != nullptr; }
    bool IsLoaded() const noexcept { return ptr_ && *ptr_ != nullptr; }

    void Load()
    {
        if (IsManaged() && ptr_ && *ptr_ == nullptr) {
            AssetManager::Load<T>(id_);
        }
    }

    T* operator->() const { return *ptr_; }
    T& operator* () const { return **ptr_; }

    uint32_t GetID() const noexcept { return id_; }
    T*       Get()   const noexcept { return ptr_ ? *ptr_ : nullptr; }

    // utilitaires
    void Swap(Asset& other) noexcept
    {
        std::swap(id_, other.id_);
        std::swap(external_, other.external_);
        std::swap(ptr_, other.ptr_);
    }

private:
    static constexpr uint32_t kInvalid = uint32_t(-1);

    bool IsManaged() const noexcept { return !external_ && id_ != kInvalid; }

    void release_() noexcept
    {
        if (IsManaged()) {
            AssetManager::DecrementRef(id_);
            // Ne pas toucher à ptr_ : c'est géré par l'AssetManager
        } else if (external_ && ptr_) {
            // Nous possédons le T** (pas le T* pointé)
            delete ptr_;
        }
        id_       = kInvalid;
        external_ = false;
        ptr_      = nullptr;
    }

private:
    uint32_t id_;
    bool     external_;
    T**      ptr_;
};


} // namespace Particule::Core

#endif