#version 330 core
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture1;  // �ؽ�ó ���÷�

void main() {
    vec4 texColor = texture(texture1, TexCoords);
    FragColor = texColor;  // �ؽ�ó ���� ���
}