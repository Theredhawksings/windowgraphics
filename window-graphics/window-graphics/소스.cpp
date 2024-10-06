#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

struct Shape {
    GLenum type;
    vector<GLfloat> vertices;
    vector<GLfloat> colors;
    GLfloat size;
};

vector<Shape> shapes;
GLint width = 800, height = 600;
char currentMode = 'p';

GLuint shaderProgram;
GLuint VBO, VAO, CBO;

string loadShaderSource(const char* filename) {
    ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        ::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << endl;
    }

    return shader;
}

void compileShaders() {
    string vertexSource = loadShaderSource("vertex.glsl");
    string fragmentSource = loadShaderSource("fragment.glsl");

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource.c_str());
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DrawShapes() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glUseProgram(shaderProgram);

    for (const auto& shape : shapes) {
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, shape.vertices.size() * sizeof(GLfloat), shape.vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, shape.colors.size() * sizeof(GLfloat), shape.colors.data(), GL_STATIC_DRAW);

        if (shape.type == GL_POINTS) {
            glPointSize(shape.size * 10.0f);
        }

        glDrawArrays(shape.type, 0, shape.vertices.size() / 2);
    }

    glutSwapBuffers();
}

void createShape(int x, int y) {
    Shape newShape;
    GLfloat randomColor[4] = { static_cast<GLfloat>(rand()) / RAND_MAX,
                                static_cast<GLfloat>(rand()) / RAND_MAX,
                                static_cast<GLfloat>(rand()) / RAND_MAX, 1.0f };

    GLfloat openglX = (2.0f * x) / width - 1.0f;
    GLfloat openglY = 1.0f - (2.0f * y) / height;

    GLfloat randomSize = 1.0f + static_cast<GLfloat>(rand()) / (static_cast<GLfloat>(RAND_MAX / 2.0f));

    newShape.size = randomSize;

    switch (currentMode) {
    case 'p':
        newShape.type = GL_POINTS;
        newShape.vertices = { openglX, openglY };
        newShape.colors = { randomColor[0], randomColor[1], randomColor[2], randomColor[3] };
        break;
    case 'l':
        newShape.type = GL_LINES;
        newShape.vertices = { openglX, openglY,
                              openglX + 0.1f * randomSize, openglY + 0.1f * randomSize };
        newShape.colors = { randomColor[0], randomColor[1], randomColor[2], randomColor[3],
                            randomColor[0], randomColor[1], randomColor[2], randomColor[3] };
        break;
    case 't':
        newShape.type = GL_TRIANGLES;
        newShape.vertices = {
            openglX, openglY,
            openglX + 0.1f * randomSize, openglY + 0.1f * randomSize,
            openglX - 0.1f * randomSize, openglY + 0.1f * randomSize
        };
        newShape.colors = { randomColor[0], randomColor[1], randomColor[2], randomColor[3],
                            randomColor[0], randomColor[1], randomColor[2], randomColor[3],
                            randomColor[0], randomColor[1], randomColor[2], randomColor[3] };
        break;
    case 'r':
        newShape.type = GL_TRIANGLES;
        newShape.vertices = {
            openglX - 0.1f * randomSize, openglY - 0.1f * randomSize,
            openglX + 0.1f * randomSize, openglY - 0.1f * randomSize,
            openglX + 0.1f * randomSize, openglY + 0.1f * randomSize,
            openglX - 0.1f * randomSize, openglY - 0.1f * randomSize,
            openglX + 0.1f * randomSize, openglY + 0.1f * randomSize,
            openglX - 0.1f * randomSize, openglY + 0.1f * randomSize
        };
        newShape.colors = { randomColor[0], randomColor[1], randomColor[2], randomColor[3],
                            randomColor[0], randomColor[1], randomColor[2], randomColor[3],
                            randomColor[0], randomColor[1], randomColor[2], randomColor[3],
                            randomColor[0], randomColor[1], randomColor[2], randomColor[3],
                            randomColor[0], randomColor[1], randomColor[2], randomColor[3],
                            randomColor[0], randomColor[1], randomColor[2], randomColor[3] };
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

void Reshape(int w, int h) {
    glViewport(0, 0, w, h);
    width = w;
    height = h;
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
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }

    compileShaders();
    setupBuffers();

    glutDisplayFunc(DrawShapes);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(Reshape);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    cout << "p: 점 그리기  l: 선 그리기  t: 삼각형 그리기  r: 사각형 그리기" << endl;
    cout << "w/a/s/d: 랜덤 도형 이동 (위/왼쪽/아래/오른쪽)" << endl;
    glutMainLoop();

    return 0;
}