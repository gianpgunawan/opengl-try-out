#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"

#define TEXTURE_FILE_NAME "./assets/wall.jpg"

#define WIDTH 800
#define HEIGHT 600

#define N 0.1f
#define F 100.0f
#define R 0.1f
#define T 0.1f

float fn = N;
float ff = F;
float fr = R;
float ft = T;

uint8_t *buffer;
int x, y, n_channels;
float n_deg = 0.0f;

GLfloat MAT_PROJECTION[] = {
    N/R, 0.0f, 0.0f, 0.0f,
    0.0f, N/T, 0.0f, 0.0f,
    0.0f, 0.0f, -(F + N)/(F - N), (-2.0f*(F*N))/(F - N),
    0.0f, 0.0f, -1.0f, 0.0f
};

GLfloat MAT_TRANSLATION[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

GLfloat MAT_SCALING[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

static inline float degree_to_radian(float degree)
{
    return degree / 360.0f * (2.0f * M_PI);
}

void set_projection_mat()
{
    int i = 0;
    int j = 1*4 + 1;
    int k = 2*4 + 2;
    int k2 = 2*4 + 3;
    MAT_PROJECTION[i] = fn/fr;
    MAT_PROJECTION[j] = fn/ft;
    MAT_PROJECTION[k] = -(ff + fn)/(ff - fn);
    MAT_PROJECTION[k2] = (-2.0f*(ff*fn))/(ff - fn);
}

void set_scaling_mat(float n)
{
    int i = 0;
    int j = 1*4 + 1;
    int k = 2*4 + 2;
    MAT_SCALING[i] = n;
    MAT_SCALING[j] = n;
    MAT_SCALING[k] = n;
}

void set_translation_mat(float x, float y, float z)
{
    int i = 3;
    int j = 1*4 + 3;
    int k = 2*4 + 3;
    MAT_TRANSLATION[i] = x;
    MAT_TRANSLATION[j] = y;
    MAT_TRANSLATION[k] = z;
}

int print_mat4()
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int pos = i * 4 + j;
            printf("%f ", MAT_TRANSLATION[pos]);
        }
        printf("\n");
    }
    return 0;
}

static void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static float zn = -3.0f;
    static float xn = 0.0f;
    static float yn = 0.0f;

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        n_deg += 1.0f;
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        n_deg -= 1.0f;
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        bool is_shift = (mods & GLFW_MOD_SHIFT);
        fn += is_shift ? -0.1f : +0.1f;
        set_projection_mat();
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        bool is_shift = (mods & GLFW_MOD_SHIFT);
        xn += is_shift ? -0.01f : +0.01f;
        set_translation_mat(xn, 0.0f, zn);
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
        bool is_shift = (mods & GLFW_MOD_SHIFT);
        yn += is_shift ? -0.01f : +0.01f;
        set_translation_mat(xn, yn, zn);
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        bool is_shift = (mods & GLFW_MOD_SHIFT);
        zn += is_shift ? 0.5f : -0.5f;
        set_translation_mat(xn, 0.0f, zn);
    }
}

int main() {
#if(0)
    set_translation_mat(0.0f, 0.0f, -3.0f);
    set_scaling_mat(2.0f);
    print_mat4(); 
    exit(1);
#endif

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

    float z_val_front = 0.5f;
    float z_val_back  = -0.5f;
    float w_val = 1.0f;

    float vertices[] = {
        -0.5f, -0.5f, z_val_front, w_val, 0.0f, 0.0f,   
         0.5f, -0.5f, z_val_front, w_val, 1.0f, 0.0f,
         0.5f,  0.5f, z_val_front, w_val, 1.0f, 1.0f,
         0.5f,  0.5f, z_val_front, w_val, 1.0f, 1.0f,
        -0.5f,  0.5f, z_val_front, w_val, 0.0f, 1.0f,
        -0.5f, -0.5f, z_val_front, w_val, 0.0f, 0.0f,

        -0.5f, -0.5f, z_val_back,  w_val, 0.0f, 0.0f,
         0.5f, -0.5f, z_val_back,  w_val, 1.0f, 0.0f,
         0.5f,  0.5f, z_val_back,  w_val, 1.0f, 1.0f,
         0.5f,  0.5f, z_val_back,  w_val, 1.0f, 1.0f,
        -0.5f,  0.5f, z_val_back,  w_val, 0.0f, 1.0f,
        -0.5f, -0.5f, z_val_back,  w_val, 0.0f, 0.0f,

        -0.5f,  0.5f,  z_val_back, w_val, 1.0f, 0.0f,
        -0.5f,  0.5f, z_val_front, w_val, 1.0f, 1.0f,
        -0.5f, -0.5f, z_val_front, w_val, 0.0f, 1.0f,
        -0.5f, -0.5f, z_val_front, w_val, 0.0f, 1.0f,
        -0.5f, -0.5f, z_val_back,  w_val, 0.0f, 0.0f,
        -0.5f,  0.5f, z_val_back,  w_val, 1.0f, 0.0f,

         0.5f,  0.5f, z_val_back,  w_val, 1.0f, 0.0f,
         0.5f,  0.5f, z_val_front, w_val, 1.0f, 1.0f,
         0.5f, -0.5f, z_val_front, w_val, 0.0f, 1.0f,
         0.5f, -0.5f, z_val_front, w_val, 0.0f, 1.0f,
         0.5f, -0.5f, z_val_back,  w_val, 0.0f, 0.0f,
         0.5f,  0.5f, z_val_back,  w_val, 1.0f, 0.0f,

        -0.5f, -0.5f, z_val_front, w_val, 0.0f, 1.0f,
         0.5f, -0.5f, z_val_front, w_val, 1.0f, 1.0f,
         0.5f, -0.5f, z_val_back,  w_val, 1.0f, 0.0f,
         0.5f, -0.5f, z_val_back,  w_val, 1.0f, 0.0f,
        -0.5f, -0.5f, z_val_back,  w_val, 0.0f, 0.0f,
        -0.5f, -0.5f, z_val_front, w_val, 0.0f, 1.0f,

        -0.5f,  0.5f, z_val_front, w_val, 0.0f, 1.0f,
         0.5f,  0.5f, z_val_front, w_val, 1.0f, 1.0f,
         0.5f,  0.5f, z_val_back,  w_val, 1.0f, 0.0f,
         0.5f,  0.5f, z_val_back,  w_val, 1.0f, 0.0f,
        -0.5f,  0.5f, z_val_back,  w_val, 0.0f, 0.0f,
        -0.5f,  0.5f, z_val_front, w_val, 0.0f, 1.0f
    };

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

        "uniform mat4 mat_rotate_y;\n"
        "uniform mat4 mat_rotate_x;\n"
        "uniform mat4 mat_projection;\n"
        "uniform mat4 mat_translation;\n"
        "uniform mat4 mat_scaling;\n"

        "out vec3 color;\n"
        "out vec2 texCoord;\n"

        "out vec3 vNDC;\n"
        "void main() {\n"
        "   vec4 clip = mat_projection * mat_translation * mat_rotate_x * mat_rotate_y * mat_scaling * aPos;\n"
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
    GLint ok = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        printf("ERRRORRRRJ\n");
        exit(1);
        // glGetShaderInfoLog(...) and print
    }
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint mat_rotate_y = glGetUniformLocation(shaderProgram, "mat_rotate_y");
    GLuint mat_rotate_x = glGetUniformLocation(shaderProgram, "mat_rotate_x");
    GLuint mat_projection_loc = glGetUniformLocation(shaderProgram, "mat_projection");
    GLuint mat_translation_loc = glGetUniformLocation(shaderProgram, "mat_translation");
    GLuint mat_scaling_loc = glGetUniformLocation(shaderProgram, "mat_scaling");

    GLuint texture_loc = glGetUniformLocation(shaderProgram, "ourTexture");
     
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    set_translation_mat(0.0f, 0.0f, -3.0f);
    set_scaling_mat(2.0f);

    while (!glfwWindowShouldClose(window)) {
        n_deg += 0.1f;
        float degree = degree_to_radian(n_deg);
        float cos_val = cos(degree);
        float sin_val = sin(degree);

        const GLfloat matrix_y[] = {
            cos_val, 0.0f, sin_val, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            -sin_val, 0.0f, cos_val, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };

        const GLfloat matrix_x[] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cos_val, -sin_val, 0.0f,
            0.0f, sin_val, cos_val, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        glUniformMatrix4fv(mat_rotate_y, 1, GL_TRUE, matrix_y);
        glUniformMatrix4fv(mat_rotate_x, 1, GL_TRUE, matrix_x);
        glUniformMatrix4fv(mat_projection_loc, 1, GL_TRUE, MAT_PROJECTION);
        glUniformMatrix4fv(mat_translation_loc, 1, GL_TRUE, MAT_TRANSLATION);
        glUniformMatrix4fv(mat_scaling_loc, 1, GL_TRUE, MAT_SCALING);

        glUniform1i(texture_loc, 0);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
