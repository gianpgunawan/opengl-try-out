#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define ARENA_IMPLEMENTATION
#define MATRIX_IMPLEMENTATION
#define MATRIX_DYN_IMPLEMENTATION
#define MAT_PIPELINE_HELPER_IMPLEMENTATION
#define NOB_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "arena.h"
#include "box.h"
#include "matrices/matrix.h"
#include "matrices/matrix_dyn.h"
#include "matrices/mat_pipeline_helper.h"
#include "nob.h"
#include "thirdparty/stb_image.h"

#define TEXTURE_FILE_NAME "./assets/wall.jpg"

const char *vertext_shader_path = "./shaders/box.vert";
const char *fragment_shader_path = "./shaders/texture.frag";

#define WIDTH 800
#define HEIGHT 600

#define ARENA_SIZE 256 * 1024 * 1024
Arena arena = {0};

#define N 0.1f
#define F 100.0f
#define R 0.1f
#define T 0.1f

uint8_t *buffer;
int x, y, n_channels;
float n_deg = 0.0f;

extern float vertices[];

typedef struct {
    GLuint vbo;
    GLuint vao;
} Renderer;

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

static inline float degree_to_radian(float degree)
{
    return degree / 360.0f * (2.0f * M_PI);
}

const char *shader_type_as_cstr(GLuint shader)
{
    switch (shader) {
    case GL_VERTEX_SHADER:
        return "GL_VERTEX_SHADER";
    case GL_FRAGMENT_SHADER:
        return "GL_FRAGMENT_SHADER";
    default:
        return "(Unknown)";
    }
}

bool compile_shader_source(const GLchar *source, GLenum shader_type, GLuint *shader)
{
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    GLint compiled = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &message_size, message);
        fprintf(stderr, "ERROR: could not compile %s\n", shader_type_as_cstr(shader_type));
        fprintf(stderr, "%.*s\n", message_size, message);
        return false;
    }

    return true;
}

bool compile_shader_file(const char *file_path, GLenum shader_type, GLuint *shader)
{
    String_Builder sb = {0};
    if (!read_entire_file(file_path, &sb)) {
        fprintf(stderr, "ERROR: failed to read file `%s`: %s\n", file_path, strerror(errno));
        errno = 0;
        return false;
    }
    char *source = (char *)temp_sv_to_cstr(sb_to_sv(sb));
    bool ok = compile_shader_source(source, shader_type, shader);
    if (!ok) {
        fprintf(stderr, "ERROR: failed to compile `%s` shader file\n", file_path);
    }
    return ok;
}

bool link_program(GLuint vert_shader, GLuint frag_shader, GLuint *program)
{
    *program = glCreateProgram();

    glAttachShader(*program, vert_shader);
    glAttachShader(*program, frag_shader);
    glLinkProgram(*program);

    GLint linked = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLsizei message_size = 0;
        GLchar message[1024];

        glGetProgramInfoLog(*program, sizeof(message), &message_size, message);
        fprintf(stderr, "Program Linking: %.*s\n", message_size, message);
    }

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    return program;
}

static void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods) {
    bool is_shift = (mods & GLFW_MOD_SHIFT);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    }
}

int main()
{
    arena_init(&arena, ARENA_SIZE);
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello world", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_cb);

    if (glewInit() != GLEW_OK) {
        printf("GLEW failed\n");
        return -1;
    }

    Renderer renderer = {0};
    renderer_set_vertex(&renderer, vertices, sizeof(vertices));

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    buffer = stbi_load(TEXTURE_FILE_NAME, &x, &y, &n_channels, 0);
    if (buffer) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Failed to load image\n");
    }

    GLuint vertext_shader;
    GLuint fragment_shader;

    compile_shader_file(vertext_shader_path, GL_VERTEX_SHADER, &vertext_shader);
    compile_shader_file(fragment_shader_path, GL_FRAGMENT_SHADER, &fragment_shader);

    GLuint program;
    link_program(vertext_shader, fragment_shader, &program);

    GLuint mvc_loc = glGetUniformLocation(program, "mvc");
    GLuint texture_loc = glGetUniformLocation(program, "ourTexture");
     
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    mat scale = mat_scaling(&arena, 2.0f);
    mat proj = mat_projection(&arena, F, N, T, R);
    mat trans = mat_translation(&arena, 0.0f, 0.0f, -3.0f);

    printf("VAO => %u, VBO => %u\n", renderer.vao, renderer.vbo);

    while (!glfwWindowShouldClose(window)) {
        size_t checkpoint = arena.count;

        n_deg += 0.1f;
        float degree = degree_to_radian(n_deg);
        mat rot_y = mat_rotate(&arena, degree, MAT_ROTATE_Y);
        mat rot_x = mat_rotate(&arena, degree, MAT_ROTATE_X);
        mat rot_z = mat_rotate(&arena, degree, MAT_ROTATE_Z);

        mat tmp = mdyn_mul(&arena, &proj, &trans);
        tmp = mdyn_mul(&arena, &tmp, &rot_x);
        tmp = mdyn_mul(&arena, &tmp, &rot_y);
        tmp = mdyn_mul(&arena, &tmp, &rot_z);
        tmp = mdyn_mul(&arena, &tmp, &scale);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(renderer.vao);

        glUniformMatrix4fv(mvc_loc, 1, GL_TRUE, tmp.es);
        arena_reset_to(&arena, checkpoint);

        glUniform1i(texture_loc, 0);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
