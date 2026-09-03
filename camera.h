#ifndef CAMERA_H
#define CAMERA_H

#include "matrices/matrix.h"
#include "matrices/matrix_dyn.h"
#include "matrices/mat_pipeline_helper.h"
#include "arena.h"

typedef struct {
    mat eye;
    mat center;
    mat up;
    float yaw;
    float pitch;

    // Pointer to the global arena variable
    Arena *arena;
} Camera;

typedef enum {
    CAM_MOV_FWD = 0,
    CAM_MOV_BWD
} Camera_Movement;

typedef enum {
    CAM_OBJ_EYE = 0,
    CAM_OBJ_CENTER
} Camera_Obj;

void camera_init(Camera *c, Arena *arena);
void camera_turn(Camera *c);
void camera_move(Camera *c, Camera_Movement move, float steps);

#ifdef CAMERA_IMPLEMENTATION

void camera_init(Camera *c, Arena *arena)
{
    c->eye = mdyn_make_mat(arena, 3, 1, (float[]){0.0f, 0.0f, 3.0f});
    c->center = mdyn_make_mat(arena, 3, 1, (float[]){0.0f, 0.0f, -3.0f});
    c->up = mdyn_make_mat(arena, 3, 1, (float[]){0.0f, 1.0f, 0.0f});
    c->yaw = 0.0f;
    c->pitch = 0.0f;
    c->arena = arena;
}

void camera_move(Camera *c, Camera_Movement mov, float steps)
{
    switch (mov) {
        case CAM_MOV_FWD: {
            size_t mark = c->arena->count;
            mat diff = mdyn_sub(c->arena, c->eye, c->center);
            mat norm = mdyn_normalize(c->arena, diff);
            mat_mul_scalar(&norm, steps, &norm);
            mat_sub(&c->eye, &norm, &c->eye);
            arena_reset_to(c->arena, mark);
        } break;
        case CAM_MOV_BWD: {
            size_t mark = c->arena->count;
            mat diff = mdyn_sub(c->arena, c->eye, c->center);
            mat norm = mdyn_normalize(c->arena, diff);
            mat_mul_scalar(&norm, steps, &norm);
            mat_add(&c->eye, &norm, &c->eye);
            arena_reset_to(c->arena, mark);
        } break;
        default:
            break;
    }
}

void camera_turn(Camera *c)
{
    mat rot_y = mat_rotate(c->arena, c->yaw, MAT_ROTATE_Y);
    mat rot_x = mat_rotate(c->arena, c->pitch, MAT_ROTATE_X);

    size_t mark = c->arena->count;

    mat center = mdyn_make_mat(c->arena, 4, 1, (float[]){
            MAT_AT(&c->eye, 0, 0),
            MAT_AT(&c->eye, 1, 0),
            -MAT_AT(&c->eye, 2, 0)*10,
            1.0f
        });
    center = mdyn_mul(c->arena, rot_x, center);
    center = mdyn_mul(c->arena, rot_y, center);

    MAT_AT(&c->center, 0, 0) = MAT_AT(&center, 0, 0);
    MAT_AT(&c->center, 1, 0) = MAT_AT(&center, 1, 0);
    MAT_AT(&c->center, 2, 0) = MAT_AT(&center, 2, 0);

    arena_reset_to(c->arena, mark);
}

#endif // CAMERA_IMPLEMENTATION
#endif // CAMERA_H
