#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Shape {
    GLenum type;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    GLfloat size;
    bool isRectangle;
};

std::vector<Shape> shapes;
GLint width = 800, height = 600;
char currentMode = 'p';
GLuint shaderProgram, VAO, VBO;
glm::mat4 projection;

GLuint loadShader(const char* file_path, GLenum shaderType) {
    std::string shaderCode;
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shaderFile.open(file_path);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
    const char* shaderSource = shaderCode.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

void initShaders() {
    GLuint vertexShader = loadShader("vertex.glsl", GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader("fragment.glsl", GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);
}

void setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

void updateProjection() {
    projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void DrawShapes() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    for (const auto& shape : shapes) {
        std::vector<GLfloat> data;
        for (size_t i = 0; i < shape.vertices.size(); i += 2) {
            data.push_back(shape.vertices[i]);
            data.push_back(shape.vertices[i + 1]);
            data.insert(data.end(), shape.colors.begin() + i / 2 * 4, shape.colors.begin() + (i / 2 + 1) * 4);
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        if (shape.type == GL_POINTS) {
            glPointSize(shape.size * 10.0f);
        }

        glDrawArrays(shape.type, 0, shape.vertices.size() / 2);

        if (shape.isRectangle) {
            glLineWidth(3.0f);
            std::vector<GLfloat> diagonalData = {
                shape.vertices[0], shape.vertices[1], 0.0f, 0.0f, 0.0f, 1.0f,
                shape.vertices[4], shape.vertices[5], 0.0f, 0.0f, 0.0f, 1.0f
            };
            glBufferData(GL_ARRAY_BUFFER, diagonalData.size() * sizeof(GLfloat), diagonalData.data(), GL_STATIC_DRAW);
            glDrawArrays(GL_LINES, 0, 2);
            glLineWidth(1.0f);
        }
    }

    glBindVertexArray(0);
    glutSwapBuffers();
}

void createShape(int x, int y) {
    Shape newShape;
    std::vector<GLfloat> randomColor = {
        static_cast<GLfloat>(rand()) / RAND_MAX,
        static_cast<GLfloat>(rand()) / RAND_MAX,
        static_cast<GLfloat>(rand()) / RAND_MAX,
        1.0f
    };

    GLfloat openglX = (2.0f * x) / width - 1.0f;
    GLfloat openglY = 1.0f - (2.0f * y) / height;

    GLfloat randomSize = 1.0f + static_cast<GLfloat>(rand()) / (static_cast<GLfloat>(RAND_MAX / 2.0f));

    newShape.colors = randomColor;
    newShape.isRectangle = false;
    newShape.size = randomSize;

    switch (currentMode) {
    case 'p':
        newShape.type = GL_POINTS;
        newShape.vertices = { openglX, openglY };
        newShape.colors = randomColor;
        break;
    case 'l':
        newShape.type = GL_LINES;
        newShape.vertices = {
            openglX, openglY,
            openglX + 0.1f * randomSize, openglY + 0.1f * randomSize
        };
        newShape.colors.insert(newShape.colors.end(), randomColor.begin(), randomColor.end());
        newShape.colors.insert(newShape.colors.end(), randomColor.begin(), randomColor.end());
        break;
    case 't':
        newShape.type = GL_TRIANGLES;
        newShape.vertices = {
            openglX, openglY,
            openglX + 0.1f * randomSize, openglY + 0.1f * randomSize,
            openglX - 0.1f * randomSize, openglY + 0.1f * randomSize
        };
        for (int i = 0; i < 3; i++) {
            newShape.colors.insert(newShape.colors.end(), randomColor.begin(), randomColor.end());
        }
        break;
    case 'r':
        newShape.type = GL_QUADS;
        newShape.vertices = {
            openglX - 0.1f * randomSize, openglY - 0.1f * randomSize,
            openglX + 0.1f * randomSize, openglY - 0.1f * randomSize,
            openglX + 0.1f * randomSize, openglY + 0.1f * randomSize,
            openglX - 0.1f * randomSize, openglY + 0.1f * randomSize
        };
        for (int i = 0; i < 4; i++) {
            newShape.colors.insert(newShape.colors.end(), randomColor.begin(), randomColor.end());
        }
        newShape.isRectangle = true;
        break;
    }

    shapes.push_back(newShape);
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        createShape(x, y);
    }
}

void printMode() {
    std::cout << "현재 모드: ";
    switch (currentMode) {
    case 'p': std::cout << "점 그리기" << std::endl; break;
    case 'l': std::cout << "선 그리기" << std::endl; break;
    case 't': std::cout << "삼각형 그리기" << std::endl; break;
    case 'r': std::cout << "사각형 그리기" << std::endl; break;
    }
}

void moveRandomShape(int direction) {
    if (shapes.empty()) return;

    int index = rand() % shapes.size();
    Shape& shape = shapes[index];

    GLfloat moveAmount = 0.01f;
    for (size_t i = 0; i < shape.vertices.size(); i += 2) {
        switch (direction) {
        case 0: shape.vertices[i + 1] += moveAmount; break; // 위
        case 1: shape.vertices[i + 1] -= moveAmount; break; // 아래
        case 2: shape.vertices[i] -= moveAmount; break; // 왼쪽
        case 3: shape.vertices[i] += moveAmount; break; // 오른쪽
        }
    }

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'p': case 'l': case 't': case 'r':
        currentMode = key;
        printMode();
        break;
    case 'c':
        shapes.clear();
        glutPostRedisplay();
        break;
    case 'w': moveRandomShape(0); break; // 위
    case 's': moveRandomShape(1); break; // 아래
    case 'a': moveRandomShape(2); break; // 왼쪽
    case 'd': moveRandomShape(3); break; // 오른쪽
    }
}

void specialKeyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP: moveRandomShape(0); break;
    case GLUT_KEY_DOWN: moveRandomShape(1); break;
    case GLUT_KEY_LEFT: moveRandomShape(2); break;
    case GLUT_KEY_RIGHT: moveRandomShape(3); break;
    }
}

void Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    updateProjection();
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("Shape Drawing");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    initShaders();
    setupBuffers();
    updateProjection();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glutDisplayFunc(DrawShapes);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutReshapeFunc(Reshape);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "p: 점 그리기  l: 선 그리기  t: 삼각형 그리기  r: 사각형 그리기" << std::endl;
    std::cout << "w/a/s/d: 랜덤 도형 이동 (위/왼쪽/아래/오른쪽)" << std::endl;
    printMode();

    glutMainLoop();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    return 0;
}