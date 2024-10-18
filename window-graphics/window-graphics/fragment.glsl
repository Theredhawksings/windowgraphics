#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 Color;

uniform vec3 lightPos = vec3(5.0, 5.0, 5.0);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform bool isAxis;

void main()
{
    if (isAxis) {
        FragColor = vec4(Color, 1.0);  // 축의 경우 단순히 색상만 출력
    } else {
        // ambient
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * lightColor;
  	
        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
            
        vec3 result = (ambient + diffuse) * Color;
        FragColor = vec4(result, 1.0);
    }
}