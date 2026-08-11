#include <stdio.h>

#define ARENA_IMPLEMENTATION
#define MATRIX_IMPLEMENTATION
#define MATRIX_DYN_IMPLEMENTATION

#include "arena.h"
#include "matrices/matrix.h"
#include "matrices/matrix_dyn.h"

#define ARENA_SIZE 256 * 1024
Arena arena = {0};

int main()
{
    arena_init(&arena, ARENA_SIZE);

    mat a = mdyn_make_mat(&arena, 4, 4, (float[]){
        2, 0, 0, 0,
        0, 2, 0, 0,
        0, 0, 2, 0,
        0, 0, 0, 2,
    });

    mat b = mdyn_make_mat(&arena, 4, 4, (float[]){
        1, 0, 0, 5,
        0, 2, 0, 0,
        0, 0, 3, 0,
        0, 0, 0, 4,
    });

    mat c = mdyn_mul(&arena, &a, &b);
    mat_print(&c);
    printf("hello, world\n");
    return 0;
}
