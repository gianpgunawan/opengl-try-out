#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define ARENA_IMPLEMENTATION
#define CAMERA_IMPLEMENTATION
#define MATRIX_IMPLEMENTATION
#define MATRIX_DYN_IMPLEMENTATION
#define MAT_PIPELINE_HELPER_IMPLEMENTATION
#define NOB_IMPLEMENTATION
#define RENDERER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "arena.h"
#include "box.h"
#include "camera.h"
#include "matrices/matrix.h"
#include "matrices/matrix_dyn.h"
#include "matrices/mat_pipeline_helper.h"
#include "nob.h"
#include "renderer.h"
#include "thirdparty/stb_image.h"

#define TEXTURE0_FILE_NAME "./assets/wall.jpg"
#define TEXTURE1_FILE_NAME "./assets/scenery.jpg"

const char *vertext_shader_path = "./shaders/box.vert";
const char *fragment_shader_path = "./shaders/texture.frag";

#define WIDTH 800
#define HEIGHT 600

#define ARENA_SIZE 256 * 1024 * 1024
Arena arena = {0};

Camera camera = {0};

#define N 0.1f
#define F 100.0f
#define R 0.1f
#define T 0.1f

uint8_t *buffer;
int x, y, n_channels;
float n_deg = 0.0f;

extern float vertices[];


static inline float degree_to_radian(float degree)
{
    return degree / 360.0f * (2.0f * M_PI);
}

void load_image(uint8_t **buffer, int *x, int *y, int *n_channels, const char *file_path)
{
    *buffer = NULL;
    *buffer = stbi_load(file_path, x, y, n_channels, 0);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", file_path);
        exit(1);
    }
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
    (void)window;
    (void)is_shift;
    (void)scancode;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    }

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        MAT_AT(&camera.eye, 2, 0) += 0.2f;
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        MAT_AT(&camera.eye, 2, 0) -= 0.2f;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        MAT_AT(&camera.eye, 0, 0) -= 0.2f;
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        MAT_AT(&camera.eye, 0, 0) += 0.2f;
    }
}

int main()
{
    arena_init(&arena, ARENA_SIZE);
    camera_init(&camera, &arena);

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

    load_image(&buffer, &x, &y, &n_channels, TEXTURE0_FILE_NAME);
    renderer_set_texture0(&renderer, buffer, x, y);
    stbi_image_free(buffer);

    load_image(&buffer, &x, &y, &n_channels, TEXTURE1_FILE_NAME);
    renderer_set_texture1(&renderer, buffer, x, y);
    stbi_image_free(buffer);

    GLuint vertext_shader;
    GLuint fragment_shader;
    compile_shader_file(vertext_shader_path, GL_VERTEX_SHADER, &vertext_shader);
    compile_shader_file(fragment_shader_path, GL_FRAGMENT_SHADER, &fragment_shader);

    GLuint program;
    link_program(vertext_shader, fragment_shader, &program);
     
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    mat scale = mat_scaling(&arena, 2.0f);
    mat proj = mat_projection(&arena, F, N, T, R);
    // mat trans = mat_translation(&arena, 0.0f, 0.0f, -3.0f);
    
    while (!glfwWindowShouldClose(window)) {
        size_t checkpoint = arena.count;

        n_deg += 0.1f;
        float degree = degree_to_radian(n_deg);
        mat rot_y = mat_rotate(&arena, degree, MAT_ROTATE_Y);
        mat rot_x = mat_rotate(&arena, degree, MAT_ROTATE_X);
        mat rot_z = mat_rotate(&arena, degree, MAT_ROTATE_Z);

        mat view = mat_look_at(&arena, &camera.eye, &camera.center, &camera.up);
        mat tmp = mdyn_mul(&arena, &proj, &view);
//        tmp = mdyn_mul(&arena, &tmp, &rot_x);
//        tmp = mdyn_mul(&arena, &tmp, &rot_y);
        tmp = mdyn_mul(&arena, &tmp, &rot_z);
        tmp = mdyn_mul(&arena, &tmp, &scale);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);

        glBindVertexArray(renderer.vao);
        glUniformMatrix4fv(glGetUniformLocation(program, "mvc"),
                           1,
                           GL_TRUE,
                           tmp.es);

        glUniform1i(glGetUniformLocation(program, "brick"),
                    0);
        glUniform1i(glGetUniformLocation(program, "scenery"),
                    1);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        arena_reset_to(&arena, checkpoint);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
