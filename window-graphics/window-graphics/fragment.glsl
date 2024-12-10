#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
out vec4 FragColor;
uniform vec3 lightPos;
uniform bool ambientEnabled;  // m키 상태를 받을 uniform 변수

void main()
{
    // Ambient
    float ambientStrength = 0.1;  // 주변광 강도를 낮춤
    vec3 ambient = ambientStrength * Color;
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * Color;
    
    // Final color calculation
    vec3 result;
    if (ambientEnabled) {
        result = (ambient + diffuse) * Color * 1.5;
    } else {
        result = ambient * Color * 0.5;  // 조명이 꺼졌을 때는 주변광만 사용하고 더 어둡게
    }
    result = clamp(result, 0.0, 1.0);
    
    // 광원 큐브 처리 (발산광)
    if (length(FragPos - lightPos) < 0.5) {
        if (ambientEnabled) {
            // 발광 효과
            FragColor = vec4(vec3(2.0), 1.0);  // 밝은 발광
        } else {
            // 조명이 꺼졌을 때
            FragColor = vec4(vec3(0.1), 1.0);  // 매우 어두움
        }
    }
    // 눈송이 처리
    else if (Color.r > 0.9 && Color.g > 0.9 && Color.b > 0.9) {
        FragColor = vec4(result, 1.0);
    }
    // 통일된 분홍색 큐브들
    else if (abs(Color.r - 1.0) < 0.1 && abs(Color.g - 0.7) < 0.1 && abs(Color.b - 1.0) < 0.1) {
        if(ambientEnabled) {
            FragColor = vec4(result, 0.5);
        } else {
            FragColor = vec4(result * 0.3, 0.5);  // 조명이 꺼졌을 때 더 어둡게
        }
    }
    // 기타 다른 객체들
    else {
        if(ambientEnabled) {
            FragColor = vec4(result, 1.0);
        } else {
            FragColor = vec4(result * 0.3, 1.0);  // 조명이 꺼졌을 때 더 어둡게
        }
    }
}