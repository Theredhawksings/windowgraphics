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
GLuint vao[4];
GLuint vbo[8];
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


enum AnimationState {
    NONE,
    SPIRAL,
    THROUGH_ORIGIN,
    ORBIT_EXCHANGE,
    UP_DOWN_EXCHANGE,
    SCALE_ROTATE_ORBIT
};

AnimationState animationState = NONE;
float animationProgress = 0.0f;
const float animationSpeed = 0.02f;


std::array<std::array<glm::vec3, 3>, 12> faceColors = { {

        {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
        {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f)},

        {glm::vec3(0.2f, 0.8f, 0.2f), glm::vec3(0.9f, 0.1f, 0.1f), glm::vec3(0.1f, 0.1f, 0.9f)},
        {glm::vec3(0.3f, 0.6f, 0.4f), glm::vec3(0.8f, 0.8f, 0.2f), glm::vec3(0.1f, 0.9f, 0.7f)},

        {glm::vec3(0.2f, 0.8f, 0.2f), glm::vec3(0.9f, 0.1f, 0.1f), glm::vec3(0.1f, 0.1f, 0.9f)},
        {glm::vec3(0.3f, 0.6f, 0.4f), glm::vec3(0.8f, 0.8f, 0.2f), glm::vec3(0.1f, 0.9f, 0.7f)},

        {glm::vec3(0.5f, 0.2f, 0.8f), glm::vec3(0.3f, 0.6f, 0.4f), glm::vec3(0.8f, 0.8f, 0.2f)},
        {glm::vec3(0.7f, 0.4f, 0.5f), glm::vec3(0.3f, 0.9f, 0.2f), glm::vec3(0.7f, 0.4f, 0.6f)},

        {glm::vec3(0.6f, 0.3f, 0.1f), glm::vec3(0.2f, 0.7f, 0.9f), glm::vec3(0.9f, 0.5f, 0.3f)},
        {glm::vec3(0.8f, 0.1f, 0.5f), glm::vec3(0.3f, 0.9f, 0.2f), glm::vec3(0.7f, 0.4f, 0.6f)},

        {glm::vec3(0.8f, 0.1f, 0.5f), glm::vec3(0.3f, 0.9f, 0.2f), glm::vec3(0.7f, 0.4f, 0.6f)},
        {glm::vec3(0.6f, 0.3f, 0.1f), glm::vec3(0.2f, 0.7f, 0.9f), glm::vec3(0.9f, 0.5f, 0.3f)}
    } };

std::array<std::array<glm::vec3, 3>, 16> coneFaceColors = { {

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

glm::vec3 objectPosition(0.0f, 0.0f, 0.0f);
const float moveSpeed = 0.1f;


glm::vec3 leftObjectPosition(-3.0f, 0.0f, 0.0f);
glm::vec3 rightObjectPosition(3.0f, 0.0f, 0.0f);


float leftObjectScale = 1.0f;     
float rightObjectScale = 1.0f;    
float scaleSpeed = 0.1f;
float minScale = 0.1f;
bool isScaleFromOrigin = false;


GLuint spiralVAO, spiralVBO;
std::vector<glm::vec3> spiralPoints;  
bool showSpiral = false;

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

    glBindVertexArray(0);
}


bool isSpiralAnimation = false;
float spiralAngle = 0.0f;
float spiralRadius = 0.0f;
const float spiralSpeed = 1.0f;       
const float spiralGrowth = 0.05f;     
const float maxSpiralRadius = 6.0f; 

void generateSpiralPoints() {
    spiralPoints.clear();
    const float maxAngle = 1440.0f;  
    const float angleStep = 1.0f;     
    const float radiusGrowth = 0.005f; 

    for (float angle = 0.0f; angle <= maxAngle; angle += angleStep) {
        float radius = radiusGrowth * angle;
        float x = radius * cos(glm::radians(angle));
        float z = radius * sin(glm::radians(angle));
        spiralPoints.push_back(glm::vec3(x, 0.0f, z));
    }

    glGenVertexArrays(1, &spiralVAO);
    glGenBuffers(1, &spiralVBO);

    glBindVertexArray(spiralVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spiralVBO);
    glBufferData(GL_ARRAY_BUFFER, spiralPoints.size() * sizeof(glm::vec3), spiralPoints.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int currentPoint = 0;

void renderSpiral() {
    glBindVertexArray(spiralVAO);
    glLineWidth(2.0f);

    glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
    glUniform3f(glGetUniformLocation(shaderProgramID, "Normal"), normal.x, normal.y, normal.z);

    glDrawArrays(GL_LINE_STRIP, 0, spiralPoints.size());
    glBindVertexArray(0);
}



void timer(int value) {

    if (animationState != NONE) {
        animationProgress += animationSpeed;

        if (animationState == SPIRAL) {
            showSpiral = true;
            currentPoint = (currentPoint + 2) % spiralPoints.size();
            glm::vec3 spiralPos = spiralPoints[currentPoint];
            leftObjectPosition = spiralPos;
            rightObjectPosition = -spiralPos;
        }

        else if (animationState == THROUGH_ORIGIN) {
            float t = glm::min(animationProgress, 1.0f);
            leftObjectPosition = glm::mix(glm::vec3(-3.0f, 0.0f, 0.0f),
                glm::vec3(3.0f, 0.0f, 0.0f), t);
            rightObjectPosition = glm::mix(glm::vec3(3.0f, 0.0f, 0.0f),
                glm::vec3(-3.0f, 0.0f, 0.0f), t);

            if (t >= 1.0f) {
                animationState = NONE;
                animationProgress = 0.0f;
            }
        }

        else if (animationState == ORBIT_EXCHANGE) {
            float t = glm::min(animationProgress, 1.0f);
            float angle = t * glm::pi<float>();  

            float leftX = -orbitRadius * cos(angle);    
            float leftZ = orbitRadius * sin(angle);     

            float rightX = orbitRadius * cos(angle);    
            float rightZ = -orbitRadius * sin(angle);   

            leftObjectPosition = glm::vec3(leftX, 0.0f, leftZ);
            rightObjectPosition = glm::vec3(rightX, 0.0f, rightZ);

            if (t >= 1.2f) {
                animationState = NONE;
                animationProgress = 0.0f;
                leftObjectPosition = glm::vec3(3.0f, 0.0f, 0.0f);
                rightObjectPosition = glm::vec3(-3.0f, 0.0f, 0.0f);
            }
        }

        else if (animationState == UP_DOWN_EXCHANGE) {

            float t = glm::min(animationProgress, 1.0f);
            const float maxHeight = 3.0f;  

            float curve = sin(t * glm::pi<float>());

            float leftX = glm::mix(-3.0f, 3.0f, t); 
            float leftY = maxHeight * curve; 

            float rightX = glm::mix(3.0f, -3.0f, t); 
            float rightY = -maxHeight * curve; 

            leftObjectPosition = glm::vec3(leftX, leftY, 0.0f);
            rightObjectPosition = glm::vec3(rightX, rightY, 0.0f);

            if (t >= 1.0f) {
                animationState = NONE;
                animationProgress = 0.0f;
                leftObjectPosition = glm::vec3(3.0f, 0.0f, 0.0f);
                rightObjectPosition = glm::vec3(-3.0f, 0.0f, 0.0f);
            }
        }

        else if (animationState == SCALE_ROTATE_ORBIT) {
             

            animationProgress -= 0.027f;

            if (animationProgress < 0.001f) {
                animationProgress = 0.001f;
            }

            float t = glm::min(animationProgress, 1.0f);

            leftObjectScale = 1.0f + t;
            rightObjectScale = 1.0f - 0.5f * t;

       
            float rotationAngle = t * 360.0f;


            float orbitAngle = t * glm::pi<float>() * 4.0f;  


            float radius = 3.0f;
            leftObjectPosition = glm::vec3(
                radius * cos(orbitAngle),
                0.0f,
                radius * sin(orbitAngle)
            );

            rightObjectPosition = glm::vec3(
                -radius * cos(orbitAngle),
                0.0f,
                -radius * sin(orbitAngle)
            );

            if (t >= 1.0f) {
                animationState = NONE;
                animationProgress = 0.0f;
                leftObjectPosition = glm::vec3(3.0f, 0.0f, 0.0f);
                rightObjectPosition = glm::vec3(-3.0f, 0.0f, 0.0f);
            }

            animationProgress += animationSpeed;
        }

            
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboardCallback(unsigned char key, int x, int y) {
    switch (key) {

    case '1':
        animationState = SPIRAL;
        currentPoint = 0;
        break;

    case '2':
        animationState = THROUGH_ORIGIN;
        animationProgress = 0.0f;
        break;

    case '3':  
        animationState = ORBIT_EXCHANGE;
        animationProgress = 0.0f;
        break;

    case '4': 
        if (animationState == NONE) {
            animationState = UP_DOWN_EXCHANGE;
            animationProgress = 0.0f;
        }
        break;

    case '5':
        if (animationState == NONE) {
            animationState = SCALE_ROTATE_ORBIT;
            animationProgress = 0.0f;
        }
        break;

    case '6':  // 애니메이션 중지
        animationState = NONE;
        animationProgress = 0.0f;
        currentPoint = 0;

        leftObjectScale = 1.0f;
        rightObjectScale = 1.0f;

        leftObjectPosition = glm::vec3(-3.0f, 0.0f, 0.0f);
        rightObjectPosition = glm::vec3(3.0f, 0.0f, 0.0f);

        showSpiral = false;
        break;

    case 'q': leftObjectPosition.x -= moveSpeed; break;
    case 'w': leftObjectPosition.x += moveSpeed; break;
    case 'e': leftObjectPosition.y -= moveSpeed; break;
    case 'r': leftObjectPosition.y += moveSpeed; break;
    case 't': leftObjectPosition.z -= moveSpeed; break;
    case 'y': leftObjectPosition.z += moveSpeed; break;

        // 오른쪽 도형 이동
    case 'Q': rightObjectPosition.x -= moveSpeed; break;
    case 'W': rightObjectPosition.x += moveSpeed; break;
    case 'E': rightObjectPosition.y -= moveSpeed; break;
    case 'R': rightObjectPosition.y += moveSpeed; break;
    case 'T': rightObjectPosition.z -= moveSpeed; break;
    case 'Y': rightObjectPosition.z += moveSpeed; break;


    case 'v':
        isScaleFromOrigin = !isScaleFromOrigin; // 모드 토글
        break;

    case 'z': // 확대
        leftObjectScale += scaleSpeed;
        break;
    case 'x': // 축소
        if (leftObjectScale - scaleSpeed >= minScale) {
            leftObjectScale -= scaleSpeed;
        }
        break;

    case 'Z': // 확대
        rightObjectScale += scaleSpeed;
        break;
    case 'X': // 축소
        if (rightObjectScale - scaleSpeed >= minScale) {
            rightObjectScale -= scaleSpeed;
        }
        break;


    case 'a': leftObjectPosition.x -= moveSpeed; leftObjectPosition.z -= moveSpeed, rightObjectPosition.x -= moveSpeed; rightObjectPosition.z -= moveSpeed;  break;
    case 's': leftObjectPosition.x += moveSpeed; leftObjectPosition.z += moveSpeed, rightObjectPosition.x += moveSpeed; rightObjectPosition.z += moveSpeed;  break;

    case 'c':
        leftObjectPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        rightObjectPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        leftObjectScale = 1.0f;
        rightObjectScale = 1.0f;
        break;
    }

    glutPostRedisplay();
}

void drawScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    view = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");
    GLuint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(lightPosLoc, 5.0f, 5.0f, 5.0f);

    if (showSpiral) {
        renderSpiral();
    }

    glBindVertexArray(vao[0]);
    model = glm::mat4(1.0f);
    if (isScaleFromOrigin) {
        model = glm::scale(model, glm::vec3(leftObjectScale));
        model = glm::translate(model, leftObjectPosition);
    }
    else {
        model = glm::translate(model, leftObjectPosition);
        model = glm::scale(model, glm::vec3(leftObjectScale));
    }
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(vao[1]);
    model = glm::mat4(1.0f);
    if (isScaleFromOrigin) {
        model = glm::scale(model, glm::vec3(rightObjectScale));
        model = glm::translate(model, rightObjectPosition);
    }
    else {
        model = glm::translate(model, rightObjectPosition);
        model = glm::scale(model, glm::vec3(rightObjectScale));
    }
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, coneIndices.size(), GL_UNSIGNED_INT, 0);

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
    glutCreateWindow("OBJ Loader Example with Selectable Faces");
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    read_obj_file("cube.obj", cubeVertexData, cubeIndices, faceColors);
    read_obj_file("cone.obj", coneVertexData, coneIndices, coneFaceColors);

    make_shaderProgram();
    InitBuffer();
    initAxes();
    generateSpiralPoints(); 
   

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyboardCallback); 
    glutTimerFunc(0, timer, 0);

    glutMainLoop();


    return 0;
}