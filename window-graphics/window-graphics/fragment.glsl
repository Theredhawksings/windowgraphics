#version 330 core

in vec4 vertexColor;
out vec4 color;

void main() {
    color = vertexColor;  // 버텍스 셰이더에서 전달된 색상 사용
}
