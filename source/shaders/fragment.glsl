R"(
#version 330

out vec4 outputColor;
uniform sampler2D ourTexture;
in vec2 TexCoord;
void main() {
    outputColor = texture(ourTexture, TexCoord);

}
)"