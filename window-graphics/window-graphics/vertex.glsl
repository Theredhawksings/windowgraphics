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
uniform bool isAxis;  // 축인지 여부를 나타내는 uniform 변수 추가

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    if (isAxis) {
        Normal = vec3(0.0, 0.0, 0.0);  // 축에는 노멀 정보가 필요 없음
        Color = aColor;  // 축의 경우 입력 색상을 그대로 사용
    } else {
        Normal = mat3(transpose(inverse(model))) * aNormal;  
        Color = aColor;
    }
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}