//#define GL_VERSION_4_0
#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <algorithm>

const float vertexPositions[] = {
        0.75f, 0.75f, 0.0f, 1.0f,
        0.75f, -0.75f, 0.0f, 1.0f,
        -0.75f, -0.75f, 0.0f, 1.0f,
};

void display() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
//    glutSwapBuffers();
}

GLuint positionBufferObject;

void initializeVertexBuffers() {
    // Create vertex buffer (just one)
    glGenBuffers(1, &positionBufferObject);

    // Bind the newly created vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);

    // Put the vertices into the vertex buffer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

    // Unbind the buffer from the context.
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}


static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

GLuint createProgram(const std::vector<GLuint> &shaderList) {
    GLuint program = glCreateProgram();

    for (unsigned int iLoop : shaderList) {
        printf("Attaching shader #%i\n", iLoop);
        glAttachShader(program, iLoop);
    }

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        auto *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, nullptr, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    for (unsigned int iLoop : shaderList) glDetachShader(program, iLoop);

    return program;
}

GLuint createShader(GLenum shaderType, const char* shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    printf("%s", shaderSource);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        auto *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, nullptr, strInfoLog);

        const char *strShaderType = nullptr;
        switch (shaderType) {
            case GL_VERTEX_SHADER:
                strShaderType = "vertex";
                break;
            case GL_GEOMETRY_SHADER:
                strShaderType = "geometry";
                break;
            case GL_FRAGMENT_SHADER:
                strShaderType = "fragment";
                break;
        }

        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
        delete[] strInfoLog;
    }

    return shader;
//    printf("%s", shaderSource.c_str());
}

GLuint initialiseProgram() {
    std::vector<GLuint> list;


    list.push_back(
            createShader(
                    GL_VERTEX_SHADER,

#include "shaders/vertex.glsl"

            )
    );

    list.push_back(
            createShader(
                    GL_FRAGMENT_SHADER,

#include "shaders/fragment.glsl"
    )
    );

    auto program = createProgram(list);
    std::for_each(list.begin(), list.end(), glDeleteShader);
    return program;
}

int main(int argc, char *argv[]) {


    GLFWwindow *window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


    window = glfwCreateWindow(640, 480, "Simple example", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
//    glViewport(0, 0, 640, 480);

    auto program = initialiseProgram();
    initializeVertexBuffers();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
//    GLuint vao;
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//    glCreateShader()
    while (!glfwWindowShouldClose(window)) {
//        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
//        ratio = width / (float) height;
        glViewport(0, 0, width, height);
//        glClearColor(0.0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
//        glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);

        glDrawArrays(GL_TRIANGLES, 0, 3);

//        glDisableVertexAttribArray(0);
//        glUseProgram(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}