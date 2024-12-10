#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
out vec4 FragColor;
uniform vec3 lightPos;
uniform bool ambientEnabled;  // mŰ ���¸� ���� uniform ����

void main()
{
    // Ambient
    float ambientStrength = 0.1;  // �ֺ��� ������ ����
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
        result = ambient * Color * 0.5;  // ������ ������ ���� �ֺ����� ����ϰ� �� ��Ӱ�
    }
    result = clamp(result, 0.0, 1.0);
    
    // ���� ť�� ó�� (�߻걤)
    if (length(FragPos - lightPos) < 0.5) {
        if (ambientEnabled) {
            // �߱� ȿ��
            FragColor = vec4(vec3(2.0), 1.0);  // ���� �߱�
        } else {
            // ������ ������ ��
            FragColor = vec4(vec3(0.1), 1.0);  // �ſ� ��ο�
        }
    }
    // ������ ó��
    else if (Color.r > 0.9 && Color.g > 0.9 && Color.b > 0.9) {
        FragColor = vec4(result, 1.0);
    }
    // ���ϵ� ��ȫ�� ť���
    else if (abs(Color.r - 1.0) < 0.1 && abs(Color.g - 0.7) < 0.1 && abs(Color.b - 1.0) < 0.1) {
        if(ambientEnabled) {
            FragColor = vec4(result, 0.5);
        } else {
            FragColor = vec4(result * 0.3, 0.5);  // ������ ������ �� �� ��Ӱ�
        }
    }
    // ��Ÿ �ٸ� ��ü��
    else {
        if(ambientEnabled) {
            FragColor = vec4(result, 1.0);
        } else {
            FragColor = vec4(result * 0.3, 1.0);  // ������ ������ �� �� ��Ӱ�
        }
    }
}