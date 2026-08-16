#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct {
    GLuint vbo;
    GLuint vao;
    GLuint texture0;
    GLuint texture1;
} Renderer;

void renderer_set_vertex(Renderer *r, float *vertex, size_t vertex_size);
void renderer_set_texture0(Renderer *r, uint8_t *buffer, int x, int y);

#ifdef RENDERER_IMPLEMENTATION

void renderer_set_vertex(Renderer *r, float *vertex, size_t vertex_size)
{
    glGenBuffers(1, &(r->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 vertex_size,
                 vertex,
                 GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &(r->vao));
    glBindVertexArray(r->vao);
    glVertexAttribPointer(0,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(float),
                          (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(float),
                          (GLvoid*)(4 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);
}

void renderer_set_texture1(Renderer *r, uint8_t *buffer, int x, int y)
{
    glGenTextures(1, &r->texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, r->texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void renderer_set_texture0(Renderer *r, uint8_t *buffer, int x, int y)
{
    glGenTextures(1, &r->texture0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, r->texture0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glGenerateMipmap(GL_TEXTURE_2D);
}

#endif // RENDERER_IMPLEMENTATION
#endif // RENDERER_H
