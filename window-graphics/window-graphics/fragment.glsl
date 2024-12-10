#version 330 core
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture1;  // 텍스처 샘플러

void main() {
    vec4 texColor = texture(texture1, TexCoords);
    FragColor = texColor;  // 텍스처 색상만 사용
}