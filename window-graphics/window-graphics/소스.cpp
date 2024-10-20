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

GLint width, height;
GLuint vertexShader, fragmentShader, shaderProgramID;
GLuint vao[2];
GLuint vbo[4];
GLint result;
GLchar errorLog[512];

GLuint axisVAO, axisVBO;
const float axisLength = 5.0f;

std::vector<GLuint> indices;
std::array<bool, 6> cubeFaceVisible = { false, false, false, false, false, false };
std::array<bool, 4> tetrahedronFaceVisible = { false, false, false, false };

glm::mat4 model, view, projection;


// faceColors 배열 수정 (각 면의 모서리에 대한 색상)
std::array<std::array<glm::vec3, 4>, 6> faceColors = { {
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f)}},
    {{glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.2f, 0.6f)}},
    {{glm::vec3(0.2f, 0.8f, 0.2f), glm::vec3(0.9f, 0.1f, 0.1f), glm::vec3(0.1f, 0.1f, 0.9f), glm::vec3(0.7f, 0.7f, 0.1f)}},
    {{glm::vec3(0.5f, 0.2f, 0.8f), glm::vec3(0.3f, 0.6f, 0.4f), glm::vec3(0.8f, 0.8f, 0.2f), glm::vec3(0.1f, 0.9f, 0.7f)}},
    {{glm::vec3(0.6f, 0.3f, 0.1f), glm::vec3(0.2f, 0.7f, 0.9f), glm::vec3(0.9f, 0.5f, 0.3f), glm::vec3(0.4f, 0.1f, 0.8f)}},
    {{glm::vec3(0.8f, 0.1f, 0.5f), glm::vec3(0.3f, 0.9f, 0.2f), glm::vec3(0.7f, 0.4f, 0.6f), glm::vec3(0.1f, 0.5f, 0.9f)}}
} };

std::array<std::array<glm::vec3, 3>, 4> tetrahedronFaceColors = { {
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}},
    {{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f)}},
    {{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.2f, 0.6f), glm::vec3(0.2f, 0.8f, 0.4f)}},
    {{glm::vec3(0.7f, 0.3f, 0.1f), glm::vec3(0.1f, 0.7f, 0.3f), glm::vec3(0.3f, 0.1f, 0.7f)}}
} };


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    int faceIndex;  // 이 정점이 속한 면의 인덱스
};

std::vector<Vertex> cubeVertexData;
std::vector<GLuint> cubeIndices;
std::vector<Vertex> tetrahedronVertexData;
std::vector<GLuint> tetrahedronIndices;


void initAxes() {std::vector<Vertex> cubeVertexData;
std::vector<GLuint> cubeIndices;
std::vector<Vertex> tetrahedronVertexData;
std::vector<GLuint> tetrahedronIndices;

    float axisVertices[] = {
        // 위치              // 노멀 (사용하지 않음)   // 색상
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,         1.0f, 0.0f, 0.0f,  // X축 시작
        axisLength, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,  // X축 끝
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,         0.0f, 1.0f, 0.0f,  // Y축 시작
        0.0f, axisLength, 0.0f, 0.0f, 0.0f, 0.0f,      0.0f, 1.0f, 0.0f,  // Y축 끝
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,         0.0f, 0.0f, 1.0f,  // Z축 시작
        0.0f, 0.0f, axisLength, 0.0f, 0.0f, 0.0f,      0.0f, 0.0f, 1.0f   // Z축 끝
    };

    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);

    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    // 위치 속성
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 노멀 속성 (사용하지 않지만 셰이더와 일치시키기 위해 설정)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 색상 속성
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    glGenVertexArrays(2, vao);  // vao 배열 전체를 전달
    glGenBuffers(4, vbo);

    // 큐브 데이터 설정
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, cubeVertexData.size() * sizeof(Vertex), cubeVertexData.data(), GL_STATIC_DRAW);

    // 정점 속성 설정 (큐브)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    // 큐브 인덱스 버퍼
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(GLuint), cubeIndices.data(), GL_STATIC_DRAW);

    // 정사면체 데이터 설정
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, tetrahedronVertexData.size() * sizeof(Vertex), tetrahedronVertexData.data(), GL_STATIC_DRAW);

    // 정점 속성 설정 (정사면체)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    // 정사면체 인덱스 버퍼
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, tetrahedronIndices.size() * sizeof(GLuint), tetrahedronIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

template<size_t N>
void selectRandomFaces(std::array<bool, N>& faceArray) {
    std::fill(faceArray.begin(), faceArray.end(), false);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, N - 1);

    int count = 0;
    while (count < 2) {
        int face = dis(gen);
        if (!faceArray[face]) {
            faceArray[face] = true;
            count++;
        }
    }
}

void keyboardCallback(unsigned char key, int x, int y) {
    switch (key) {
    case 'c':
    case 'C':
        selectRandomFaces(cubeFaceVisible);
        break;
    case 't':
    case 'T':
        selectRandomFaces(tetrahedronFaceVisible);
        break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
        cubeFaceVisible[key - '1'] = !cubeFaceVisible[key - '1'];
        break;
    case '7':
    case '8':
    case '9':
    case '0':
    {
        int index = (key == '0') ? 3 : (key - '7');
        if (index >= 0 && index < 4) {
            tetrahedronFaceVisible[index] = !tetrahedronFaceVisible[index];
        }
    }
    break;
    }
    glutPostRedisplay();
}

GLvoid drawScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    // 카메라 위치 조정
    view = glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");
    GLuint isAxisLoc = glGetUniformLocation(shaderProgramID, "isAxis");
    GLuint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(lightPosLoc, 5.0f, 5.0f, 5.0f);  // 광원 위치 전달

    glBindVertexArray(vao[0]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    for (int i = 0; i < 6; i++) {
        if (cubeFaceVisible[i]) {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(GLuint)));
        }
    }

    // 사면체 그리기
    glBindVertexArray(vao[1]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    for (int i = 0; i < 4; i++) {
        if (tetrahedronFaceVisible[i]) {
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(GLuint)));
        }
    }

    // 축 그리기
    model = glm::mat4(1.0f); // 축에는 회전 적용 안 함
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(isAxisLoc, 1);  // 축임을 표시
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 6);

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
    glutCreateWindow("OBJ Loader Example with Selectable Faces");
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    read_obj_file("cube.obj", cubeVertexData, cubeIndices, faceColors);
    read_obj_file("tetrahedron.obj", tetrahedronVertexData, tetrahedronIndices, tetrahedronFaceColors);

    make_shaderProgram();
    InitBuffer();
    initAxes();

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyboardCallback);  // 키보드 콜백 함수 등록

    glutMainLoop();


    return 0;
}
