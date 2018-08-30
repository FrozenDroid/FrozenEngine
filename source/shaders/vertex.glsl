R"(
#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

smooth out vec3 theColor;
uniform mat4 rotationMatrix;

void main() {
    //gl_Position = position;
    gl_Position = rotationMatrix * vec4(position, 1.0f);
    theColor = color;
}
)"