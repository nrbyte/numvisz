R"(
#version 330 core

in vec2 textureCoord;

uniform sampler2D tex;

out vec4 outColor;

void main() {
  outColor = vec4(0.0f, 0.0f, 0.0f, texture(tex, textureCoord).r);
}
)"
