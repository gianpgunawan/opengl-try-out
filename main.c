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
#define STB_IMAGE_IMPLEMENTATION

#include "arena.h"
#include "box.h"
#include "matrices/matrix.h"
#include "matrices/matrix_dyn.h"
#include "matrices/mat_pipeline_helper.h"
#include "thirdparty/stb_image.h"

#define TEXTURE_FILE_NAME "./assets/wall.jpg"

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

static inline float degree_to_radian(float degree)
{
    return degree / 360.0f * (2.0f * M_PI);
}

static void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods) {
//    static float zn = -3.0f;
//    static float xn = 0.0f;
//    static float yn = 0.0f;

//    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
//        n_deg += 1.0f;
//    }
//
//    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
//        n_deg -= 1.0f;
//    }
//
//    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
//        bool is_shift = (mods & GLFW_MOD_SHIFT);
//        fn += is_shift ? -0.1f : +0.1f;
//        set_projection_mat();
//    }
//
//    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
//        bool is_shift = (mods & GLFW_MOD_SHIFT);
//        xn += is_shift ? -0.01f : +0.01f;
//    }
//
//    if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
//        bool is_shift = (mods & GLFW_MOD_SHIFT);
//        yn += is_shift ? -0.01f : +0.01f;
//    }
//
//    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
//        bool is_shift = (mods & GLFW_MOD_SHIFT);
//        zn += is_shift ? 0.5f : -0.5f;
//    }
}

void init_box()
{
    
}

int main() {
#if(0)
#endif
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
    

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    buffer = stbi_load(TEXTURE_FILE_NAME, &x, &y, &n_channels, 0);
    if (buffer) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Failed to load image\n");
    }

// VBO
    GLuint VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

// VAO
    GLuint VAO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // FOR POSITIONS 
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // FOR TEXTURE
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(4 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec4 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"

        "uniform mat4 mvc;\n"

        "out vec3 color;\n"
        "out vec2 texCoord;\n"

        "out vec3 vNDC;\n"
        "void main() {\n"
        "   vec4 clip = mvc * aPos;\n"
        "   gl_Position = clip;\n"
        "   vec3 ndc = clip.xyz / clip.w;\n"
        "   vNDC = ndc;\n"
        "   texCoord = aTexCoord;\n"
        "}\0";

    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"

        "in vec2 texCoord;\n"
        "in vec3 vNDC;\n"

        "uniform sampler2D ourTexture;\n"

        "void main() {\n"
        "   FragColor = texture(ourTexture, texCoord);\n"
        "   // FragColor = vec4(clamp(vNDC, -1.0, 1.0) * 0.5 + 0.5, 1.0);\n" 
        "   // FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n" 
        "}\0";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint mvc_loc = glGetUniformLocation(shaderProgram, "mvc");
    GLuint texture_loc = glGetUniformLocation(shaderProgram, "ourTexture");
     
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    mat scale = mat_scaling(&arena, 2.0f);
    mat proj = mat_projection(&arena, F, N, T, R);
    mat trans = mat_translation(&arena, 0.0f, 0.0f, -3.0f);

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
        glUseProgram(shaderProgram);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

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
