#ifndef MATRIX_DYN_C
#define MATRIX_DYN_C

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"
#include "assert_util.h"
#include "matrices/matrix.h"

mat mdyn_make_mat(Arena *arena, size_t row, size_t col, float *es);
mat mdyn_make_out(Arena *arena,mat *m1, mat *m2);
float *mdyn_make_float_array(Arena *arena,int count, ...);
mat mdyn_mul(Arena *arena, mat *a, mat *b);
mat mdyn_transpose(Arena *arena, mat *a);
mat mdyn_add(Arena *arena, mat *a, mat *b);
mat mdyn_sub(Arena *arena, mat *a, mat *b);
mat mdyn_hadamard(Arena *arena, mat *a, mat *b);
mat mdyn_slice(Arena *arena, mat *a, size_t row1, size_t row2, size_t col1, size_t col2);
mat mdyn_make_randomly_filled_mat(Arena *arena, size_t rows, size_t cols);
mat mdyn_make_zero_filled_mat(Arena *arena, size_t rows, size_t cols);
mat mdyn_identity(Arena *arena, size_t rows, size_t cols);

#ifdef MATRIX_DYN_IMPLEMENTATION

static inline float get_randf();
static inline float zero();

static inline float get_randf()
{
    return (float) rand() / (float)RAND_MAX;
}

static inline float zero()
{
    return 0.0f;
}

mat mdyn_make_mat(Arena *arena, size_t row, size_t col, float *es)
{
    mat mat = {0};
    float *tbl = (float*) arena_alloc(arena, sizeof(float) * row * col);
    ASSERT(tbl != NULL, "Buy new computer lmao");
    memcpy(tbl, es, sizeof(float) * row * col);
    mat_init(&mat, row, col, tbl);
    return mat;
}

mat mdyn_identity(Arena *arena, size_t rows, size_t cols)
{
    #define MAT_AT(mat, row, col) ((mat)->es[(mat)->cols * (row) + (col)])
    ASSERT(rows == cols, "Matrix must be a square");
    mat m = mdyn_make_zero_filled_mat(arena, rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        MAT_AT(&m, i, i) = 1.0f;
    }
    return m;
}

float *make_float_array(Arena *arena,int count, ...) {
    va_list args;
    va_start(args, count);
    float *es = arena_alloc(arena, count * sizeof(float));
    ASSERT(es != NULL, "Buy more RAM lol");
    for (int i = 0; i < count; i++) {
        double d = va_arg(args, double); // ALWAYS double
        es[i] = (float) d;
    }

    va_end(args);
    return es;
}

mat mdyn_mul(Arena *arena, mat *a, mat *b)
{
    mat m = mdyn_make_out(arena, a, b);
    mat_mul(a, b, &m);
    return m;
}

mat mdyn_make_out(Arena *arena, mat *m1, mat *m2)
{
    ASSERT(m1 != NULL, "m1 is NULL");
    ASSERT(m2 != NULL, "m2 is NULL");
    size_t r = m1->rows;
    size_t c = m2->cols;
    return mdyn_make_mat(arena,r, c, (float[]){0});
}


mat mdyn_transpose(Arena *arena, mat *a)
{
    mat b = mdyn_make_mat(arena, a->cols, a->rows, a->es);
    mat_transpose(a, &b);
    return b;
}

mat mdyn_add(Arena *arena, mat *a, mat *b)
{
    mat m = mdyn_make_mat(arena, a->rows, a->cols, a->es);
    mat_add(a, b, &m);
    return m;
}

mat mdyn_sub(Arena *arena, mat *a, mat *b)
{
    mat m = mdyn_make_mat(arena, a->rows, a->cols, a->es);
    mat_sub(a, b, &m);
    return m;
}

mat mdyn_hadamard(Arena *arena, mat *a, mat *b)
{
    mat m = mdyn_make_mat(arena, a->rows, a->cols, a->es);
    mat_hadamard(a, b, &m);
    return m;
}

mat mdyn_slice(Arena *arena, mat *a, size_t row1, size_t row2, size_t col1, size_t col2)
{
    size_t rows = row2 - row1;
    size_t cols = col2 - col1;
    mat m = mdyn_make_mat(arena, rows, cols, arena_alloc(arena, rows * cols * sizeof(float)));
    mat_slice(a, row1, row2, col1, col2, &m);
    return m;
}

mat mdyn_make_randomly_filled_mat(Arena *arena, size_t rows, size_t cols)
{
    mat mat = {0};
    float *es = (float *) arena_alloc(arena, rows * cols * sizeof(float));
    mat_init(&mat, rows, cols, es);
    mat_fill_func(&mat, get_randf);
    return mat;
}

mat mdyn_make_zero_filled_mat(Arena *arena, size_t rows, size_t cols)
{
    mat mat = {0};
    float *es = (float *) arena_alloc(arena, rows * cols * sizeof(float));
    mat_init(&mat, rows, cols, es);
    mat_fill_func(&mat, zero);
    return mat;
}

#endif // MATRIX_DYN_IMPLEMENTATION
#endif // MATRIX_DYN_C
