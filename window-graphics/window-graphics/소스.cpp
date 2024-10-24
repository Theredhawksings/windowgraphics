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
GLuint vao[5];
GLuint vbo[10];
GLint result;
GLchar errorLog[512];

GLuint axisVAO, axisVBO;
const float axisLength = 5.0f;

std::vector<GLuint> indices;

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

bool change = true;

std::array<std::array<glm::vec3, 4>, 6> faceColors = { {
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f)}},
    {{glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.2f, 0.6f)}},
    {{glm::vec3(0.2f, 0.8f, 0.2f), glm::vec3(0.9f, 0.1f, 0.1f), glm::vec3(0.1f, 0.1f, 0.9f), glm::vec3(0.7f, 0.7f, 0.1f)}},
    {{glm::vec3(0.5f, 0.2f, 0.8f), glm::vec3(0.3f, 0.6f, 0.4f), glm::vec3(0.8f, 0.8f, 0.2f), glm::vec3(0.1f, 0.9f, 0.7f)}},
    {{glm::vec3(0.6f, 0.3f, 0.1f), glm::vec3(0.2f, 0.7f, 0.9f), glm::vec3(0.9f, 0.5f, 0.3f), glm::vec3(0.4f, 0.1f, 0.8f)}},
    {{glm::vec3(0.8f, 0.1f, 0.5f), glm::vec3(0.3f, 0.9f, 0.2f), glm::vec3(0.7f, 0.4f, 0.6f), glm::vec3(0.1f, 0.5f, 0.9f)}}
} };

std::array<std::array<glm::vec3, 3>, 17> coneFaceColors = { {

        {{glm::vec3(0.8f, 0.2f, 0.3f), glm::vec3(0.7f, 0.3f, 0.4f), glm::vec3(0.9f, 0.1f, 0.2f)}},  
        {{glm::vec3(0.2f, 0.8f, 0.4f), glm::vec3(0.3f, 0.7f, 0.5f), glm::vec3(0.1f, 0.9f, 0.3f)}},  
        {{glm::vec3(0.4f, 0.3f, 0.8f), glm::vec3(0.5f, 0.2f, 0.7f), glm::vec3(0.3f, 0.4f, 0.9f)}},  
        {{glm::vec3(0.9f, 0.8f, 0.2f), glm::vec3(0.8f, 0.9f, 0.3f), glm::vec3(0.7f, 0.7f, 0.1f)}},  
        {{glm::vec3(0.3f, 0.9f, 0.7f), glm::vec3(0.2f, 0.8f, 0.8f), glm::vec3(0.4f, 0.7f, 0.6f)}},  
        {{glm::vec3(0.7f, 0.4f, 0.6f), glm::vec3(0.6f, 0.5f, 0.5f), glm::vec3(0.8f, 0.3f, 0.7f)}},  
        {{glm::vec3(0.1f, 0.5f, 0.9f), glm::vec3(0.2f, 0.4f, 0.8f), glm::vec3(0.3f, 0.6f, 0.7f)}},  
        {{glm::vec3(0.6f, 0.8f, 0.2f), glm::vec3(0.5f, 0.9f, 0.3f), glm::vec3(0.7f, 0.7f, 0.1f)}},  
        {{glm::vec3(0.9f, 0.3f, 0.5f), glm::vec3(0.8f, 0.4f, 0.6f), glm::vec3(0.7f, 0.2f, 0.4f)}},  

        {{glm::vec3(0.4f, 0.7f, 0.3f), glm::vec3(0.3f, 0.8f, 0.2f), glm::vec3(0.5f, 0.6f, 0.4f)}},  
        {{glm::vec3(0.2f, 0.3f, 0.8f), glm::vec3(0.3f, 0.2f, 0.9f), glm::vec3(0.1f, 0.4f, 0.7f)}},  
        {{glm::vec3(0.8f, 0.6f, 0.1f), glm::vec3(0.9f, 0.5f, 0.2f), glm::vec3(0.7f, 0.7f, 0.3f)}},  
        {{glm::vec3(0.5f, 0.1f, 0.6f), glm::vec3(0.4f, 0.2f, 0.5f), glm::vec3(0.6f, 0.3f, 0.7f)}},  
        {{glm::vec3(0.3f, 0.7f, 0.8f), glm::vec3(0.2f, 0.8f, 0.7f), glm::vec3(0.4f, 0.6f, 0.9f)}},  
        {{glm::vec3(0.7f, 0.2f, 0.2f), glm::vec3(0.8f, 0.3f, 0.1f), glm::vec3(0.6f, 0.1f, 0.3f)}},  
        {{glm::vec3(0.1f, 0.8f, 0.5f), glm::vec3(0.2f, 0.7f, 0.6f), glm::vec3(0.3f, 0.9f, 0.4f)}},  
        {{glm::vec3(0.6f, 0.4f, 0.8f), glm::vec3(0.5f, 0.3f, 0.9f), glm::vec3(0.7f, 0.5f, 0.7f)}}   
    } };

std::array<std::array<glm::vec3, 3>, 4> prismFaceColors = { {
        {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.9f, 0.0f, 0.0f), glm::vec3(0.8f, 0.0f, 0.0f)}},
        {{glm::vec3(0.8f, 0.0f, 0.0f), glm::vec3(0.75f, 0.0f, 1.0f), glm::vec3(0.6f, 0.0f, 0.0f)}},
        {{glm::vec3(0.6f, 0.0f, 0.0f), glm::vec3(0.1f, 1.0f, 0.0f), glm::vec3(0.4f, 0.0f, 1.0f)}},
        {{glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(0.4f, 0.0f, 1.0f), glm::vec3(0.2f, 1.0f, 0.0f)}},

} };


std::array<std::array<glm::vec3, 4>, 6> face2Colors = { {
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f)}},
    {{glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.2f, 0.6f)}},
    {{glm::vec3(0.2f, 0.8f, 0.2f), glm::vec3(0.9f, 0.1f, 0.1f), glm::vec3(0.1f, 0.1f, 0.9f), glm::vec3(0.7f, 0.7f, 0.1f)}},
    {{glm::vec3(0.5f, 0.2f, 0.8f), glm::vec3(0.3f, 0.6f, 0.4f), glm::vec3(0.8f, 0.8f, 0.2f), glm::vec3(0.1f, 0.9f, 0.7f)}},
    {{glm::vec3(0.6f, 0.3f, 0.1f), glm::vec3(0.2f, 0.7f, 0.9f), glm::vec3(0.9f, 0.5f, 0.3f), glm::vec3(0.4f, 0.1f, 0.8f)}},
    {{glm::vec3(0.8f, 0.1f, 0.5f), glm::vec3(0.3f, 0.9f, 0.2f), glm::vec3(0.7f, 0.4f, 0.6f), glm::vec3(0.1f, 0.5f, 0.9f)}}
} };

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    int faceIndex;
};

std::vector<Vertex> cubeVertexData;
std::vector<GLuint> cubeIndices;
std::vector<Vertex> coneVertexData; 
std::vector<GLuint> coneIndices;    
std::vector<Vertex> prismVertexData;
std::vector<GLuint> prismIndices;
std::vector<Vertex> rect_prismVertexData;
std::vector<GLuint> rect_prismIndices;


glm::vec3 objectPosition(0.0f, 0.0f, 0.0f);
const float moveSpeed = 0.1f;

bool leftObjectRotate = true; 
bool rightObjectRotate = true; 
float rotationAngle = 0.0f;
bool rotationActive = false; 
int rotationDirection = 1;  


void initAxes() {
    std::vector<Vertex> cubeVertexData;
    std::vector<GLuint> cubeIndices;
    std::vector<Vertex> tetrahedronVertexData;
    std::vector<GLuint> tetrahedronIndices;
    std::vector<Vertex> prismVertexData;
    std::vector<GLuint> prismIndices;
    std::vector<Vertex> rect_prismVertexData;
    std::vector<GLuint> rect_prismIndices;

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
    glGenVertexArrays(4, vao);
    glGenBuffers(8, vbo);

    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, cubeVertexData.size() * sizeof(Vertex), cubeVertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(GLuint), cubeIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);

    glBufferData(GL_ARRAY_BUFFER, coneVertexData.size() * sizeof(Vertex), coneVertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, coneIndices.size() * sizeof(GLuint), coneIndices.data(), GL_STATIC_DRAW);


    glBindVertexArray(vao[2]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, prismVertexData.size() * sizeof(Vertex), prismVertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, prismIndices.size() * sizeof(GLuint), prismIndices.data(), GL_STATIC_DRAW);


    glBindVertexArray(vao[3]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glBufferData(GL_ARRAY_BUFFER, prismVertexData.size() * sizeof(Vertex), prismVertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[7]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, prismIndices.size() * sizeof(GLuint), prismIndices.data(), GL_STATIC_DRAW);


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
    case 'x': 
        if (leftObjectRotate || rightObjectRotate) {  
            xOrbitActive = true;
            xOrbitDirection = 1;
        }
        break;
    case 'X': 
        if (leftObjectRotate || rightObjectRotate) {
            xOrbitActive = true;
            xOrbitDirection = -1;
        }
        break;
    case 'y': 
        if (leftObjectRotate || rightObjectRotate) {
            yOrbitActive = true;
            yOrbitDirection = 1;
        }
        break;
    case 'Y': 
        if (leftObjectRotate || rightObjectRotate) {
            yOrbitActive = true;
            yOrbitDirection = -1;
        }
        break;

    case '1':  
        leftObjectRotate = true;
        rightObjectRotate = false;
        break;
    case '2':  
        leftObjectRotate = false;
        rightObjectRotate = true;
        break;
    case '3': 
        leftObjectRotate = true;
        rightObjectRotate = true;
        break;

    case 's':
        xOrbitActive = false;  
        yOrbitActive = false;   
        xOrbitAngle = 0.0f;    
        yOrbitAngle = 0.0f;    
        rotationActive = false; 
        rotationAngle = 0.0f;  
        leftObjectRotate = true; 
        rightObjectRotate = true; 
        break;

    case 'r':  
        rotationActive = true;
        rotationDirection = 1;
        break;

    case 'R':  
        rotationActive = true;
        rotationDirection = -1;
        break;

    case 'c':
         change = !change;
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
    if (rotationActive) { 
        rotationAngle += 1.0f * rotationDirection;
        if (rotationAngle > 360.0f) rotationAngle -= 360.0f;
        if (rotationAngle < 0.0f) rotationAngle += 360.0f;
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void drawScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    view = glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");
    GLuint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(lightPosLoc, 5.0f, 5.0f, 5.0f);

    if (change) {

        glBindVertexArray(vao[0]);
        model = glm::mat4(1.0f);
        if (rotationActive) { 
            model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
        if (leftObjectRotate) {
            model = glm::translate(model, objectPosition); 
            if (yOrbitActive) {  
                model = glm::rotate(model, glm::radians(yOrbitAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            if (xOrbitActive) {  
                model = glm::rotate(model, glm::radians(xOrbitAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            }
            model = glm::translate(model, -objectPosition);  
        }
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);


        glBindVertexArray(vao[1]);
        model = glm::mat4(1.0f);
        if (rotationActive) {  
            model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
        if (rightObjectRotate) {
            model = glm::translate(model, objectPosition);
            if (yOrbitActive) {  
                model = glm::rotate(model, glm::radians(yOrbitAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            if (xOrbitActive) { 
                model = glm::rotate(model, glm::radians(xOrbitAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            }
            model = glm::translate(model, -objectPosition); 
        }
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_INT, 0);
    }

    else {
        glBindVertexArray(vao[2]);
        model = glm::mat4(1.0f);
        if (rotationActive) {  
            model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
        if (rightObjectRotate) {
            model = glm::translate(model, objectPosition);
            if (yOrbitActive) {  
                model = glm::rotate(model, glm::radians(yOrbitAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            if (xOrbitActive) { 
                model = glm::rotate(model, glm::radians(xOrbitAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            }
            model = glm::translate(model, -objectPosition);  
        }
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_INT, 0);



        glBindVertexArray(vao[1]); 

        model = glm::mat4(1.0f);
        if (rotationActive) {
            model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
        if (rightObjectRotate) {
            model = glm::translate(model, objectPosition);
            if (yOrbitActive) {
                model = glm::rotate(model, glm::radians(yOrbitAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            if (xOrbitActive) {
                model = glm::rotate(model, glm::radians(xOrbitAngle), glm::vec3(1.0f, 0.0f, 0.0f));
            }
            model = glm::translate(model, -objectPosition);
        }
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, prismIndices.size(), GL_UNSIGNED_INT, 0); 
    }


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 6);

    if (!isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
    glutCreateWindow("OBJ Loader Example with Selectable Faces");
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    read_obj_file("cube.obj", cubeVertexData, cubeIndices, faceColors);
    read_obj_file("cone.obj", coneVertexData, coneIndices, coneFaceColors);
    read_obj_file("triangular_pillar.obj", prismVertexData, prismIndices, prismFaceColors);
    read_obj_file("rect_prism.obj", rect_prismVertexData, rect_prismIndices, face2Colors);



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