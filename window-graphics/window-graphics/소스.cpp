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


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    int faceIndex;
};

void initAxes() {
    float axisVertices[] = {
        -axisLength, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
        axisLength, 0.0f, 0.0f,     0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,

        0.0f, -axisLength, 0.0f,    0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,
        0.0f, axisLength, 0.0f,     0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, -axisLength,    0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, axisLength,     0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);

    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


std::array<std::array<glm::vec3, 3>, 958> sphereColors;


void initSphereColors() {
    for (auto& face : sphereColors) {
        for (auto& color : face) {
            color = glm::vec3(0.0f, 0.0f, 1.0f);  // 파란색
        }
    }
}

std::vector<Vertex> sphereVertexData;
std::vector<GLuint> sphereIndices;



std::vector<Vertex> planet1VertexData;
std::vector<GLuint> planet1Indices;
std::vector<Vertex> planet2VertexData;
std::vector<GLuint> planet2Indices;
std::vector<Vertex> planet3VertexData;
std::vector<GLuint> planet3Indices;

std::array<std::array<glm::vec3, 3>, 958> planetColors;

void initPlanetColors() {
    for (auto& face : planetColors) {
        for (auto& color : face) {
            color = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}

std::vector<Vertex> moon1VertexData;
std::vector<GLuint> moon1Indices;
std::vector<Vertex> moon2VertexData;
std::vector<GLuint> moon2Indices;
std::vector<Vertex> moon3VertexData;
std::vector<GLuint> moon3Indices;
std::array<std::array<glm::vec3, 3>, 958> moonColors;

float moonRotationAngle1 = 0.0f;
float moonRotationAngle2 = 0.0f;
float moonRotationAngle3 = 0.0f;

void initMoonColors() {
    for (auto& face : moonColors) {
        for (auto& color : face) {
            color = glm::vec3(1.0f, 0.0f, 0.0f);  
        }
    }
}

float rotationAngle1 = 0.0f;
float rotationAngle2 = 0.0f;
float rotationAngle3 = 0.0f;


// 궤도 경로를 위한 전역 변수
GLuint orbitVAO[6];
GLuint orbitVBO[6];

void initOrbitPath() {
    const int numSegments = 100;
    const float planetRadius = 2.5f;  // 행성의 공전 거리
    const float moonRadius = 0.5f;    // 달의 공전 거리
    std::vector<float> planetOrbitVertices;
    std::vector<float> moonOrbitVertices;

    // 행성 궤도와 달 궤도 모두 같은 방향(시계 방향)으로 정점 생성
    for (int i = 0; i <= numSegments; i++) {
        float angle = 2.0f * glm::pi<float>() * float(i) / float(numSegments);

        // 행성 궤도 정점
        float x = planetRadius * cos(angle);
        float z = planetRadius * sin(angle);
        planetOrbitVertices.push_back(x);
        planetOrbitVertices.push_back(0.0f);
        planetOrbitVertices.push_back(z);

        // 달 궤도 정점 (행성과 같은 방향)
        x = moonRadius * cos(angle);
        z = moonRadius * sin(angle);
        moonOrbitVertices.push_back(x);
        moonOrbitVertices.push_back(0.0f);
        moonOrbitVertices.push_back(z);
    }

    // VAO와 VBO 생성
    glGenVertexArrays(6, orbitVAO);
    glGenBuffers(6, orbitVBO);

    // 행성 궤도 버퍼 설정 (0, 1, 2번 인덱스)
    for (int i = 0; i < 3; i++) {
        glBindVertexArray(orbitVAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, orbitVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, planetOrbitVertices.size() * sizeof(float), planetOrbitVertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    // 달 궤도 버퍼 설정 (3, 4, 5번 인덱스)
    for (int i = 3; i < 6; i++) {
        glBindVertexArray(orbitVAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, orbitVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, moonOrbitVertices.size() * sizeof(float), moonOrbitVertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
}

float sceneRotationY = 0.0f;
bool isRotatingPositive = false;
bool isRotatingNegative = false;

float sceneRotationZ = 0.0f;
bool isRotatingZPositive = false;
bool isRotatingZNegative = false;

void drawOrbitPaths() {
    glm::mat4 model;
    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLuint objectColorLoc = glGetUniformLocation(shaderProgramID, "objectColor");

    // 라인 두께 설정
    glLineWidth(1.0f);

    // 행성 궤도 그리기 (흰색)
    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

    // 행성들의 궤도 그리기
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    model = glm::rotate(model, glm::radians(sceneRotationY), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 전체 회전
    model = glm::rotate(model, glm::radians(sceneRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 전체 회전

    glBindVertexArray(orbitVAO[0]);
    glDrawArrays(GL_LINE_STRIP, 0, 101);

    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(sceneRotationY), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 전체 회전
    model = glm::rotate(model, glm::radians(sceneRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 전체 회전

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(orbitVAO[1]);
    glDrawArrays(GL_LINE_STRIP, 0, 101);

    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(sceneRotationY), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 전체 회전
    model = glm::rotate(model, glm::radians(sceneRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 전체 회전

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(orbitVAO[2]);
    glDrawArrays(GL_LINE_STRIP, 0, 101);

    // 달 궤도 그리기 (빨간색)
    glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
    float planetOrbitRadius = 2.5f;

    float planet1X = planetOrbitRadius * cos(glm::radians(-rotationAngle1));
    float planet1Y = 0.0f;
    float planet1Z = planetOrbitRadius * sin(glm::radians(-rotationAngle1));

    float planet2X = planetOrbitRadius * cos(glm::radians(-rotationAngle2));
    float planet2Y = planetOrbitRadius * sin(glm::radians(45.0f)) * sin(glm::radians(rotationAngle2));
    float planet2Z = planetOrbitRadius * sin(glm::radians(-rotationAngle2)) * cos(glm::radians(45.0f));

    float planet3X = planetOrbitRadius * cos(glm::radians(-rotationAngle3));
    float planet3Y = planetOrbitRadius * sin(glm::radians(-45.0f)) * sin(glm::radians(rotationAngle3));
    float planet3Z = planetOrbitRadius * sin(glm::radians(-rotationAngle3)) * cos(glm::radians(-45.0f));

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(planet1X, planet1Y, planet1Z));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축으로 180도 회전

    model = glm::rotate(model, glm::radians(sceneRotationY), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 전체 회전
    model = glm::rotate(model, glm::radians(sceneRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 전체 회전

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(orbitVAO[3]);
    glDrawArrays(GL_LINE_STRIP, 0, 101);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(planet2X, planet2Y, planet2Z));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축으로 180도 회전

    
    model = glm::rotate(model, glm::radians(sceneRotationY), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축 전체 회전
    model = glm::rotate(model, glm::radians(sceneRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 전체 회전
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(orbitVAO[4]);
    glDrawArrays(GL_LINE_STRIP, 0, 101);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(planet3X, planet3Y, planet3Z));
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Y축으로 180도 회전

    model = glm::rotate(model, glm::radians(sceneRotationY), glm::vec3(0.0f, 1.0f, 0.0f));  // 4. Y축 전체 회전
    model = glm::rotate(model, glm::radians(sceneRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 전체 회전 추가

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(orbitVAO[5]);
    glDrawArrays(GL_LINE_STRIP, 0, 101);

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
    }

    glm::vec3 center(0.0f);
    for (const auto& v : temp_vertices) {
        center += v;
    }
    center /= temp_vertices.size();

    for (auto& v : temp_vertices) {
        v -= center;
    }

    file.clear();
    file.seekg(0);

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "f") {
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

    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, planet1VertexData.size() * sizeof(Vertex), planet1VertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, planet1Indices.size() * sizeof(GLuint), planet1Indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, planet2VertexData.size() * sizeof(Vertex), planet2VertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, planet2Indices.size() * sizeof(GLuint), planet2Indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao[3]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glBufferData(GL_ARRAY_BUFFER, planet3VertexData.size() * sizeof(Vertex), planet3VertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[7]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, planet3Indices.size() * sizeof(GLuint), planet3Indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao[4]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
    glBufferData(GL_ARRAY_BUFFER, moon1VertexData.size() * sizeof(Vertex), moon1VertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[9]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, moon1Indices.size() * sizeof(GLuint), moon1Indices.data(), GL_STATIC_DRAW);

    // 달 2
    glBindVertexArray(vao[5]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
    glBufferData(GL_ARRAY_BUFFER, moon2VertexData.size() * sizeof(Vertex), moon2VertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[11]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, moon2Indices.size() * sizeof(GLuint), moon2Indices.data(), GL_STATIC_DRAW);

    // 달 3
    glBindVertexArray(vao[6]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
    glBufferData(GL_ARRAY_BUFFER, moon3VertexData.size() * sizeof(Vertex), moon3VertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[13]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, moon3Indices.size() * sizeof(GLuint), moon3Indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void drawPlanet(int vaoIndex, float rotateX, float rotationAngle, float distance, float scale) {
    glm::mat4 model = glm::mat4(1.0f);

    
    model = glm::rotate(model, glm::radians(rotateX), glm::vec3(1.0f, 0.0f, 0.0f));  // 1. 궤도 기울기
    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));  // 2. 공전


    model = glm::translate(model, glm::vec3(distance, 0.0f, 0.0f));  // 3. 거리 이동

    model = glm::rotate(model, glm::radians(rotateX), glm::vec3(1.0f, 0.0f, 0.0f));  // 1. 궤도 기울기
    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));  // 2. 공전

    model = glm::scale(model, glm::vec3(scale));  // 5. 크기 조절

    model = glm::rotate(model, glm::radians(sceneRotationY), glm::vec3(0.0f, 1.0f, 0.0f));  // 4. Y축 전체 회전
    model = glm::rotate(model, glm::radians(sceneRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 전체 회전 추가
    

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(vao[vaoIndex]);
    glDrawElements(GL_TRIANGLES, planet1Indices.size(), GL_UNSIGNED_INT, 0);
}

void drawMoon(int vaoIndex, float parentRotateX, float parentRotationAngle, float moonRotationAngle, float parentDistance, float moonDistance, float scale) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(parentRotateX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(parentRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    model = glm::translate(model, glm::vec3(parentDistance, 0.0f, 0.0f));

    model = glm::rotate(model, glm::radians(moonRotationAngle), glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::rotate(model, -glm::radians(-parentRotateX), glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, -glm::radians(-parentRotationAngle), glm::vec3(0.0f, -1.0f, 0.0f));

    model = glm::rotate(model, glm::radians(sceneRotationY), glm::vec3(0.0f, 1.0f, 0.0f));  // 4. Y축 전체 회전
    model = glm::rotate(model, glm::radians(sceneRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 전체 회전 추가

    model = glm::translate(model, glm::vec3(moonDistance, 0.0f, 0.0f));

    

    model = glm::scale(model, glm::vec3(scale));

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(vao[vaoIndex]);
    glDrawElements(GL_TRIANGLES, moon1Indices.size(), GL_UNSIGNED_INT, 0);
}

bool isOrtho = true;  

bool isSolidModel = true;

float moveX = 0.0f;
float moveY = 0.0f;
float moveZ = 0.0f;  
const float MOVE_SPEED = 0.1f;

void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'p':
    case 'P':
        isOrtho = !isOrtho;  // 투영 방식 토글
        glutPostRedisplay();
        break;

    case 'm':
    case 'M':
        isSolidModel = !isSolidModel;
        if (isSolidModel) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        break;
    case 'w':
    case 'W':
        moveY += MOVE_SPEED;  // 위로 이동
        break;
    case 's':
    case 'S':
        moveY -= MOVE_SPEED;  // 아래로 이동
        break;
    case 'a':
    case 'A':
        moveX -= MOVE_SPEED;  // 왼쪽으로 이동
        break;
    case 'd':
    case 'D':
        moveX += MOVE_SPEED;  // 오른쪽으로 이동
        break;
    case '+':
        moveZ += MOVE_SPEED;  // 앞으로 이동
        break;
    case '-':
        moveZ -= MOVE_SPEED;  // 뒤로 이동
        break;
    case 'y':
        isRotatingPositive = !isRotatingPositive;
        isRotatingNegative = false;
        break;
    case 'Y':
        isRotatingNegative = !isRotatingNegative;
        isRotatingPositive = false;
        break;
    case 'z':
        isRotatingZPositive = !isRotatingZPositive;
        isRotatingZNegative = false;
        break;
    case 'Z':
        isRotatingZNegative = !isRotatingZNegative;
        isRotatingZPositive = false;
        break;
    }
    glutPostRedisplay();
}

void drawScene() {
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);
    glEnable(GL_DEPTH_TEST);

    if (isOrtho) {
        projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 50.0f);
    }
    else {
        projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 50.0f);
    }

    view = glm::lookAt(glm::vec3(15.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(moveX, moveY, moveZ));

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");
    GLuint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(lightPosLoc, 5.0f, 5.0f, 5.0f);

    // Y축 회전이 적용된 기본 model 행렬
    glm::mat4 baseModel = glm::rotate(glm::mat4(1.0f), glm::radians(sceneRotationY), glm::vec3(0.0f, 1.0f, 0.0f));

    // 축 그리기
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(baseModel));
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 6);

    // 중앙 구체 그리기
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(baseModel));
    glBindVertexArray(vao[0]);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

    // drawPlanet 함수 수정 필요 - Y축 회전 적용
    glm::mat4 planetModel;

    // 행성 그리기
    drawPlanet(1, 0.0f, rotationAngle1, 2.5f, 0.3f);     // 첫 번째 행성
    drawPlanet(2, 45.0f, rotationAngle2, 2.5f, 0.3f);    // 두 번째 행성
    drawPlanet(3, -45.0f, rotationAngle3, 2.5f, 0.3f);   // 세 번째 행성

    // 달들과 궤도도 동일한 방식으로 baseModel 적용
    drawMoon(4, 0.0f, rotationAngle1, moonRotationAngle1, 2.5f, 0.5f, 0.1f);
    drawMoon(5, 45.0f, rotationAngle2, moonRotationAngle2, 2.5f, 0.5f, 0.1f);
    drawMoon(6, -45.0f, rotationAngle3, moonRotationAngle3, 2.5f, 0.5f, 0.1f);

    glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 1.0f, 1.0f);
    drawOrbitPaths();

    // 솔리드/와이어프레임 모드 설정
    if (isSolidModel) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glutSwapBuffers();

    // 회전 각도 업데이트
    rotationAngle1 += 0.01f;
    rotationAngle2 += 0.02f;
    rotationAngle3 += 0.03f;
    moonRotationAngle1 += 0.01f;
    moonRotationAngle2 += 0.02f;
    moonRotationAngle3 += 0.03f;

    if (isRotatingPositive) {
        sceneRotationY += 0.05f;
    }
    if (isRotatingNegative) {
        sceneRotationY -= 0.05f;
    }

    if (isRotatingZPositive) {
        sceneRotationZ += 0.05f;  // 0.01f에서 증가
    }
    if (isRotatingZNegative) {
        sceneRotationZ -= 0.05f;  // 0.01f에서 증가
    }

    glutPostRedisplay();
}

GLvoid Reshape(int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(width, height);
    glutCreateWindow("Sphere Example");
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    initSphereColors();
    read_obj_file<958, 3>("sphere.obj", sphereVertexData, sphereIndices, sphereColors);


    initPlanetColors();
    read_obj_file<958, 3>("sphere.obj", planet1VertexData, planet1Indices, planetColors);
    read_obj_file<958, 3>("sphere.obj", planet2VertexData, planet2Indices, planetColors);
    read_obj_file<958, 3>("sphere.obj", planet3VertexData, planet3Indices, planetColors);

    initMoonColors(); 
    read_obj_file<958, 3>("sphere.obj", moon1VertexData, moon1Indices, moonColors);
    read_obj_file<958, 3>("sphere.obj", moon2VertexData, moon2Indices, moonColors);
    read_obj_file<958, 3>("sphere.obj", moon3VertexData, moon3Indices, moonColors);

    make_shaderProgram();
    InitBuffer();
    initAxes();
    initOrbitPath();
    
    glutKeyboardFunc(Keyboard);
    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMainLoop();

    return 0;
}