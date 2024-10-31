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

bool isHiddenSurfaceRemoval = true;

glm::mat4 model, view, projection;

bool change = true;

std::array<std::array<glm::vec3, 4>, 6> faceColors = { {
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f)}},
    {{glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.8f, 0.2f, 0.6f)}},
    {{glm::vec3(0.2f, 0.8f, 0.2f), glm::vec3(0.9f, 0.1f, 0.1f), glm::vec3(0.1f, 0.1f, 0.9f), glm::vec3(0.7f, 0.7f, 0.1f)}},
    {{glm::vec3(0.5f, 0.2f, 0.8f), glm::vec3(0.3f, 0.6f, 0.4f), glm::vec3(0.8f, 0.8f, 0.2f), glm::vec3(0.1f, 0.9f, 0.7f)}},
    {{glm::vec3(0.6f, 0.3f, 0.1f), glm::vec3(0.2f, 0.7f, 0.9f), glm::vec3(0.9f, 0.5f, 0.3f), glm::vec3(0.4f, 0.1f, 0.8f)}},
    {{glm::vec3(0.8f, 0.1f, 0.5f), glm::vec3(0.3f, 0.9f, 0.2f), glm::vec3(0.7f, 0.4f, 0.6f), glm::vec3(0.1f, 0.5f, 0.9f)}}
} };

std::array<std::array<glm::vec3, 3>, 6> prismFaceColors = { {
        {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.9f, 0.0f, 0.0f), glm::vec3(0.8f, 0.0f, 0.0f)}},
        {{glm::vec3(0.9f, 0.0f, 0.0f), glm::vec3(0.85f, 0.0f, 0.0f), glm::vec3(0.8f, 0.0f, 0.0f)}},
        {{glm::vec3(0.6f, 0.0f, 0.0f), glm::vec3(0.1f, 1.0f, 0.0f), glm::vec3(0.4f, 0.0f, 1.0f)}},
        {{glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(0.4f, 0.0f, 1.0f), glm::vec3(0.2f, 1.0f, 0.0f)}},
        {{glm::vec3(0.3f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 1.0f), glm::vec3(0.3f, 1.0f, 0.0f)}},
        {{glm::vec3(0.4f, 0.0f, 0.0f), glm::vec3(0.6f, 0.0f, 1.0f), glm::vec3(0.4f, 1.0f, 0.0f)}}
    } };




struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    int faceIndex;
};

std::vector<Vertex> cubeVertexData;
std::vector<GLuint> cubeIndices;
std::vector<Vertex> prismVertexData;
std::vector<GLuint> prismIndices;


void initAxes() {
    std::vector<Vertex> cubeVertexData;
    std::vector<GLuint> cubeIndices;
    std::vector<Vertex> prismVertexData;
    std::vector<GLuint> prismIndices;

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
    glGenVertexArrays(2, vao);
    glGenBuffers(4, vbo);

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

    glBufferData(GL_ARRAY_BUFFER, prismVertexData.size() * sizeof(Vertex), prismVertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, prismIndices.size() * sizeof(GLuint), prismIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

bool isRotating = false;
float currentRotationAngle = 0.0f;

bool isTopRotating = false;
float topRotationAngle = 0.0f;

bool isAutoFront = false;    
float frontRotationAngle = 0.0f;
bool frontroate = true;

bool isAutoside = false;
float sideRotationAngle = 0.0f;
bool sideroate = true;

bool isAutoscale = false;
float behindscale = 1.0f; 
bool scaleroate = true;

bool isAutoOpening = false;
float openAngle1 = 0.0f;
float openAngle2 = 0.0f;
float openAngle3 = 0.0f;
float openAngle4 = 0.0f;
bool isOpening = true;
bool iseachOpening = false;
int currentFace = 0;

bool isOrtho = true;

void keyboardCallback(unsigned char key, int x, int y) {
    switch (key) {

    case 'h':
        isHiddenSurfaceRemoval = !isHiddenSurfaceRemoval;
        if (isHiddenSurfaceRemoval) {
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }
        break;

    case 'y':
        isRotating = !isRotating;
        break;

    case 't':
        isTopRotating = !isTopRotating;
        break;

    case 'f':
        isAutoFront = !isAutoFront; 
        break;

    case 's':
        isAutoside = !isAutoside;
        break;
    
    case 'b':
        isAutoscale = !isAutoscale;
        break;

    case 'o' :
        isAutoOpening = !isAutoOpening;
        break;

    case 'r':
        iseachOpening = !iseachOpening;
        break;

    case 'p': 
        isOrtho = !isOrtho;
        if (isOrtho) {
            projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 50.0f);
        }
        else {
            projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 50.0f);
        }
        break;
            
    case 'c':
        change = !change;
        break;

    case 'l':
        isRotating = false;
        currentRotationAngle = 0.0f;

        isTopRotating = false;
        topRotationAngle = 0.0f;

        isAutoFront = false;
        frontRotationAngle = 0.0f;
        frontroate = true;

        isAutoside = false;
        sideRotationAngle = 0.0f;
        sideroate = true;

        isAutoscale = false;
        behindscale = 1.0f;
        scaleroate = true;

        isAutoOpening = false;
        openAngle1 = 0.0f;
        openAngle2 = 0.0f;
        openAngle3 = 0.0f;
        openAngle4 = 0.0f;
        isOpening = true;
        iseachOpening = false;
        break;
    }

    glutPostRedisplay();
}



void timer(int value) {

    if (isRotating) {
        currentRotationAngle += 1.0f;
        if (currentRotationAngle >= 360.0f)
            currentRotationAngle -= 360.0f;
    }

    if (isTopRotating) {
        topRotationAngle += 1.0f;
        if (topRotationAngle >= 360.0f)
            topRotationAngle -= 360.0f;
    }

    if (isAutoFront) {
        if (frontroate) {
            frontRotationAngle += 1.0f;
        }

        else if(!frontroate){
            frontRotationAngle -= 1.0f;
        }

        if (frontRotationAngle >= 180.0f || frontRotationAngle<=0.0f)
            frontroate = !frontroate;
    }

    if (isAutoside) {
        if (sideroate) {
            sideRotationAngle += 0.1f;
        }

        else if (!sideroate) {
            sideRotationAngle -= 0.1f;
        }

        if (sideRotationAngle >= 2.0f || sideRotationAngle <= 0.0f)
            sideroate = !sideroate;
    }

    if (isAutoscale) {

        if (scaleroate) {
            behindscale -= 0.005f;
        }

        else if (!scaleroate) {
            behindscale += 0.005f;
        }

        if (behindscale <= -1.0f)
            scaleroate = false;

        if (behindscale >= 1.0f)
            scaleroate = true;
    }

    if (isAutoOpening) {
        if (isOpening) {
            openAngle1 += 1.0f;
            openAngle2 += 1.0f;
            openAngle3 += 1.0f;
            openAngle4 += 1.0f;
        }
        else {
            openAngle1 -= 1.0f;
            openAngle2 -= 1.0f;
            openAngle3 -= 1.0f;
            openAngle4 -= 1.0f;
        }

        if (openAngle1 >= 235.0f || openAngle1 <= 0.0f) {
            isOpening = !isOpening;
        }
    }

    if (iseachOpening) {

        const float ANIMATION_SPEED = 1.0f;
        const float MAX_ANGLE = 90.0f;

        if (isOpening) {
            if (currentFace == 0) {
                if (openAngle1 < MAX_ANGLE) {
                    openAngle1 = std::min(openAngle1 + ANIMATION_SPEED, MAX_ANGLE);
                }
                else {
                    currentFace = 1;
                }
            }
            else if (currentFace == 1) {
                if (openAngle2 < MAX_ANGLE) {
                    openAngle2 = std::min(openAngle2 + ANIMATION_SPEED, MAX_ANGLE);
                }
                else {
                    currentFace = 2;
                }
            }
            else if (currentFace == 2) {
                if (openAngle3 < MAX_ANGLE) {
                    openAngle3 = std::min(openAngle3 + ANIMATION_SPEED, MAX_ANGLE);
                }
                else {
                    currentFace = 3;
                }
            }
            else if (currentFace == 3) {
                if (openAngle4 < MAX_ANGLE) {
                    openAngle4 = std::min(openAngle4 + ANIMATION_SPEED, MAX_ANGLE);
                }
                else {
                    isOpening = false;
                    currentFace = 0; 
                }
            }
        }
        else {  
            if (currentFace == 0) {
                if (openAngle1 > 0) {
                    openAngle1 = std::max(openAngle1 - ANIMATION_SPEED, 0.0f);
                }
                else {
                    currentFace = 1;
                }
            }
            else if (currentFace == 1) {
                if (openAngle2 > 0) {
                    openAngle2 = std::max(openAngle2 - ANIMATION_SPEED, 0.0f);
                }
                else {
                    currentFace = 2;
                }
            }
            else if (currentFace == 2) {
                if (openAngle3 > 0) {
                    openAngle3 = std::max(openAngle3 - ANIMATION_SPEED, 0.0f);
                }
                else {
                    currentFace = 3;
                }
            }
            else if (currentFace == 3) {
                if (openAngle4 > 0) {
                    openAngle4 = std::max(openAngle4 - ANIMATION_SPEED, 0.0f);
                }
                else {
                    isOpening = true;
                    currentFace = 0;  
                }
            }
        }

    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}


void drawScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    if (isOrtho) {
        projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 50.0f);
    }
    else {
        projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 50.0f);
    }

    view = glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");
    GLuint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(lightPosLoc, 5.0f, 5.0f, 5.0f);

    if (change) {
        model = glm::mat4(1.0f);
        glBindVertexArray(vao[0]);
        model = glm::rotate(model, glm::radians(currentRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // 뒷면
        glm::mat4 backModel = model;
        backModel = glm::translate(backModel, glm::vec3(0.5f, 0.5f, 0.0f));
        backModel = glm::scale(backModel, glm::vec3(behindscale, behindscale, 1.0f));
        backModel = glm::translate(backModel, glm::vec3(-0.5f, -0.5f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(backModel));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // 왼면
        glm::mat4 leftModel = model;
        leftModel = glm::translate(leftModel, glm::vec3(0.0f, sideRotationAngle, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(leftModel));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));


        //윗면
        glm::mat4 topModel = model;
        topModel = glm::translate(topModel, glm::vec3(0.5f, 1.0f, 0.5f));
        topModel = glm::rotate(topModel, glm::radians(topRotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        topModel = glm::translate(topModel, glm::vec3(-0.5f, -1.0f, -0.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(topModel));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));


        // 오른면
        glm::mat4 rightmModel = model;
        rightmModel = glm::translate(rightmModel, glm::vec3(0.0f, sideRotationAngle, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rightmModel));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint)));

        // 아랫면
        glm::mat4 bottomModel = model;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bottomModel));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(GLuint)));

        // 앞면
        glm::mat4 frontModel = model;
        frontModel = glm::translate(frontModel, glm::vec3(0.5f, 0.0f, 1.0f));
        frontModel = glm::rotate(frontModel, glm::radians(frontRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        frontModel = glm::translate(frontModel, glm::vec3(-0.5f, 0.0f, -1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(frontModel));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(GLuint)));
    }

    else {
        glBindVertexArray(vao[1]);
        glm::mat4 baseModel = glm::mat4(1.0f);
        baseModel = glm::rotate(baseModel, glm::radians(currentRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        baseModel = glm::translate(baseModel, glm::vec3(0.0f, 0.0f, 2.0f));
        baseModel = glm::rotate(baseModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        baseModel = glm::scale(baseModel, glm::vec3(2.0f, 2.0f, 2.0f));

        // 바닥면 (고정)
        glm::mat4 bottomModel = baseModel;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bottomModel));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glm::mat4 frontModel = baseModel;
        frontModel = glm::translate(frontModel, glm::vec3(0.5f, 0.0f, 0.0f));
        frontModel = glm::rotate(frontModel, glm::radians(openAngle1), glm::vec3(1.0f, 0.0f, 0.0f));
        frontModel = glm::translate(frontModel, glm::vec3(-0.5f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(frontModel));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint)));

        glm::mat4 rightModel = baseModel;
        rightModel = glm::translate(rightModel, glm::vec3(1.0f, 0.0f, 0.0f));  // 아래쪽 모서리로 이동
        rightModel = glm::rotate(rightModel, glm::radians(openAngle2), glm::vec3(0.0f, 1.0f, 0.0f));  // X축 기준으로 회전
        rightModel = glm::translate(rightModel, glm::vec3(-1.0f, 0.0f, 0.0f));  // 원래 위치로
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rightModel));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(9 * sizeof(GLuint)));

        glm::mat4 backModel = baseModel;
        backModel = glm::translate(backModel, glm::vec3(0.5f, 1.0f, 0.0f));  // 아래쪽 모서리로 이동
        backModel = glm::rotate(backModel, glm::radians(openAngle3), glm::vec3(-1.0f, 0.0f, 0.0f));  // -Y축 기준으로 회전
        backModel = glm::translate(backModel, glm::vec3(-0.5f, -1.0f, 0.0f));  // 원래 위치로
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(backModel));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));


        glm::mat4 leftModel = baseModel;
        leftModel = glm::translate(leftModel, glm::vec3(0.0f, 0.5f, 0.0f));
        leftModel = glm::rotate(leftModel, glm::radians(openAngle4), glm::vec3(0.0f, -1.0f, 0.0f));
        leftModel = glm::translate(leftModel, glm::vec3(0.0f, -0.5f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(leftModel));
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(15 * sizeof(GLuint)));

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
    glutCreateWindow("OBJ Loader Example with Selectable Faces");
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    read_obj_file("cube.obj", cubeVertexData, cubeIndices, faceColors);
    read_obj_file("pyramid.obj", prismVertexData, prismIndices, prismFaceColors);



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