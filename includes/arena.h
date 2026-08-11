#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    size_t capacity;
    size_t count;
    unsigned char *data;
} Arena;

int arena_init(Arena *arena, size_t n);
int arena_init_from_block(Arena *arena, void *block, size_t size);
void arena_reset(Arena *arena);
void *arena_alloc(Arena *arena, size_t n);
void arena_free(Arena *arena);
void arena_reset_to(Arena *arena, size_t checkpoint);

#ifdef ARENA_IMPLEMENTATION

int arena_init(Arena *arena, size_t n)
{
    arena->capacity = n;
    arena->count = 0;
    arena->data = malloc(n);
    return arena->data != NULL;
}

int arena_init_from_block(Arena *arena, void *block, size_t size)
{
    arena->capacity = size;
    arena->count = 0;
    arena->data = block;
    return arena->data != NULL;
}

void arena_reset(Arena *arena)
{
    arena->count = 0;
}

void arena_reset_to(Arena *arena, size_t checkpoint)
{
    arena->count = checkpoint;
}

void *arena_alloc(Arena *arena, size_t n)
{
    assert(arena->count + n <= arena->capacity);
    void *result = arena->data + arena->count;
    arena->count += n;
    return result;
}

void arena_free(Arena *arena)
{
    free(arena->data);
    arena->data = NULL;
    arena->count = 0;
    arena->capacity= 0;
}

#endif // ARENA_IMPLEMENTATION
#endif // ARENA_H
