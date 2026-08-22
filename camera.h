#ifndef CAMERA_H
#define CAMERA_H

#include "matrices/matrix.h"
#include "matrices/matrix_dyn.h"

typedef struct {
    mat eye;
    mat center;
    mat up;
} Camera;

void camera_init(Camera *c, Arena *arena);

#ifdef CAMERA_IMPLEMENTATION

void camera_init(Camera *c, Arena *arena)
{
    c->eye = mdyn_make_mat(arena, 3, 1, (float[]){0.0f, 0.0f, 3.0f});
    c->center = mdyn_make_mat(arena, 3, 1, (float[]){0.0f, 0.0f, 0.0f});
    c->up = mdyn_make_mat(arena, 3, 1, (float[]){0.0f, 1.0f, 0.0f});
}

#endif // CAMERA_IMPLEMENTATION
#endif // CAMERA_H
