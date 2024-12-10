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

GLint width, height;
GLuint vertexShader, fragmentShader, shaderProgramID;
GLuint vao[8];
GLuint vbo[16];
GLint result;
GLchar errorLog[512];

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    int faceIndex;
};
bool isRotating = false;  // y축 회전 상태
float rotationAngle = 0.0f;  // 전체 회전 각도

GLuint snowVAO, snowVBO[2];  // 눈송이용 VAO, VBO

GLuint axisVAO, axisVBO;
const float axisLength = 5.0f;

struct Snowflake {
    glm::vec3 position;
    float speed;
    bool active;
};
std::vector<Vertex> snowVertexData;
std::vector<GLuint> snowIndices;

const int NUM_SNOWFLAKES = 100;
std::vector<Snowflake> snowflakes;
bool isSnowing = false;
const float SNOW_AREA = 10.0f;
const float SNOW_HEIGHT = 8.0f;

void initSnowflakes() {
    snowflakes.clear();
    snowVertexData.clear();
    snowIndices.clear();

    for (int i = 0; i < NUM_SNOWFLAKES; i++) {
        Snowflake flake;
        flake.position = glm::vec3(
            ((float)rand() / RAND_MAX) * SNOW_AREA - SNOW_AREA / 2,
            SNOW_HEIGHT + ((float)rand() / RAND_MAX) * 5.0f,
            ((float)rand() / RAND_MAX) * SNOW_AREA - SNOW_AREA / 2
        );
        flake.speed = 0.02f + ((float)rand() / RAND_MAX) * 0.03f;
        flake.active = true;
        snowflakes.push_back(flake);

        Vertex v1, v2, v3, v4;
        v1.position = glm::vec3(-0.05f, 0.0f, -0.05f);
        v2.position = glm::vec3(0.05f, 0.0f, -0.05f);
        v3.position = glm::vec3(0.05f, 0.0f, 0.05f);
        v4.position = glm::vec3(-0.05f, 0.0f, 0.05f);

        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        v1.color = color; v2.color = color; v3.color = color; v4.color = color;

        snowVertexData.push_back(v1);
        snowVertexData.push_back(v2);
        snowVertexData.push_back(v3);
        snowVertexData.push_back(v4);

        GLuint baseIndex = i * 4;
        snowIndices.push_back(baseIndex);
        snowIndices.push_back(baseIndex + 1);
        snowIndices.push_back(baseIndex + 2);
        snowIndices.push_back(baseIndex);
        snowIndices.push_back(baseIndex + 2);
        snowIndices.push_back(baseIndex + 3);
    }

    glBindVertexArray(snowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, snowVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, snowVertexData.size() * sizeof(Vertex), snowVertexData.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, snowVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, snowIndices.size() * sizeof(GLuint), snowIndices.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
}




glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 15.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


glm::mat4 model, view, projection;

glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 2.0f);

bool ambientEnabled = true;
int currentLightColor = 0;
bool isLightRotating = false;
float lightRotationAngle = 0.0f;

int sierpinskiDepth = 0;

float lightIntensity = 1.0f;

const std::vector<glm::vec3> lightColors = {
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f)  // 초록색
};



std::vector<Vertex> cube1VertexData;
std::vector<GLuint> cube1Indices;
std::vector<Vertex> cube2VertexData;
std::vector<GLuint> cube2Indices;
std::vector<Vertex> cube3VertexData;
std::vector<GLuint> cube3Indices;
std::vector<Vertex> cube4VertexData;
std::vector<GLuint> cube4Indices;
std::vector<Vertex> cube5VertexData;
std::vector<GLuint> cube5Indices;
std::vector<Vertex> cube6VertexData;
std::vector<GLuint> cube6Indices;
std::vector<Vertex> cube7VertexData;
std::vector<GLuint> cube7Indices;
std::vector<Vertex> cube8VertexData;
std::vector<GLuint> cube8Indices;
GLuint lightVAO, lightVBO;

// face colors 생성 함수 수정
std::array<std::array<glm::vec3, 4>, 6> createFaceColors(glm::vec3 color) {
    std::array<std::array<glm::vec3, 4>, 6> faceColors;
    for (int i = 0; i < 6; i++) {  // 6으로 수정
        for (int j = 0; j < 4; j++) {
            faceColors[i][j] = color;
        }
    }
    return faceColors;
}

// 각 큐브의 색상을 정의
std::array<std::array<glm::vec3, 4>, 6> face1Colors = createFaceColors(glm::vec3(1.0f, 0.0f, 0.0f));    // 빨간색 바닥
std::array<std::array<glm::vec3, 4>, 6> face2Colors = createFaceColors(glm::vec3(1.0f, 1.0f, 0.0f));    // 노란색 피라미드
std::array<std::array<glm::vec3, 4>, 6> face3Colors = createFaceColors(glm::vec3(1.0f, 0.7f, 1.0f));    // 분홍색 큐브1
std::array<std::array<glm::vec3, 4>, 6> face4Colors = createFaceColors(glm::vec3(1.0f, 0.7f, 1.0f));    // 연한 분홍색 큐브2
std::array<std::array<glm::vec3, 4>, 6> face5Colors = createFaceColors(glm::vec3(1.0f, 0.7f, 1.0f));    // 하늘색 큐브3
std::array<std::array<glm::vec3, 4>, 6> face6Colors = createFaceColors(glm::vec3(1.0f, 0.7f, 1.0f));    // 보라색 큐브4
std::array<std::array<glm::vec3, 4>, 6> face7Colors = createFaceColors(glm::vec3(1.0f, 0.7f, 1.0f));    // ??? 큐브6
std::array<std::array<glm::vec3, 4>, 6> face8Colors = createFaceColors(glm::vec3(1.0f, 1.0f, 1.0f));    // 하얀색 광원





void initAxes() {
    float axisVertices[] = {
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,         1.0f, 0.0f, 0.0f,  // x축 (빨간색)
        axisLength, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,         0.0f, 1.0f, 0.0f,  // y축 (초록색)
        0.0f, axisLength, 0.0f, 0.0f, 0.0f, 0.0f,      0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,         0.0f, 0.0f, 1.0f,  // z축 (파란색)
        0.0f, 0.0f, axisLength, 0.0f, 0.0f, 0.0f,      0.0f, 0.0f, 1.0f
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



void generateSierpinskiTriangle(std::vector<glm::vec3>& vertices, std::vector<int>& indices,
    glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int depth) {
    if (depth == 0) {
        // 현재 삼각형의 정점 인덱스 추가
        int baseIndex = vertices.size();
        vertices.push_back(p1);
        vertices.push_back(p2);
        vertices.push_back(p3);
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
    }
    else {
        // 중점 계산
        glm::vec3 m1 = (p1 + p2) * 0.5f;
        glm::vec3 m2 = (p2 + p3) * 0.5f;
        glm::vec3 m3 = (p3 + p1) * 0.5f;

        // 재귀적으로 작은 삼각형 생성
        generateSierpinskiTriangle(vertices, indices, p1, m1, m3, depth - 1);
        generateSierpinskiTriangle(vertices, indices, m1, p2, m2, depth - 1);
        generateSierpinskiTriangle(vertices, indices, m3, m2, p3, depth - 1);
    }
}

template<size_t N, size_t M>
void read_obj_file(const char* filename, std::vector<Vertex>& vertexData, std::vector<GLuint>& indices, const std::array<std::array<glm::vec3, M>, N>& colors) {

    if (strcmp(filename, "pyramid.obj") == 0) {
        vertexData.clear();
        indices.clear();

        std::vector<glm::vec3> sierpinskiVertices;
        std::vector<int> sierpinskiIndices;

        // 피라미드의 각 면에 대해 시어핀스키 삼각형 생성
        glm::vec3 top(0.0f, 1.0f, 0.0f);
        glm::vec3 front(0.0f, 0.0f, 1.0f);
        glm::vec3 right(1.0f, 0.0f, -0.5f);
        glm::vec3 left(-1.0f, 0.0f, -0.5f);

        // 4개의 면에 대해 시어핀스키 삼각형 생성
        generateSierpinskiTriangle(sierpinskiVertices, sierpinskiIndices, top, front, right, sierpinskiDepth);
        generateSierpinskiTriangle(sierpinskiVertices, sierpinskiIndices, top, right, left, sierpinskiDepth);
        generateSierpinskiTriangle(sierpinskiVertices, sierpinskiIndices, top, left, front, sierpinskiDepth);
        generateSierpinskiTriangle(sierpinskiVertices, sierpinskiIndices, front, right, left, sierpinskiDepth);

        // Vertex 구조체로 변환
        for (size_t i = 0; i < sierpinskiVertices.size(); i++) {
            Vertex vertex;
            vertex.position = sierpinskiVertices[i];
            vertex.normal = glm::normalize(vertex.position);
            vertex.color = colors[0][0];  // 모든 삼각형에 같은 색상 적용
            vertex.faceIndex = i / 3;
            vertexData.push_back(vertex);
        }

        // 인덱스 복사
        indices.assign(sierpinskiIndices.begin(), sierpinskiIndices.end());
        return;
    }

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

void InitBuffer() {
    glGenVertexArrays(8, vao);
    glGenBuffers(16, vbo);


    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, cube1VertexData.size() * sizeof(Vertex), cube1VertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube1Indices.size() * sizeof(GLuint), cube1Indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, cube2VertexData.size() * sizeof(Vertex), cube2VertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube2Indices.size() * sizeof(GLuint), cube2Indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, cube3VertexData.size() * sizeof(Vertex), cube3VertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube3Indices.size() * sizeof(GLuint), cube3Indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao[3]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glBufferData(GL_ARRAY_BUFFER, cube4VertexData.size() * sizeof(Vertex), cube4VertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[7]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube4Indices.size() * sizeof(GLuint), cube4Indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao[4]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
    glBufferData(GL_ARRAY_BUFFER, cube5VertexData.size() * sizeof(Vertex), cube5VertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[9]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube5Indices.size() * sizeof(GLuint), cube5Indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao[5]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
    glBufferData(GL_ARRAY_BUFFER, cube6VertexData.size() * sizeof(Vertex), cube6VertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[11]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube6Indices.size() * sizeof(GLuint), cube6Indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao[6]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[12]);
    glBufferData(GL_ARRAY_BUFFER, cube7VertexData.size() * sizeof(Vertex), cube7VertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[13]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube7Indices.size() * sizeof(GLuint), cube7Indices.data(), GL_STATIC_DRAW);


    glBindVertexArray(vao[7]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[14]);

    glBufferData(GL_ARRAY_BUFFER, cube8VertexData.size() * sizeof(Vertex), cube8VertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[15]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube8Indices.size() * sizeof(GLuint), cube8Indices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &snowVAO);
    glGenBuffers(2, snowVBO);

    glBindVertexArray(snowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, snowVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, snowVertexData.size() * sizeof(Vertex), snowVertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, snowVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, snowIndices.size() * sizeof(GLuint), snowIndices.data(), GL_STATIC_DRAW);
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

// 전역 변수 추가
float planetRotation1 = 0.0f;
float planetRotation2 = 0.0f;
float planetRotation3 = 0.0f;

// 각 행성의 공전 반지름과 속도 설정
const float orbit1Radius = 2.0f;
const float orbit2Radius = 3.0f;
const float orbit3Radius = 4.0f;
const float orbit1Speed = 1.0f;
const float orbit2Speed = 0.7f;
const float orbit3Speed = 0.5f;

float lightDistance = 2.0f;

// timer 함수 수정
void timer(int value) {
    if (isLightRotating) {
        lightRotationAngle += 2.0f;
        lightPos.x = lightDistance * cos(glm::radians(lightRotationAngle));
        lightPos.z = lightDistance * sin(glm::radians(lightRotationAngle));
        lightPos.y = 2.0f;
    }

    // 행성들의 공전 각도 업데이트
    planetRotation1 += orbit1Speed;
    planetRotation2 += orbit2Speed;
    planetRotation3 += orbit3Speed;

    if (isSnowing) {
        for (auto& flake : snowflakes) {
            if (flake.active) {
                flake.position.y -= flake.speed;
                if (flake.position.y < 0.0f) {
                    flake.position.y = SNOW_HEIGHT;
                    flake.position.x = ((float)rand() / RAND_MAX) * SNOW_AREA - SNOW_AREA / 2;
                    flake.position.z = ((float)rand() / RAND_MAX) * SNOW_AREA - SNOW_AREA / 2;
                }
            }
        }
    }

    if (isRotating) {
        rotationAngle += 1.0f;  // 회전 속도 조절
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}



void keyboardCallback(unsigned char key, int x, int y) {

    static int lightPositionIndex = 0;
    static glm::vec3 lightPositions[] = {
        glm::vec3(1.0f, 2.0f, 0.0f),
        glm::vec3(0.0f, 2.0f, 1.0f),
        glm::vec3(0.0f, 2.0f, -1.0f),
        glm::vec3(-1.0f, 2.0f, 0.0f)
    };

    switch (key) {
    case 'm':
        ambientEnabled = !ambientEnabled;
        break;
    case 'c':
        currentLightColor = (currentLightColor + 1) % lightColors.size();
        break;
    case 'p':
        lightPositionIndex = (lightPositionIndex + 1) % 4;
        lightPos = lightPositions[lightPositionIndex];
        break;
    case 'n':
        lightDistance += 0.5f;
        lightPos *= glm::vec3(lightDistance / glm::length(lightPos));
        break;
    case 'f':
        lightDistance = std::max(0.5f, lightDistance - 0.5f);
        lightPos *= glm::vec3(lightDistance / glm::length(lightPos));
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
        sierpinskiDepth = key - '0';  // char to int 변환
        // 피라미드 데이터 다시 생성
        cube2VertexData.clear();
        cube2Indices.clear();
        read_obj_file("pyramid.obj", cube2VertexData, cube2Indices, face2Colors);
        // 버퍼 다시 초기화
        glBindVertexArray(vao[1]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
        glBufferData(GL_ARRAY_BUFFER, cube2VertexData.size() * sizeof(Vertex), cube2VertexData.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube2Indices.size() * sizeof(GLuint), cube2Indices.data(), GL_STATIC_DRAW);
        break;
    case 'r':
        isLightRotating = !isLightRotating;
        break;
    case '+': // 조명 세기 증가
        lightIntensity += 0.1f;
        break;
    case '-':
        lightIntensity -= 0.1f;
        break;
    case 's':
        isSnowing = !isSnowing;
        if (isSnowing) {
            initSnowflakes();
        }
        break;
    case 'y':
        isRotating = !isRotating;  // 회전 상태 토글
        break;
    }
    glutPostRedisplay();
}

void drawScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    glUniform1f(glGetUniformLocation(shaderProgramID, "lightIntensity"), lightIntensity);
    glUniform3fv(glGetUniformLocation(shaderProgramID, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgramID, "lightColor"), 1, glm::value_ptr(lightColors[currentLightColor]));
    glUniform1i(glGetUniformLocation(shaderProgramID, "ambientEnabled"), ambientEnabled);

    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 50.0f);
    view = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    if (isRotating) {
        view = glm::rotate(view, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // 빨간 큐브 그리기
    glBindVertexArray(vao[0]);
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5.0f, 0.0f, 5.0f));  // z축 방향으로 얇게 만듦
    model = glm::translate(model, glm::vec3(-0.5f, 0.0f, -0.5f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube1Indices.size(), GL_UNSIGNED_INT, 0);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube1Indices.size(), GL_UNSIGNED_INT, 0);

    // 노란 큐브 그리기
    glBindVertexArray(vao[1]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));  // y축 기준 90도 회전
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));  // z축 방향으로 얇게 만듦
    model = glm::translate(model, glm::vec3(0.0f, 0.2f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube2Indices.size(), GL_UNSIGNED_INT, 0);

    // 첫 번째 행성
    glBindVertexArray(vao[2]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.5f, 0.0f, 1.0f));  // x축 방향으로 이동한 후 회전
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube3Indices.size(), GL_UNSIGNED_INT, 0);

    // 두 번째 행성
    glBindVertexArray(vao[3]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.5f, 0.0f, 1.0f));  // x축 방향으로 이동한 후 회전
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube4Indices.size(), GL_UNSIGNED_INT, 0);

    // 세 번째 행성
    glBindVertexArray(vao[4]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));  // x축 방향으로 이동한 후 회전
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube5Indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(vao[5]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 1.0f));  // x축 방향으로 이동한 후 회전
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube6Indices.size(), GL_UNSIGNED_INT, 0);



    glBindVertexArray(vao[6]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.5f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube7Indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(vao[7]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));  // 크기를 줄임
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube8Indices.size(), GL_UNSIGNED_INT, 0);

    if (isSnowing) {
        glBindVertexArray(snowVAO);
        for (const auto& flake : snowflakes) {
            if (flake.active) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, flake.position);
                model = glm::scale(model, glm::vec3(0.1f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
        glBindVertexArray(0);
    }



    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 6);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Three Cubes with Axes");
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }


    read_obj_file("cube.obj", cube1VertexData, cube1Indices, face1Colors);
    read_obj_file("pyramid.obj", cube2VertexData, cube2Indices, face2Colors);
    read_obj_file("cube.obj", cube3VertexData, cube3Indices, face3Colors);

    read_obj_file("cube.obj", cube4VertexData, cube4Indices, face4Colors);
    read_obj_file("cube.obj", cube5VertexData, cube5Indices, face5Colors);
    read_obj_file("cube.obj", cube6VertexData, cube6Indices, face6Colors);
    read_obj_file("cube.obj", cube7VertexData, cube7Indices, face7Colors);
    read_obj_file("cube.obj", cube8VertexData, cube8Indices, face8Colors);




    make_shaderProgram();
    InitBuffer();
    initAxes();


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);  // 블렌딩 활성화
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // 블렌딩 함수 설정

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);

    glutKeyboardFunc(keyboardCallback);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();

    return 0;
}