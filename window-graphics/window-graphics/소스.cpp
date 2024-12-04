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


GLuint axisVAO, axisVBO;
const float axisLength = 5.0f;


glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 15.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


glm::mat4 model, view, projection;

glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 2.0f);  

bool ambientEnabled = true;
int currentLightColor = 0;
bool isLightRotating = false;
float lightRotationAngle = 0.0f;

const std::vector<glm::vec3> lightColors = {
    glm::vec3(1.0f, 1.0f, 1.0f), 
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),  
    glm::vec3(0.0f, 0.0f, 1.0f)  // 초록색
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    int faceIndex;
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
GLuint lightVAO, lightVBO;

std::array<std::array<glm::vec3, 4>, 6> face1Colors = { {
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)}},
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)}},
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)}},
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)}},
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)}},
    {{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)}}
} };

std::array<std::array<glm::vec3, 4>, 6> face2Colors = { {
    {{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f)}},
    {{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f)}},
    {{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f)}},
    {{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f)}},
    {{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f)}},
    {{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f)}}
} };

std::array<std::array<glm::vec3, 4>, 6> face3Colors = { {
    {{glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f)}},
    {{glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f)}},
    {{glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f)}},
    {{glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f)}},
    {{glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f)}},
    {{glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f), glm::vec3(1.0f, 0.7f, 1.0f)}}
} };

std::array<std::array<glm::vec3, 4>, 6> face4Colors = { {
    {{glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f)}},
    {{glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f)}},
    {{glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f)}},
    {{glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f)}},
    {{glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f)}},
    {{glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f), glm::vec3(1.0f, 0.5f, 1.0f)}}
} };

std::array<std::array<glm::vec3, 4>, 6> face5Colors = { {
    {{glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f)}},
    {{glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f)}},
    {{glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f)}},
    {{glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f)}},
    {{glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f)}},
    {{glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f), glm::vec3(0.0f, 0.5f, 0.8f)}}
} };

std::array<std::array<glm::vec3, 4>, 6> face6Colors = { {
    {{glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f)}},
    {{glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f)}},
    {{glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f)}},
    {{glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f)}},
    {{glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f)}},
    {{glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f), glm::vec3(0.2f, 0.0f, 0.8f)}}
} };

std::array<std::array<glm::vec3, 4>, 6> face7Colors = { {
    {{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)}},  // 노란색
    {{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)}},
    {{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)}},
    {{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)}},
    {{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)}},
    {{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)}}
} };




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

void InitBuffer() {
    glGenVertexArrays(7, vao);
    glGenBuffers(14, vbo);


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

float moveX = 0.0f;
bool isMoving = false;
float moveSpeed = 0.1f;
int moveDirection = 1;

float rotateY = 0.0f;
bool isRotating = false;
float rotateSpeed = 2.0f;
int rotateDirection = 1;

float armRotateY = 0.0f;
bool isArmRotating = false;
float armRotateSpeed = 2.0f;

bool isArmsMoving = false;
bool isArmsCombined = false;
float armsPosition = 0.0f;
float armsCombineSpeed = 0.02f;

float armRotateZ = 0.0f;
bool isArmRotatingZ = false;
float armRotateSpeedZ = 2.0f;
int armRotateDirectionZ = 1;


float cameraRotationY = 0.0f;
float cameraOrbitY = 0.0f;
float cameraMoveSpeed = 0.1f;
float cameraRotateSpeed = 0.1f;

// 전역 변수 추가
bool isOrbiting = false;  // 카메라 공전 애니메이션 상태

// 초기 상태 값 저장을 위한 상수 추가
const float INITIAL_CAMERA_Y = 5.0f;
const float INITIAL_CAMERA_Z = 5.0f;

void timer(int value) {
    if (isLightRotating) {
        lightRotationAngle += 2.0f;
        lightPos.x = 3.0f * cos(glm::radians(lightRotationAngle));
        lightPos.y = 2.0f;  // 고정된 높이
        lightPos.z = 3.0f * sin(glm::radians(lightRotationAngle));
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboardCallback(unsigned char key, int x, int y) {
    switch (key) {
    case 'm': ambientEnabled = !ambientEnabled; break;
    case 'c': currentLightColor = (currentLightColor + 1) % lightColors.size(); break;
    case 'y': isLightRotating = !isLightRotating; break;
    case 's': isLightRotating = false; break;

    case 'z':  // z축 음의 방향으로 이동
        cameraPos.z -= cameraMoveSpeed;
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        break;
    case 'Z':  // z축 양의 방향으로 이동
        cameraPos.z += cameraMoveSpeed;
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        break;
    case 'x':  // x축 음의 방향으로 이동
        cameraPos.x -= cameraMoveSpeed;
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        break;
    case 'X':  // x축 양의 방향으로 이동
        cameraPos.x += cameraMoveSpeed;
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        break;
    case 'r':
        cameraOrbitY -= cameraRotateSpeed;
        cameraPos.x = 5.0f * sin(glm::radians(cameraOrbitY));
        cameraPos.z = 15.0f * cos(glm::radians(cameraOrbitY));
        cameraPos.y = cameraPos.y;  // y축 높이 유지
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        break;
    case 'R':
        cameraOrbitY += cameraRotateSpeed;
        cameraPos.x = 5.0f * sin(glm::radians(cameraOrbitY));
        cameraPos.z = 15.0f * cos(glm::radians(cameraOrbitY));
        cameraPos.y = cameraPos.y;  // y축 높이 유지
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        break;
    case 'a':  // 공전 애니메이션 시작
    case 'A':
        isOrbiting = !isOrbiting;
        break;
    case 'Q':  // 프로그램 종료
        glutLeaveMainLoop();  // 또는 exit(0);
        break;
    }
    glutPostRedisplay();
}

void drawScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniform3fv(glGetUniformLocation(shaderProgramID, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgramID, "lightColor"), 1, glm::value_ptr(lightColors[currentLightColor]));
    glUniform1i(glGetUniformLocation(shaderProgramID, "ambientEnabled"), ambientEnabled);

    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 50.0f);

    glm::vec3 cameraDirection;
    model = glm::rotate(glm::mat4(1.0f), glm::radians(cameraRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    cameraDirection = glm::vec3(model * glm::vec4(cameraTarget - cameraPos, 0.0f));

    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgramID, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // 빨간 큐브 그리기
    glBindVertexArray(vao[0]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));  // 먼저 회전
    model = glm::translate(model, glm::vec3(moveX - 0.5f, 0.0f, -0.5f));  // 그 다음 이동
    model = glm::scale(model, glm::vec3(1.0f, 0.5f, 1.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube1Indices.size(), GL_UNSIGNED_INT, 0);

    // 노란 큐브 그리기
    glBindVertexArray(vao[1]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(moveX - 0.25f, 0.5f, -0.25f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube2Indices.size(), GL_UNSIGNED_INT, 0);

    float leftArmPosition = -0.4f * (1.0f - armsPosition);   // 왼쪽 팔 시작 위치
    float rightArmPosition = 0.2f * (1.0f - armsPosition);   // 오른쪽 팔 시작 위치

    glBindVertexArray(vao[2]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));  // 몸체 회전
    model = glm::rotate(model, glm::radians(armRotateY * (1.0f - armsPosition)), glm::vec3(0.0f, 1.0f, 0.0f));  // 팔 회전
    model = glm::translate(model, glm::vec3(moveX + leftArmPosition, 0.1f, -0.25f));
    model = glm::scale(model, glm::vec3(0.25f, 0.25f, 1.2f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube3Indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(vao[3]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));  // 몸체 회전
    model = glm::rotate(model, glm::radians(-armRotateY * (1.0f - armsPosition)), glm::vec3(0.0f, 1.0f, 0.0f));  // 반대 방향 팔 회전
    model = glm::translate(model, glm::vec3(moveX + rightArmPosition, 0.1f, -0.25f));
    model = glm::scale(model, glm::vec3(0.25f, 0.25f, 1.2f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube4Indices.size(), GL_UNSIGNED_INT, 0);


    glBindVertexArray(vao[4]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(moveX - 0.2f, 0.8f, -0.2f));
    model = glm::rotate(model, glm::radians(armRotateZ), glm::vec3(0.0f, 0.0f, 1.0f));  // Z축 회전 추가
    model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.1f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube5Indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(vao[5]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(moveX + 0.1f, 0.8f, -0.2f));
    model = glm::rotate(model, glm::radians(-armRotateZ), glm::vec3(0.0f, 0.0f, 1.0f));  // 반대 방향 Z축 회전 추가
    model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.1f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube6Indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(vao[6]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));  // 크기를 줄임
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, cube7Indices.size(), GL_UNSIGNED_INT, 0);


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
    read_obj_file("cube.obj", cube2VertexData, cube2Indices, face2Colors);
    read_obj_file("cube.obj", cube3VertexData, cube3Indices, face3Colors);
    read_obj_file("cube.obj", cube4VertexData, cube4Indices, face4Colors);
    read_obj_file("cube.obj", cube5VertexData, cube5Indices, face5Colors);
    read_obj_file("cube.obj", cube6VertexData, cube6Indices, face6Colors);
    read_obj_file("cube.obj", cube7VertexData, cube7Indices, face7Colors);

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
