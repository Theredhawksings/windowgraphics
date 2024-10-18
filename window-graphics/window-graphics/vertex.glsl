#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 Normal;
out vec3 FragPos;
out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool isAxis;  // ������ ���θ� ��Ÿ���� uniform ���� �߰�

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    if (isAxis) {
        Normal = vec3(0.0, 0.0, 0.0);  // �࿡�� ��� ������ �ʿ� ����
        Color = aColor;  // ���� ��� �Է� ������ �״�� ���
    } else {
        Normal = mat3(transpose(inverse(model))) * aNormal;  
        Color = aColor;
    }
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}