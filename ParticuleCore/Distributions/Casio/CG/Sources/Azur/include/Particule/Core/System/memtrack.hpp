#if !defined(MEMTRACK_H) && defined(MEMTRACK_ENABLED)
#define MEMTRACK_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gint.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// --- Compteur global ---
#define MAX_TRACKED_PTRS 4096
static void* tracked_ptrs[MAX_TRACKED_PTRS];
static int memtrack_alloc_count = 0;

int is_tracked(void* ptr) {
    if (ptr == NULL) return 0; // Avoid checking NULL pointers
    for(int i = 0; i < MAX_TRACKED_PTRS; i++)
        if (tracked_ptrs[i] == ptr) return 1;
    return 0;
}

void track_ptr(void* ptr) {
    if (ptr == NULL) return; // Avoid tracking NULL pointers
    for(int i = 0; i < MAX_TRACKED_PTRS; i++) {
        if (tracked_ptrs[i] == NULL) {
            memtrack_alloc_count++;
            tracked_ptrs[i] = ptr;
            return;
        }
    }
}

void untrack_ptr(void* ptr) {
    if (ptr == NULL) return; // Avoid untracking NULL pointers
    for(int i = 0; i < MAX_TRACKED_PTRS; i++) {
        if (tracked_ptrs[i] == ptr) {
            memtrack_alloc_count--;
            tracked_ptrs[i] = NULL;
            return;
        }
    }
}

__attribute__((constructor))
void memtrack_init(void) {
    memtrack_alloc_count = 0;
    for(int i = 0; i < MAX_TRACKED_PTRS; i++)
        tracked_ptrs[i] = NULL;
}

// --- malloc/free overrides ---
void* malloc(size_t size) {
    
    void* ptr = kmalloc(size, NULL);
    track_ptr(ptr);
    return ptr;
}

void free(void* ptr) {
    if (!ptr) return; // Avoid double free
    untrack_ptr(ptr);
    kfree(ptr);
}

//calloc
void* calloc(size_t count, size_t size) {
    void* ptr = kmalloc(count * size, NULL);
    if (ptr) {
        track_ptr(ptr);
        memset(ptr, 0, count * size); // Initialize memory to zero
    }
    return ptr;
}

//realloc
void* realloc(void* ptr, size_t size) {
    untrack_ptr(ptr);
    ptr = krealloc(ptr, size);
    track_ptr(ptr);
    return ptr;
}

// --- Report à la fin du programme ---
__attribute__((destructor))
static void memtrack_report_leaks(void) {
    dclear(C_BLACK);
    if(memtrack_alloc_count > 0) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Memory leak: %d block(s)", memtrack_alloc_count);
        dprint(1, 1, C_WHITE, buffer);
    } else {
        dprint(1, 1, C_WHITE, "No memory leaks!");
    }
    dupdate();
    getkey(); // pause
}

#ifdef __cplusplus
}
#endif

// --- Opérateurs C++ ---
#ifdef __cplusplus

#include <new>

void* operator new(std::size_t size) {
    return malloc(size);
}

void* operator new[](std::size_t size) {
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    free(ptr);
}

void operator delete[](void* ptr) noexcept {
    free(ptr);
}

// Overloads supplémentaires (C++17, compatibilité SH)
void operator delete(void* ptr, unsigned int) {
    free(ptr);
}
void operator delete[](void* ptr, unsigned int) {
    free(ptr);
}

#endif // __cplusplus

#endif // MEMTRACK_ENABLED
