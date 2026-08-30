#ifndef MAT_PIPELINE_HELPER_H
#define MAT_PIPELINE_HELPER_H

#include <math.h>
#include <string.h>

typedef enum {
    MAT_ROTATE_X,
    MAT_ROTATE_Y,
    MAT_ROTATE_Z,
} Mat_Rotate;

mat mat_translation(Arena *arena, float x, float y, float z);
mat mat_projection(Arena *arena, float f, float n, float t, float r);
mat mat_scaling(Arena *arena, float n);
mat mat_rotate(Arena *arena, float degree, Mat_Rotate orientation);
mat mat_look_at(Arena *arena, mat *eye, mat *center, mat *up);

#ifdef MAT_PIPELINE_HELPER_IMPLEMENTATION

mat mat_look_at(Arena *arena, mat *eye, mat *center, mat *up)
{
    mat view = mdyn_make_zero_filled_mat(arena, 4, 4);
    size_t mark = arena->count;
    mat target = *center;
    mat pos = *eye;
    mat up_by_respect_to_worldspace = *up;

    mat direction_unnormalized = mdyn_sub(arena, &pos, &target);
    mat direction = mdyn_normalize(arena, &direction_unnormalized);
    
    mat right_unnormalized = mdyn_cross(arena, &up_by_respect_to_worldspace, &direction);
    mat right = mdyn_normalize(arena, &right_unnormalized);
    mat up_by_respect_to_the_eye = mdyn_cross(arena, &direction, &right);

    float view_es[] = {
      MAT_AT(&right, 0, 0), MAT_AT(&right, 1, 0), MAT_AT(&right, 2, 0), -mat_dot(&right, &pos),
      MAT_AT(&up_by_respect_to_the_eye, 0, 0), MAT_AT(&up_by_respect_to_the_eye, 1, 0), MAT_AT(&up_by_respect_to_the_eye, 2, 0), -mat_dot(&up_by_respect_to_the_eye, &pos),
      MAT_AT(&direction, 0, 0), MAT_AT(&direction, 1, 0), MAT_AT(&direction, 2, 0), -mat_dot(&direction, &pos),
      0.0f, 0.0f, 0.0f, 1.0f
    };

    memcpy(view.es, view_es, sizeof(float) * 4 * 4);
    
    arena_reset_to(arena, mark);
    return view;
}

mat mat_translation(Arena *arena, float x, float y, float z)
{
    mat lm = mdyn_identity(arena, 4, 4);
    MAT_AT(&lm, 0, 3) = x;
    MAT_AT(&lm, 1, 3) = y;
    MAT_AT(&lm, 2, 3) = z;
    return lm;
}

mat mat_projection(Arena *arena, float f, float n, float t, float r)
{
    float mat_projection[] = {
        n/r, 0.0f, 0.0f, 0.0f,
        0.0f, n/t, 0.0f, 0.0f,
        0.0f, 0.0f, -(f + n)/(f - n), (-2.0f*(f*n))/(f - n),
        0.0f, 0.0f, -1.0f, 0.0f
    };
    return mdyn_make_mat(arena, 4, 4, mat_projection);
}

mat mat_scaling(Arena *arena, float n)
{
    mat lm = mdyn_identity(arena, 4, 4);
    mat_mul_scalar(&lm, n, &lm);
    MAT_AT(&lm, 3, 3) = 1.0f;
    return lm;
}

mat mat_rotate(Arena *arena, float degree, Mat_Rotate orientation)
{
    float cos_val = cos(degree);
    float sin_val = sin(degree);

    switch (orientation) {
    case MAT_ROTATE_X:
        return mdyn_make_mat(arena, 4, 4, (float[]){
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cos_val, -sin_val, 0.0f,
            0.0f, sin_val, cos_val, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        });
    case MAT_ROTATE_Y:
        return mdyn_make_mat(arena, 4, 4, (float[]){
            cos_val, 0.0f, sin_val, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            -sin_val, 0.0f, cos_val, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        });
    default:
        return mdyn_make_mat(arena, 4, 4, (float[]){
            cos_val, -sin_val, 0.0f, 0.0f,
            sin_val,  cos_val, 0.0f, 0.0f,
            0.0f,     0.0f,    1.0f, 0.0f,
            0.0f,     0.0f,    0.0f, 1.0f
        });
    }
}

#endif // MAT_PIPELINE_HELPER_IMPLEMENTATION
#endif // MAT_PIPELINE_HELPER_H
