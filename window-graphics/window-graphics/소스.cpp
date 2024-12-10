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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

std::vector<Vertex> cubeVertexData;
std::vector<GLuint> cubeIndices;
std::vector<Vertex> pyramidVertexData;
std::vector<GLuint> pyramidIndices;

GLuint cubeTextures[6];
GLuint pyramidTextures[5];

GLint width, height;
GLuint vertexShader, fragmentShader, shaderProgramID;
GLuint vao[2];
GLuint vbo[4];
GLint result;
GLchar errorLog[512];

GLuint axisVAO, axisVBO;

GLuint backgroundVAO, backgroundVBO, backgroundEBO;
GLuint backgroundTexture;
GLuint backgroundShaderProgram;

float backgroundVertices[] = {
    // positions          // texture coords
    -1.0f,  1.0f, -0.99f,  0.0f, 1.0f,   // 왼쪽 위
    -1.0f, -1.0f, -0.99f,  0.0f, 0.0f,   // 왼쪽 아래
     1.0f, -1.0f, -0.99f,  1.0f, 0.0f,   // 오른쪽 아래
     1.0f,  1.0f, -0.99f,  1.0f, 1.0f    // 오른쪽 위
};

unsigned int backgroundIndices[] = {
    0, 1, 2,   // 첫 번째 삼각형
    0, 2, 3    // 두 번째 삼각형
};


const float axisLength = 5.0f;

bool showCube = false;
bool showPyramid = false;
bool isHiddenSurfaceRemoval = true;
bool isWireframe = false;

glm::mat4 model, view, projection;
float xOrbitAngle = 0.0f;
float yOrbitAngle = 0.0f;
bool xOrbitActive = false;
bool yOrbitActive = false;
int xOrbitDirection = 1;
int yOrbitDirection = 1;
const float orbitRadius = 2.0f;

std::array<std::array<glm::vec3, 4>, 6> faceColors = { {
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f)}},
    {{glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.2f, 0.6f)}},
    {{glm::vec3(0.2f, 0.8f, 0.2f), glm::vec3(0.9f, 0.1f, 0.1f), glm::vec3(0.1f, 0.1f, 0.9f), glm::vec3(0.7f, 0.7f, 0.1f)}},
    {{glm::vec3(0.5f, 0.2f, 0.8f), glm::vec3(0.3f, 0.6f, 0.4f), glm::vec3(0.8f, 0.8f, 0.2f), glm::vec3(0.1f, 0.9f, 0.7f)}},
    {{glm::vec3(0.6f, 0.3f, 0.1f), glm::vec3(0.2f, 0.7f, 0.9f), glm::vec3(0.9f, 0.5f, 0.3f), glm::vec3(0.4f, 0.1f, 0.8f)}},
    {{glm::vec3(0.8f, 0.1f, 0.5f), glm::vec3(0.3f, 0.9f, 0.2f), glm::vec3(0.7f, 0.4f, 0.6f), glm::vec3(0.1f, 0.5f, 0.9f)}}
} };

std::array<std::array<glm::vec3, 3>, 5> pyramidFaceColors = { {
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}},
    {{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f)}},
    {{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.2f, 0.6f), glm::vec3(0.2f, 0.8f, 0.4f)}},
    {{glm::vec3(0.7f, 0.3f, 0.1f), glm::vec3(0.1f, 0.7f, 0.3f), glm::vec3(0.3f, 0.1f, 0.7f)}},
    {{glm::vec3(0.9f, 0.9f, 0.1f), glm::vec3(0.1f, 0.9f, 0.9f), glm::vec3(0.9f, 0.1f, 0.9f)}}
} };

glm::vec3 objectPosition(0.0f, 0.0f, 0.0f);
const float moveSpeed = 0.1f;

GLuint loadTexture(const char* filePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
    }
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

void loadAllTextures() {
    cubeTextures[0] = loadTexture("cube_face1.bmp");
    cubeTextures[1] = loadTexture("cube_face2.bmp");
    cubeTextures[2] = loadTexture("cube_face3.bmp");
    cubeTextures[3] = loadTexture("cube_face4.bmp");
    cubeTextures[4] = loadTexture("cube_face5.bmp");
    cubeTextures[5] = loadTexture("cube_face6.bmp");
}

void initAxes() {
    float axisVertices[] = {
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,         1.0f, 0.0f, 0.0f,
        axisLength, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,         0.0f, 1.0f, 0.0f,
        0.0f, axisLength, 0.0f, 0.0f, 0.0f, 0.0f,      0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 0.0f,         0.0f, 0.0f, 1.0f,
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

template<size_t N, size_t M>
void read_obj_file(const char* filename, std::vector<Vertex>& vertexData, std::vector<GLuint>& indices,
    const std::array<std::array<glm::vec3, M>, N>& colors) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_texcoords;

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
        else if (type == "vt") {
            glm::vec2 texcoord;
            iss >> texcoord.x >> texcoord.y;
            temp_texcoords.push_back(texcoord);
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

                if (vertex_indices.size() > 1 && vertex_indices[1] != -1) {
                    vertex.texCoords = temp_texcoords[vertex_indices[1]];
                }
                else {
                    // 각 면에 따른 텍스처 좌표 설정
                    float u = (faceIndex % 2 == 0) ? 0.0f : 1.0f;
                    float v = (vertexIndex < 2) ? 0.0f : 1.0f;

                    switch (vertexIndex) {
                    case 0: vertex.texCoords = glm::vec2(0.0f, 0.0f); break;
                    case 1: vertex.texCoords = glm::vec2(1.0f, 0.0f); break;
                    case 2: vertex.texCoords = glm::vec2(1.0f, 1.0f); break;
                    case 3: vertex.texCoords = glm::vec2(0.0f, 1.0f); break;
                    default: vertex.texCoords = glm::vec2(0.0f, 0.0f);
                    }
                }


                if (vertex_indices.size() > 2 && vertex_indices[2] != -1) {
                    vertex.normal = temp_normals[vertex_indices[2]];
                }

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
    FILE* fptr = fopen(file, "rb");
    if (!fptr)
        return NULL;
    fseek(fptr, 0, SEEK_END);
    long length = ftell(fptr);
    char* buf = (char*)malloc(length + 1);
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

void make_backgroundShaderProgram() {
    GLuint backgroundVertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint backgroundFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // 배경용 셰이더 소스 로드
    GLchar* backgroundVertexSource = filetobuf("background_vertex.glsl");
    GLchar* backgroundFragmentSource = filetobuf("background_fragment.glsl");

    glShaderSource(backgroundVertexShader, 1, (const GLchar**)&backgroundVertexSource, 0);
    glShaderSource(backgroundFragmentShader, 1, (const GLchar**)&backgroundFragmentSource, 0);

    // 컴파일
    glCompileShader(backgroundVertexShader);
    glCompileShader(backgroundFragmentShader);

    // 셰이더 프로그램 생성
    backgroundShaderProgram = glCreateProgram();
    glAttachShader(backgroundShaderProgram, backgroundVertexShader);
    glAttachShader(backgroundShaderProgram, backgroundFragmentShader);
    glLinkProgram(backgroundShaderProgram);

    // 셰이더 삭제
    glDeleteShader(backgroundVertexShader);
    glDeleteShader(backgroundFragmentShader);
}

void InitBuffer() {
    glGenVertexArrays(2, vao);
    glGenBuffers(4, vbo);

    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, cubeVertexData.size() * sizeof(Vertex), cubeVertexData.data(), GL_STATIC_DRAW);

    // 버텍스 속성 설정 수정
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(GLuint), cubeIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, pyramidVertexData.size() * sizeof(Vertex), pyramidVertexData.data(), GL_STATIC_DRAW);

    // 버텍스 속성 설정 수정
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pyramidIndices.size() * sizeof(GLuint), pyramidIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void initBackground() {
    // VAO, VBO, EBO 생성
    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    glGenBuffers(1, &backgroundEBO);

    glBindVertexArray(backgroundVAO);

    // VBO에 데이터 바인딩
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);

    // EBO에 데이터 바인딩
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, backgroundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backgroundIndices), backgroundIndices, GL_STATIC_DRAW);

    // 위치 속성
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 텍스처 좌표 속성
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 배경 텍스처 로드
    backgroundTexture = loadTexture("background.bmp");  // 배경 이미지 파일명
}

void keyboardCallback(unsigned char key, int x, int y) {
    switch (key) {
    case 'c':  // 육면체 표시/숨기기
        showPyramid = false;
        showCube = true;
        break;
    case 'p':  // 사각뿔 표시/숨기기
        showPyramid = true;
        showCube = false;
        break;
    case 'x':  // x축 양의 방향 회전
        xOrbitActive = true;
        xOrbitDirection = 1;
        break;
    case 'X':  // x축 음의 방향 회전
        xOrbitActive = true;
        xOrbitDirection = -1;
        break;
    case 'y':  // y축 양의 방향 회전
        yOrbitActive = true;
        yOrbitDirection = 1;
        break;
    case 'Y':  // y축 음의 방향 회전
        yOrbitActive = true;
        yOrbitDirection = -1;
        break;
    case 's':  // 초기화
        objectPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        xOrbitActive = false;
        yOrbitActive = false;
        xOrbitAngle = 0.0f;
        yOrbitAngle = 0.0f;
        break;
    }
    glutPostRedisplay();
}

void timer(int value) {
    if (xOrbitActive) {
        xOrbitAngle += 1.0f * xOrbitDirection;
        if (xOrbitAngle > 360.0f) xOrbitAngle -= 360.0f;
        if (xOrbitAngle < 0.0f) xOrbitAngle += 360.0f;
    }
    if (yOrbitActive) {
        yOrbitAngle += 1.0f * yOrbitDirection;
        if (yOrbitAngle > 360.0f) yOrbitAngle -= 360.0f;
        if (yOrbitAngle < 0.0f) yOrbitAngle += 360.0f;
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void drawScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glUseProgram(backgroundShaderProgram);
    glBindVertexArray(backgroundVAO);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glUseProgram(shaderProgramID);

    view = glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");
    GLuint textureLoc = glGetUniformLocation(shaderProgramID, "texture1");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(textureLoc, 0);  // 텍스처 유닛 0 사용

    if (showCube) {
        glBindVertexArray(vao[0]);
        model = glm::mat4(1.0f);
        model = glm::translate(model, objectPosition);
        model = glm::rotate(model, glm::radians(yOrbitAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(xOrbitAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        for (int i = 0; i < 6; ++i) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cubeTextures[i]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(GLuint)));
        }

    }

    if (showPyramid) {
        glBindVertexArray(vao[1]);
        model = glm::mat4(1.0f);
        model = glm::translate(model, objectPosition);
        model = glm::rotate(model, glm::radians(yOrbitAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(xOrbitAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // 피라미드는 첫 번째 텍스처 사용
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTextures[0]);
        glDrawElements(GL_TRIANGLES, pyramidIndices.size(), GL_UNSIGNED_INT, 0);
    }

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
    glutCreateWindow("Textured Objects Viewer");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    loadAllTextures();
    make_backgroundShaderProgram(); 
    initBackground();  

    read_obj_file("cube.obj", cubeVertexData, cubeIndices, faceColors);
    read_obj_file("pyramid.obj", pyramidVertexData, pyramidIndices, pyramidFaceColors);

    make_shaderProgram();
    InitBuffer();
    initAxes();

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyboardCallback);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}