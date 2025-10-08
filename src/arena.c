// Arena code such that i can have one large arena for all buffers

#include <stdlib.h>
#include <raylib.h>
#include <arena.h>

Arena* Arena_init(size_t capacity) {
    if (capacity < 0 || capacity == 0) {
        return NULL;
    }
    Arena* arena = MemAlloc(sizeof(Arena) + capacity);
    if (arena == NULL) {
        return NULL;
    }
    arena->capacity = capacity;
    arena->size = 0;
    arena->offset = 0;
    arena->buffer = (void*)(arena + 1);
    return arena;
}

void Arena_reset(Arena* a) {
    a->offset = 0;
    a->size = 0; 
}

void* Arena_alloc(Arena* a, size_t size) {
    size_t aligned_size = (size + 7) & ~7; // Align to 8 bytes
    if (a->offset + aligned_size > a->capacity) {
        // Not enough space in the arena
        return NULL;
    }
    void *ptr = (char*)a->buffer + a->offset;
    a->offset += aligned_size;
    return ptr;
}