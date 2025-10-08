#pragma once
typedef struct Arena {
    size_t size;
    size_t capacity;
    size_t offset;
    void* buffer;
} Arena;

#define MB(x) ((size_t)(x)*((size_t)(1000)*(size_t)(1000)))

Arena* Arena_init(size_t capacity);
void Arena_reset(Arena* a);
void* Arena_alloc(Arena* a, size_t size);