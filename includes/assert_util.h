#ifndef ASSERT_UTIL_H
#define ASSERT_UTIL_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define ASSERT(cond, message) do { \
    if (!(cond)) { \
        fprintf(stderr,message "\nat file " __FILE__ ":%d\n", __LINE__); \
        exit(1); \
    } \
} while(0);

#endif // ASSERT_UTIL_H
