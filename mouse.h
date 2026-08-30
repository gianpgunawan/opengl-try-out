#ifndef MOUSE_H
#define MOUSE_H

#include <stdbool.h>

typedef struct {
    double last_x;
    double last_y;
    bool is_drag;
} Mouse;

void mouse_init(Mouse *m);

#ifdef MOUSE_IMPLEMENTATION
#endif // MOUSE_IMPLEMENTATION

#endif // MOUSE_H
