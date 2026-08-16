#ifndef MAT_H
#define MAT_H

#include <stdio.h>
#include "assert_util.h"

#define DATA_TYPE float
#define DATA_FORMAT "%f"


typedef struct {
    size_t rows;
    size_t cols;
    DATA_TYPE *es;
} mat;

#define MAT_AT(mat, row, col) ((mat)->es[(mat)->cols * (row) + (col)])

int mat_init(mat *mat, size_t rows, size_t cols, DATA_TYPE *es);
void mat_print(mat *mat);
int mat_mul(mat *m1, mat *m2, mat *out);
int mat_sub(mat *m1, mat *m2, mat *out);
int mat_add(mat *m1, mat *m2, mat *out);
int mat_hadamard(mat *m1, mat *m2, mat *out);
void mat_cross(mat *m1, mat *m2, mat *out);
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
            MAT_AT(m, i, j) = val;
        }
    }
}

void mat_mul_scalar(mat *m, float val, mat *out)
{
    ASSERT(m != NULL, "MATRIX 1 IS NULL");
    ASSERT(out != NULL, "OUT IS NULL");
    ASSERT(m->cols == out->cols && m->rows == out->rows, "MATRICES SIZE MISMATCH");
    for (size_t i = 0; i < m->rows; ++i) {
        for (size_t j = 0; j < m->cols; ++j) {
            MAT_AT(out, i, j) *= val;
        }
    }
}

void mat_normalize(mat *m1, mat *out)
{
    ASSERT(m1->cols == 1, "Operators has to be Vector");
    ASSERT(out->cols == m1->cols && out->rows == m1->rows, "mismatch vector size");
    float total = 0;
    for (size_t i = 0; i < m1->rows; ++i) {
        total += MAT_AT(m1, i, 0)*MAT_AT(m1, i, 0);
    }
    total = sqrtf(total);
    for (size_t i = 0; i < m1->rows; ++i) {
        MAT_AT(out, i, 0) = MAT_AT(m1, i, 0)/total;
    }
}

void mat_cross(mat *m1, mat *m2, mat *out)
{
    ASSERT(m1->cols == 1 && m1->rows == 3, "Operators has to be V3");
    ASSERT(m2->cols == 1 && m2->rows == 3, "Operators has to be V3");
    ASSERT(out->cols == 1 && out->rows == 3, "Operators has to be V3");
    MAT_AT(out, 0, 0) = MAT_AT(m1, 1, 0) * MAT_AT(m2, 2, 0) - MAT_AT(m1, 2, 0) * MAT_AT(m2, 1, 0);
    MAT_AT(out, 1, 0) = MAT_AT(m1, 2, 0) * MAT_AT(m2, 0, 0) - MAT_AT(m1, 0, 0) * MAT_AT(m2, 2, 0);
    MAT_AT(out, 2, 0) = MAT_AT(m1, 0, 0) * MAT_AT(m2, 1, 0) - MAT_AT(m1, 1, 0) * MAT_AT(m2, 0, 0);
}

void mat_fill_func(mat *m, float (*func)())
{
    for (size_t i = 0; i < m->rows; ++i) {
        for (size_t j = 0; j < m->cols; ++j) {
            MAT_AT(m, i, j) = func();
        }
    }
}

int mat_mul(mat *m1, mat *m2, mat *out)
{
    ASSERT(m1 != NULL, "MATRIX 1 IS NULL");
    ASSERT(m2 != NULL, "MATRIX 2 IS NULL");
    ASSERT(out != NULL, "OUT IS NULL");
    ASSERT(m1->cols == m2->rows, "MATRICES SIZE MISMATCH");
    ASSERT(out->cols == m2->cols && out->rows == m1->rows, "MATRIX OUT SIZE MISMATCH");

    for (size_t i = 0; i < m1->rows; ++i) {
        for (size_t j = 0; j < m2->cols; ++j) {
            DATA_TYPE sum = 0;
            for (size_t k = 0; k < m2->rows; ++k) {
                sum += MAT_AT(m1, i, k) * MAT_AT(m2, k, j);
            }
            MAT_AT(out, i, j) = sum;
        }
    }
    return 0;
}

int mat_add(mat *m1, mat *m2, mat *out)
{
    ASSERT(m1 != NULL, "MATRIX 1 IS NULL");
    ASSERT(m2 != NULL, "MATRIX 2 IS NULL");
    ASSERT(out != NULL, "OUT IS NULL");
    ASSERT(m1->cols == m2->cols && m1->rows == m2->rows, "MATRIX SIZE MISMATCH");
    ASSERT(out->cols == m2->cols && out->rows == m2->rows, "MATRIX OUT SIZE MISMATCH");

    for (size_t i = 0; i < out->rows; ++i) {
        for (size_t j = 0; j < out->cols; ++j) {
            MAT_AT((out), i, j) = MAT_AT((m1), i, j) + MAT_AT((m2), i, j);
        }
    }
    return 0;
}

int mat_sub(mat *m1, mat *m2, mat *out)
{
    ASSERT(m1 != NULL, "MATRIX 1 IS NULL");
    ASSERT(m2 != NULL, "MATRIX 2 IS NULL");
    ASSERT(out != NULL, "OUT IS NULL");
    ASSERT(m1->cols == m2->cols && m1->rows == m2->rows, "MATRICES SIZE MISMATCH");
    ASSERT(out->cols == m2->cols && out->rows == m2->rows, "MATRIX OUT SIZE MISMATCH");

    for (size_t i = 0; i < out->rows; ++i) {
        for (size_t j = 0; j < out->cols; ++j) {
            MAT_AT((out), i, j) = MAT_AT((m1), i, j) - MAT_AT((m2), i, j);
        }
    }
    return 0;
}

int mat_hadamard(mat *m1, mat *m2, mat *out)
{
    ASSERT(m1 != NULL, "MATRIX 1 IS NULL");
    ASSERT(m2 != NULL, "MATRIX 2 IS NULL");
    ASSERT(out != NULL, "OUT IS NULL");
    ASSERT(m1->cols == m2->cols && m1->rows == m2->rows, "MATRIX SIZE MISMATCH");
    ASSERT(out->cols == m2->cols && out->rows == m2->rows, "MATRIX OUT SIZE MISMATCH");

    for (size_t i = 0; i < out->rows; ++i) {
        for (size_t j = 0; j < out->cols; ++j) {
            MAT_AT(out, i, j) = MAT_AT(m1, i, j) * MAT_AT(m2, i, j);
        }
    }
    return 0;
}

void mat_map(mat *m1, float (*fn)(float), mat *out)
{
    ASSERT(m1 != NULL, "MATRIX 1 IS NULL");
    ASSERT(out != NULL, "OUT IS NULL");
    ASSERT(out->cols == m1->cols && out->rows == m1->rows, "MATRIX OUT SIZE MISMATCH");
    for (size_t i = 0; i < out->rows; ++i) {
        for (size_t j = 0; j < out->cols; ++j) {
            MAT_AT(out, i, j) = fn(MAT_AT(m1, i, j));
        }
    }
}

void mat_print(mat *mat)
{
    ASSERT(mat != NULL, "MATRIX IS NULL");
    printf("{\n");
    for (size_t i = 0; i < mat->rows; ++i) {
        printf("    ");
        for (size_t j = 0; j < mat->cols; ++j) {
            DATA_TYPE val = MAT_AT(mat, i, j);
            printf(DATA_FORMAT ", ",  val);
        }
        printf("\n");

    }
    printf("}\n");
}

void mat_fprintf(mat *mat, FILE *fp)
{
    ASSERT(mat != NULL, "MATRIX IS NULL");
    fprintf(fp, "{\n");
    for (size_t i = 0; i < mat->rows; ++i) {
        fprintf(fp, "    ");
        for (size_t j = 0; j < mat->cols; ++j) {
            DATA_TYPE val = MAT_AT(mat, i, j);
            fprintf(fp, DATA_FORMAT ", ",  val);
        }
        fprintf(fp, "\n");

    }
    fprintf(fp, "}\n");
}


int mat_init(mat *mat, size_t rows, size_t cols, DATA_TYPE *es)
{
    ASSERT(es != NULL, "Table is NULL");
    mat->cols = cols;
    mat->rows = rows;
    mat->es = es;
    return mat->es != NULL;
}

void mat_transpose(mat *m, mat *out)
{
    ASSERT(m != NULL, "MATRIX 1 IS NULL");
    ASSERT(out != NULL, "OUT IS NULL");
    ASSERT(m->cols == out->rows && m->rows == out->cols, "MATRICES SIZE MISMATCH");

    for (size_t i = 0; i < out->rows; ++i) {
        for (size_t j = 0; j < out->cols; ++j) {
            MAT_AT((out), i, j) = MAT_AT((m), j, i);
        }
    }
}

void mat_slice(mat *m, size_t row1, size_t row2, size_t col1, size_t col2, mat *out)
{
    ASSERT(row2 > row1, "INVALID ROW: Row 1 smaller than Row 2");
    ASSERT(col2 > col1, "INVALID COL: Col 1 smaller than Col 2");
    ASSERT(out->cols = col2 - col1, "INVALID OUT MATRIX SIZE");
    ASSERT(out->rows = row2 - row1, "INVALID OUT MATRIX SIZE");
    ASSERT(row1 < m->rows && row2 <= m->rows, "INVALID M MATRIX SIZE");
    ASSERT(col1 < m->cols && col2 <= m->cols, "INVALID M MATRIX SIZE");
    ASSERT(out->es != NULL, "INVALID ELEMENT BUFFER");
    ASSERT(m->es != NULL, "INVALID ELEMENT BUFFER");

    for (size_t i = row1; i < row2 + 1; ++i) {
        for (size_t j = col1; j < col2 + 1; ++j) {
            MAT_AT((out), (i - row1), (j - col1)) = MAT_AT((m), i, j);
        }
    }
}

#endif // MAT_IMPLEMENTATION
#endif // MAT_H
