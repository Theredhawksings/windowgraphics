#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <algorithm>

glm::mat4 view, projection;
GLint width = 1024, height = 768;
GLuint vertexShader, fragmentShader, shaderProgramID;
GLuint vao[8];
GLuint vbo[16];
GLint result;
GLchar errorLog[512];

GLuint axisVAO, axisVBO;
const float axisLength = 5.0f;

// 조명 관련 변수들
glm::vec3 lightPos(15.0f, 0.0f, 0.0f);  // x축 상에 위치
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
bool isLightOrbiting = false;
int lightOrbitDirection = 1;
float lightOrbitAngle = 0.0f;
float lightDistance = 15.0f;  // 거리 증가
GLuint lightVAO, lightVBO;

// 조명 색상 변경을 위한 변수 추가
int currentLightColor = 0;
const glm::vec3 lightColors[3] = {
    glm::vec3(1.0f, 1.0f, 1.0f),  // 흰색
    glm::vec3(1.0f, 0.0f, 0.0f),  // 빨간색
    glm::vec3(0.0f, 0.0f, 1.0f)   // 파란색
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    int faceIndex;
};

std::array<std::array<glm::vec3, 3>, 958> sphereColors;
std::vector<Vertex> sphereVertexData;
std::vector<GLuint> sphereIndices;

void initSphereColors() {
    for (auto& face : sphereColors) {
        for (auto& color : face) {
            int colorIndex = rand() % 3;
            switch (colorIndex) {
            case 0:
                color = glm::vec3(1.0f, 0.0f, 0.0f);  // 빨간색
                break;
            case 1:
                color = glm::vec3(0.0f, 1.0f, 0.0f);  // 초록색
                break;
            case 2:
                color = glm::vec3(0.0f, 0.0f, 1.0f);  // 파란색
                break;
            }
        }
    }
}

template<size_t N, size_t M>
void read_obj_file(const char* filename, std::vector<Vertex>& vertexData, std::vector<GLuint>& indices, const std::array<std::array<glm::vec3, M>, N>& colors) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::string line;
    int faceIndex = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (type == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (type == "f") {
            std::string v1, v2, v3;
            iss >> v1 >> v2 >> v3;

            auto process_vertex = [&](const std::string& v, int vertexIndex) {
                std::istringstream vss(v);
                std::string index_str;
                std::vector<int> vertex_indices;

                while (std::getline(vss, index_str, '/')) {
                    if (!index_str.empty()) {
                        vertex_indices.push_back(std::stoi(index_str) - 1);
                    }
                    else {
                        vertex_indices.push_back(-1);
                    }
                }

                Vertex vertex;
                vertex.position = temp_vertices[vertex_indices[0]];
                if (vertex_indices.size() > 2 && vertex_indices[2] != -1) {
                    vertex.normal = temp_normals[vertex_indices[2]];
                }
                vertex.color = colors[faceIndex % N][vertexIndex % M];
                vertex.faceIndex = faceIndex;
                vertexData.push_back(vertex);
                return vertexData.size() - 1;
                };

            indices.push_back(process_vertex(v1, 0));
            indices.push_back(process_vertex(v2, 1));
            indices.push_back(process_vertex(v3, 2));
            faceIndex++;
        }
    }
    file.close();
}

void initLightCube() {
    std::vector<Vertex> lightCubeData;
    std::vector<GLuint> lightIndices;
    read_obj_file<958, 3>("sphere.obj", lightCubeData, lightIndices, sphereColors);

    for (auto& vertex : lightCubeData) {
        vertex.position *= 0.5f;  // 크기를 0.5f로 증가
        vertex.color = glm::vec3(1.0f);  // 흰색
    }

    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(2, &lightVBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, lightCubeData.size() * sizeof(Vertex), lightCubeData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
}

void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'c':  // 조명 색상 변경
        currentLightColor = (currentLightColor + 1) % 3;
        lightColor = lightColors[currentLightColor];
        break;
    case 'r':  // 양의 방향 공전
        isLightOrbiting = true;
        lightOrbitDirection = 1;
        break;
    case 'R':  // 음의 방향 공전
        isLightOrbiting = true;
        lightOrbitDirection = -1;
        break;
    case 'q':  // 프로그램 종료
        glutLeaveMainLoop();
        break;
    }
    glutPostRedisplay();
}

void timer(int value) {
    if (isLightOrbiting) {
        lightOrbitAngle += 2.0f * lightOrbitDirection;
        if (lightOrbitAngle >= 360.0f) lightOrbitAngle -= 360.0f;
        if (lightOrbitAngle < 0.0f) lightOrbitAngle += 360.0f;

        lightPos.x = lightDistance * cos(glm::radians(lightOrbitAngle));
        lightPos.z = lightDistance * sin(glm::radians(lightOrbitAngle));
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void drawScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    // 카메라 위치 조정
    view = glm::lookAt(
        glm::vec3(5.0f, 10.0f, 20.0f),  // 약간 비스듬히 보도록 조정
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 50.0f);

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projLoc = glGetUniformLocation(shaderProgramID, "projection");
    GLuint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
    GLuint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    GLuint isLightLoc = glGetUniformLocation(shaderProgramID, "isLight");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // 구체들을 x축을 따라 일정 간격으로 배치
    const float spacing = 4.0f;  // 구체 간의 간격

    // 왼쪽 구체
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-spacing, 0.0f, 0.0f));
    glUniform1i(isLightLoc, 0);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(vao[0]);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

    // 중앙 구체
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

    // 오른쪽 구체
    model = glm::translate(glm::mat4(1.0f), glm::vec3(spacing, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

    // 조명 객체 그리기
    model = glm::translate(glm::mat4(1.0f), lightPos);
    glUniform1i(isLightLoc, 1);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(lightVAO);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

    glutSwapBuffers();
}

void InitBuffer() {
    glGenVertexArrays(8, vao);
    glGenBuffers(16, vbo);

    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sphereVertexData.size() * sizeof(Vertex), sphereVertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(GLuint), sphereIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

char* filetobuf(const char* file) {
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb");
    if (!fptr)
        return NULL;
    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (char*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);
    buf[length] = 0;
    return buf;
}

void make_vertexShaders() {
    GLchar* vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

    if (!result) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: Vertex shader compilation failed\n" << errorLog << std::endl;
        exit(EXIT_FAILURE);
    }
}

void make_fragmentShaders() {
    GLchar* fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: Fragment shader compilation failed\n" << errorLog << std::endl;
        exit(EXIT_FAILURE);
    }
}

void make_shaderProgram() {
    make_vertexShaders();
    make_fragmentShaders();

    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(shaderProgramID);
}

void Reshape(int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("Solar System with Lighting");
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    initSphereColors();
    read_obj_file<958, 3>("sphere.obj", sphereVertexData, sphereIndices, sphereColors);

    make_shaderProgram();
    InitBuffer();
    initLightCube();

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
