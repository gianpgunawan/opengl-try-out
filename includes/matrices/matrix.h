#ifndef NN_MAT_H
#define NN_MAT_H

#include <stdio.h>

#define NN_DATA_TYPE float
#define NN_DATA_FORMAT "%f"

typedef struct {
    size_t rows;
    size_t cols;
    NN_DATA_TYPE *es;
} mat;

#define NN_MAT_AT(mat, row, col) ((mat)->es[(mat)->cols * (row) + (col)])

int mat_init(mat *mat, size_t rows, size_t cols, NN_DATA_TYPE *es);
void mat_print(mat *mat);
int mat_mul(mat *m1, mat *m2, mat *out);
int mat_sub(mat *m1, mat *m2, mat *out);
int mat_add(mat *m1, mat *m2, mat *out);
int mat_hadamard(mat *m1, mat *m2, mat *out);
void mat_map(mat *m1, float (*fn)(float), mat *out);
void mat_mul_scalar(mat *m, float val, mat *out);
void mat_transpose(mat *m, mat *out);
void mat_slice(mat *m, size_t row1, size_t row2, size_t col1, size_t col2, mat *out);
void mat_fprintf(mat *mat, FILE *fp);

void mat_fill(mat *m, float val);
void mat_fill_func(mat *m, float (*func)());

#ifdef MATRIX_IMPLEMENTATION 

void mat_fill(mat *m, float val)
{
    for (size_t i = 0; i < m->rows; ++i) {
        for (size_t j = 0; j < m->cols; ++j) {
            NN_MAT_AT(m, i, j) = val;
        }
    }
}

void mat_mul_scalar(mat *m, float val, mat *out)
{
    NN_ASSERT(m != NULL, "MATRIX 1 IS NULL");
    NN_ASSERT(out != NULL, "OUT IS NULL");
    NN_ASSERT(m->cols == out->cols && m->rows == out->rows, "MATRICES SIZE MISMATCH");
    for (size_t i = 0; i < m->rows; ++i) {
        for (size_t j = 0; j < m->cols; ++j) {
            NN_MAT_AT(out, i, j) *= val;
        }
    }
}

void mat_fill_func(mat *m, float (*func)())
{
    for (size_t i = 0; i < m->rows; ++i) {
        for (size_t j = 0; j < m->cols; ++j) {
            NN_MAT_AT(m, i, j) = func();
        }
    }
}

int mat_mul(mat *m1, mat *m2, mat *out)
{
    NN_ASSERT(m1 != NULL, "MATRIX 1 IS NULL");
    NN_ASSERT(m2 != NULL, "MATRIX 2 IS NULL");
    NN_ASSERT(out != NULL, "OUT IS NULL");
    NN_ASSERT(m1->cols == m2->rows, "MATRICES SIZE MISMATCH");
    NN_ASSERT(out->cols == m2->cols && out->rows == m1->rows, "MATRIX OUT SIZE MISMATCH");
    
    for (size_t i = 0; i < m1->rows; ++i) {
        for (size_t j = 0; j < m2->cols; ++j) {
            NN_DATA_TYPE sum = 0;
            for (size_t k = 0; k < m2->rows; ++k) {
                sum += NN_MAT_AT(m1, i, k) * NN_MAT_AT(m2, k, j);
            }
            NN_MAT_AT(out, i, j) = sum;
        }
    }
    return 0;
}

int mat_add(mat *m1, mat *m2, mat *out)
{
    NN_ASSERT(m1 != NULL, "MATRIX 1 IS NULL");
    NN_ASSERT(m2 != NULL, "MATRIX 2 IS NULL");
    NN_ASSERT(out != NULL, "OUT IS NULL");
    NN_ASSERT(m1->cols == m2->cols && m1->rows == m2->rows, "MATRIX SIZE MISMATCH");
    NN_ASSERT(out->cols == m2->cols && out->rows == m2->rows, "MATRIX OUT SIZE MISMATCH");

    for (size_t i = 0; i < out->rows; ++i) {
        for (size_t j = 0; j < out->cols; ++j) {
            NN_MAT_AT((out), i, j) = NN_MAT_AT((m1), i, j) + NN_MAT_AT((m2), i, j);
        }
    }
    return 0;
}

int mat_sub(mat *m1, mat *m2, mat *out)
{
    NN_ASSERT(m1 != NULL, "MATRIX 1 IS NULL");
    NN_ASSERT(m2 != NULL, "MATRIX 2 IS NULL");
    NN_ASSERT(out != NULL, "OUT IS NULL");
    NN_ASSERT(m1->cols == m2->cols && m1->rows == m2->rows, "MATRICES SIZE MISMATCH");
    NN_ASSERT(out->cols == m2->cols && out->rows == m2->rows, "MATRIX OUT SIZE MISMATCH");
    
    for (size_t i = 0; i < out->rows; ++i) {
        for (size_t j = 0; j < out->cols; ++j) {
            NN_MAT_AT((out), i, j) = NN_MAT_AT((m1), i, j) - NN_MAT_AT((m2), i, j);
        }
    }
    return 0;
}

int mat_hadamard(mat *m1, mat *m2, mat *out)
{
    NN_ASSERT(m1 != NULL, "MATRIX 1 IS NULL");
    NN_ASSERT(m2 != NULL, "MATRIX 2 IS NULL");
    NN_ASSERT(out != NULL, "OUT IS NULL");
    NN_ASSERT(m1->cols == m2->cols && m1->rows == m2->rows, "MATRIX SIZE MISMATCH");
    NN_ASSERT(out->cols == m2->cols && out->rows == m2->rows, "MATRIX OUT SIZE MISMATCH");

    for (size_t i = 0; i < out->rows; ++i) {
        for (size_t j = 0; j < out->cols; ++j) {
            NN_MAT_AT(out, i, j) = NN_MAT_AT(m1, i, j) * NN_MAT_AT(m2, i, j);
        }
    }
    return 0;
}

void mat_map(mat *m1, float (*fn)(float), mat *out)
{
    NN_ASSERT(m1 != NULL, "MATRIX 1 IS NULL");
    NN_ASSERT(out != NULL, "OUT IS NULL");
    NN_ASSERT(out->cols == m1->cols && out->rows == m1->rows, "MATRIX OUT SIZE MISMATCH");
    for (size_t i = 0; i < out->rows; ++i) {
        for (size_t j = 0; j < out->cols; ++j) {
            NN_MAT_AT(out, i, j) = fn(NN_MAT_AT(m1, i, j));
        }
    }
}

void mat_print(mat *mat)
{
    NN_ASSERT(mat != NULL, "MATRIX IS NULL");
    printf("{\n");
    for (size_t i = 0; i < mat->rows; ++i) {
        printf("    ");
        for (size_t j = 0; j < mat->cols; ++j) {
            NN_DATA_TYPE val = NN_MAT_AT(mat, i, j);
            printf(NN_DATA_FORMAT ", ",  val);
        }
        printf("\n");

    }
    printf("}\n");
}

void mat_fprintf(mat *mat, FILE *fp)
{
    NN_ASSERT(mat != NULL, "MATRIX IS NULL");
    fprintf(fp, "{\n");
    for (size_t i = 0; i < mat->rows; ++i) {
        fprintf(fp, "    ");
        for (size_t j = 0; j < mat->cols; ++j) {
            NN_DATA_TYPE val = NN_MAT_AT(mat, i, j);
            fprintf(fp, NN_DATA_FORMAT ", ",  val);
        }
        fprintf(fp, "\n");

    }
    fprintf(fp, "}\n");
}


int mat_init(mat *mat, size_t rows, size_t cols, NN_DATA_TYPE *es)
{
    NN_ASSERT(es != NULL, "Table is NULL");
    mat->cols = cols;
    mat->rows = rows;
    mat->es = es;
    return mat->es != NULL;
}

void mat_transpose(mat *m, mat *out)
{
    NN_ASSERT(m != NULL, "MATRIX 1 IS NULL");
    NN_ASSERT(out != NULL, "OUT IS NULL");
    NN_ASSERT(m->cols == out->rows && m->rows == out->cols, "MATRICES SIZE MISMATCH");
    
    for (size_t i = 0; i < out->rows; ++i) {
        for (size_t j = 0; j < out->cols; ++j) {
            NN_MAT_AT((out), i, j) = NN_MAT_AT((m), j, i);
        }
    }
}

void mat_slice(mat *m, size_t row1, size_t row2, size_t col1, size_t col2, mat *out)
{
    NN_ASSERT(row2 > row1, "INVALID ROW: Row 1 smaller than Row 2");
    NN_ASSERT(col2 > col1, "INVALID COL: Col 1 smaller than Col 2");
    NN_ASSERT(out->cols = col2 - col1, "INVALID OUT MATRIX SIZE");
    NN_ASSERT(out->rows = row2 - row1, "INVALID OUT MATRIX SIZE");
    NN_ASSERT(row1 >= 0 && row1 < m->rows && row2 >= 0 && row2 <= m->rows, "INVALID M MATRIX SIZE");
    NN_ASSERT(col1 >= 0 && col1 < m->cols && col2 >= 0 && col2 <= m->cols, "INVALID M MATRIX SIZE");
    NN_ASSERT(out->es != NULL, "INVALID ELEMENT BUFFER");
    NN_ASSERT(m->es != NULL, "INVALID ELEMENT BUFFER");

    for (size_t i = row1; i < row2 + 1; ++i) {
        for (size_t j = col1; j < col2 + 1; ++j) {
            NN_MAT_AT((out), (i - row1), (j - col1)) = NN_MAT_AT((m), i, j);
        }
    }
}

#endif // NN_MAT_IMPLEMENTATION
#endif // NN_MAT_H
