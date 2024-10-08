#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <random>
#include <ctime>
#include <algorithm>
#include <math.h>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

GLuint width = 800, height = 600;
GLuint vertexShader, fragmentShader, shaderProgramID;
GLint result;
GLchar errorLog[512];
GLuint VAO, VBO, CBO;

using namespace std;

const double PI = 3.14159265358979323846;

enum DrawMode { Nonee, Beeline, Triangular, Square, Pentagon };

struct Shape {
    vector<GLfloat> vertices;
    vector<GLfloat> colors;
    DrawMode currentDrawMode;
    DrawMode targetDrawMode;
    float animationProgress;
    float centerX, centerY;
    GLfloat color[3]; 
};

vector<Shape> shapes(4);

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

void buildShape(vector<GLfloat>& vertices, int vertex, float CenterX, float CenterY, float r) {
    vertices.clear();

    for (int i = 0; i < vertex; i++) {
        float angle = 2.0f * PI * i / vertex;
        float x = CenterX + r * cos(angle);
        float y = CenterY + r * sin(angle);
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }
}

void initShapes() {

    shapes[0] = { vector<GLfloat>(), vector<GLfloat>(), Beeline, Beeline, 1.0f, -0.5f, 0.5f, {1.0f, 0.0f, 0.0f} };
    shapes[1] = { vector<GLfloat>(), vector<GLfloat>(), Triangular, Triangular, 1.0f, 0.5f, 0.5f, {0.0f, 1.0f, 0.0f} };
    shapes[2] = { vector<GLfloat>(), vector<GLfloat>(), Square, Square, 1.0f, -0.5f, -0.5f, {0.0f, 0.0f, 1.0f} };
    shapes[3] = { vector<GLfloat>(), vector<GLfloat>(), Pentagon, Pentagon, 1.0f, 0.5f, -0.5f, {1.0f, 1.0f, 0.0f} };

    for (auto& shape : shapes) {
        buildShape(shape.vertices, 5, shape.centerX, shape.centerY, 0.3f);

        shape.colors.clear();

        for (int i = 0; i < shape.vertices.size() / 3; ++i) {
            shape.colors.push_back(shape.color[0]);
            shape.colors.push_back(shape.color[1]);
            shape.colors.push_back(shape.color[2]);
        }
    }
}

void makeVertexShaders() {
    GLchar* vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        exit(EXIT_FAILURE);
    }
}

void makeFragmentShaders() {
    GLchar* fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
        exit(EXIT_FAILURE);
    }
}

GLuint makeShaderProgram() {
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
        exit(EXIT_FAILURE);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderID;
}


void updateShapes() {

    for (auto& shape : shapes) {

        if (shape.currentDrawMode != shape.targetDrawMode) {
            shape.animationProgress += 0.05f;
            if (shape.animationProgress >= 1.0f) {
                shape.currentDrawMode = shape.targetDrawMode;
                shape.animationProgress = 1.0f;
            }
        }

        int currentVertices = shape.currentDrawMode == Beeline ? 2 :
            shape.currentDrawMode == Triangular ? 3 :
            shape.currentDrawMode == Square ? 4 : 5;


        int targetVertices = shape.targetDrawMode == Beeline ? 2 :
            shape.targetDrawMode == Triangular ? 3 :
            shape.targetDrawMode == Square ? 4 : 5;

        vector<GLfloat> interpolatedVertices;

        for (int i = 0; i < currentVertices; i++) {
            float startAngle = 2.0f * PI * i / currentVertices;
            float endAngle = 2.0f * PI * i / targetVertices;
            float interpolatedAngle = startAngle + shape.animationProgress * (endAngle - startAngle);

            float x = shape.centerX + 0.3f * cos(interpolatedAngle);
            float y = shape.centerY + 0.3f * sin(interpolatedAngle);

            interpolatedVertices.push_back(x);
            interpolatedVertices.push_back(y);
            interpolatedVertices.push_back(0.0f);
        }

        shape.vertices = interpolatedVertices;
    }
}

GLvoid DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    for (const auto& shape : shapes) {
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, shape.vertices.size() * sizeof(GLfloat), shape.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, shape.colors.size() * sizeof(GLfloat), shape.colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        switch (shape.currentDrawMode) {
        case Beeline:
            glDrawArrays(GL_LINES, 0, 2);
            break;
        case Triangular:
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        case Square:
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            break;
        case Pentagon:
            glDrawArrays(GL_TRIANGLE_FAN, 0, 5);
            break;
        default:
            break;
        }
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'l':
        for (auto& shape : shapes) {
            if (shape.currentDrawMode == Beeline) {
                shape.targetDrawMode = Triangular;
                shape.animationProgress = 0.0f;
            }
        }
        break;
    case 't':
        for (auto& shape : shapes) {
            if (shape.currentDrawMode == Triangular) {
                shape.targetDrawMode = Square;
                shape.animationProgress = 0.0f;
            }
        }
        break;
    case 'r':
        for (auto& shape : shapes) {
            if (shape.currentDrawMode == Square) {
                shape.targetDrawMode = Pentagon;
                shape.animationProgress = 0.0f;
            }
        }
        break;
    case 'p':
        for (auto& shape : shapes) {
            if (shape.currentDrawMode == Pentagon) {
                shape.targetDrawMode = Beeline;
                shape.animationProgress = 0.0f;
            }
        }
        break;
    case 'a':
        initShapes();
        break;
    default:
        break;
    }
}

void TimerFunction(int value) {
    updateShapes();
    glutPostRedisplay();
    glutTimerFunc(16, TimerFunction, 1);
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(width, height);
    glutCreateWindow("Four Animated Shapes Example");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }

    makeVertexShaders();
    makeFragmentShaders();
    shaderProgramID = makeShaderProgram();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CBO);

    initShapes();

    glutDisplayFunc(DrawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, TimerFunction, 1);
    glutMainLoop();

    return 0;
}