#ifndef CAMERA_H
#define CAMERA_H

#include "matrices/matrix.h"
#include "matrices/matrix_dyn.h"

typedef struct {
    mat pos;
    mat up;
    mat direction;
    mat right;
} Camera;

void camera_init(Camera *c, Arena *arena);

#ifdef CAMERA_IMPLEMENTATION

void camera_init(Camera *c, Arena *arena)
{
    mat target = mdyn_make_zero_filled_mat(arena, 3, 1);
    mat pos = mdyn_make_mat(arena, 3, 1, (float[]){
                0.0f,   
                0.0f,   
                3.0f,   
             });
    mat direction_unnormalized = mdyn_sub(arena, &pos, &target);
    mat direction = mdyn_normalize(arena, &direction_unnormalized);

    mat up_by_respect_to_worldspace = mdyn_make_mat(arena, 3, 1, (float[]){
                0.0f,
                1.0f,
                0.0f,
             });
    
    mat right_unnormalized = mdyn_cross(arena, &up_by_respect_to_worldspace, &direction);
    mat right = mdyn_normalize(arena, &right_unnormalized);
    mat up = mdyn_cross(arena, &direction, &right);

    size_t mark = arena->count;
    c->pos = mdyn_make_mat(arena, 4, 1, (float[]){
                MAT_AT(&pos, 0, 0),
                MAT_AT(&pos, 1, 0),
                MAT_AT(&pos, 2, 0),
                1.0f
             });
    c->direction = mdyn_make_mat(arena, 4, 1, (float[]){
                        MAT_AT(&direction, 0, 0),
                        MAT_AT(&direction, 1, 0),
                        MAT_AT(&direction, 2, 0),
                        1.0f
                   });
    c->right = mdyn_make_mat(arena, 4, 1, (float[]){
                        MAT_AT(&right, 0, 0),
                        MAT_AT(&right, 1, 0),
                        MAT_AT(&right, 2, 0),
                        1.0f
                   });
    c->up = mdyn_make_mat(arena, 4, 1, (float[]){
                        MAT_AT(&up, 0, 0),
                        MAT_AT(&up, 1, 0),
                        MAT_AT(&up, 2, 0),
                        1.0f
                   });
    arena_reset_to(arena, mark);
}

#endif // CAMERA_IMPLEMENTATION
#endif // CAMERA_H
