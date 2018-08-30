//#define GL_VERSION_4_0
#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL

#include "stb_image.h"
#include "glm/gtx/rotate_vector.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <algorithm>

const float vertexPositions[] = {
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
};

GLuint positionBufferObject;

void initializeVertexBuffers() {
    // Create vertex buffer (just one)
    glGenBuffers(1, &positionBufferObject);

    // Bind the newly created vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);

    // Put the vertices into the vertex buffer.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

    // Unbind the buffer from the context.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
        printf("Attaching shader #%i.\n", iLoop);
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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(0);

    auto program = initialiseProgram();
    initializeVertexBuffers();
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    auto unitMatrix = glm::mat4(1.0f);

    float rotation = 0;
    int tex_width, text_height, nrChannels;
    unsigned char *data = stbi_load("container.jpg", &tex_width, &text_height, &nrChannels, 0);


    auto viewMatrix = glm::lookAt(glm::vec3(0.0, 0.0, 5.0),
                                  glm::vec3(0.0, 0.0, 0.0),
                                  glm::vec3(0.0, 1.0, 0.0));

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        GLfloat aspectRatio = float(width) / float(height);
        glm::mat4 mat_projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 10.0f);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        if (rotation < 360) {
            rotation += 0.001;
        } else {
            rotation = 0;
        }

        auto rotationMatrix = glm::rotate(unitMatrix, glm::radians(rotation), glm::vec3(0.0f, 0.0f, -1.0f));

        auto translationMatrix = glm::translate(glm::vec3(0.0, -0.5, 0.0));

        glUseProgram(program);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        GLint matrixId = glGetUniformLocation(program, "rotationMatrix");
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) 36);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*) 36);
        glUniformMatrix4fv(matrixId, 1, GL_FALSE, glm::value_ptr(mat_projection * rotationMatrix * translationMatrix * glm::translate(glm::vec3(0.0, 0.634, 0.0))));

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glUseProgram(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}
