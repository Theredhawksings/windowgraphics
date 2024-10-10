#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <random>
#include <algorithm>
#include <math.h>

using namespace std;

const double PI = 3.14159265358979323846;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0.0, 1.0);

GLuint width = 800, height = 600;
GLuint vShader, fShader, shaderProg;
GLuint vao, vbo, cbo;

enum ShapeType { DOT = 1, LINE, TRI, QUAD, PENTA, HEXA };
enum AnimationType { NONE, LINEAR };

struct Shape {
    vector<GLfloat> verts;
    vector<GLfloat> colors;
    ShapeType type;
    float x, y;
    float size;
    bool picked;
    AnimationType anim;
    float dx, dy;
    bool isMerged;
};

vector<Shape> shapes;
int pickedIdx = -1;
bool isDrag = false;
float dragX, dragY;

void makeShape(vector<GLfloat>& v, ShapeType type, float x, float y, float size) {
    v.clear();
    float r = 0.1f * size;

    if (type == DOT) {
        v = { x - r / 5, y - r / 5, 0, x + r / 5, y - r / 5, 0, x + r / 5, y + r / 5, 0, x - r / 5, y + r / 5, 0 };
    }
    else if (type == LINE) {
        v = { x - r, y, 0, x + r, y, 0 };
    }
    else {
        for (int i = 0; i < type; i++) {
            float angle = 2.0f * PI * i / type;
            v.push_back(x + r * cos(angle));
            v.push_back(y + r * sin(angle));
            v.push_back(0.0f);
        }
    }
}

void initShapes() {
    shapes.clear();
    for (int i = 0; i < 15; i++) {
        Shape s;
        s.type = static_cast<ShapeType>(dis(gen) * 5 + 1);
        s.x = dis(gen) * 2 - 1;
        s.y = dis(gen) * 2 - 1;
        s.size = dis(gen) * 0.5 + 0.5;
        s.picked = false;
        s.anim = NONE;
        s.dx = s.dy = 0;
        s.isMerged = false;

        makeShape(s.verts, s.type, s.x, s.y, s.size);

        for (int j = 0; j < s.type * 3; j++) {
            s.colors.push_back(dis(gen));
        }

        shapes.push_back(s);
    }
}


char* readShader(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cerr << "Cannot open shader file!" << std::endl;
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    char* content = new char[length + 1];
    fseek(file, 0, SEEK_SET);

    fread(content, 1, length, file);
    content[length] = 0;
    fclose(file);

    return content;
}

void makeVertexShader() {
    GLchar* vertexSource = readShader("vertex.glsl");
    if (!vertexSource) return;

    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertexSource, NULL);
    glCompileShader(vShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
    }
    delete[] vertexSource;
}

void makeFragmentShader() {
    GLchar* fragmentSource = readShader("fragment.glsl");
    if (!fragmentSource) return;

    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fragmentSource, NULL);
    glCompileShader(fShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
    }
    delete[] fragmentSource;
}

void makeShaders() {
    makeVertexShader();
    makeFragmentShader();

    shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vShader);
    glAttachShader(shaderProg, fShader);
    glLinkProgram(shaderProg);

    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderProg, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderProg, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);
}

void drawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProg);

    for (const auto& s : shapes) {
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, s.verts.size() * sizeof(GLfloat), s.verts.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, cbo);
        glBufferData(GL_ARRAY_BUFFER, s.colors.size() * sizeof(GLfloat), s.colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(1);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (s.picked) {
            glLineWidth(3.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        switch (s.type) {
        case DOT:
        case QUAD:
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            break;
        case LINE:
            glDrawArrays(GL_LINES, 0, 2);
            break;
        case TRI:
        case PENTA:
        case HEXA:
            glDrawArrays(GL_TRIANGLE_FAN, 0, s.type);
            break;
        }

        if (s.picked) {
            glLineWidth(1.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

int findShape(float x, float y) {
    for (int i = shapes.size() - 1; i >= 0; i--) {
        float dx = x - shapes[i].x;
        float dy = y - shapes[i].y;
        if (dx * dx + dy * dy < shapes[i].size * shapes[i].size * 0.01) {
            return i;
        }
    }
    return -1;
}

void mergeShapes(int idx1, int idx2) {
    Shape& s1 = shapes[idx1];
    Shape& s2 = shapes[idx2];

    int newVertices = s1.type + s2.type;
    if (newVertices > 6) newVertices = 1;

    ShapeType newType = static_cast<ShapeType>(newVertices);
    float newX = (s1.x + s2.x) / 2.0f;
    float newY = (s1.y + s2.y) / 2.0f;
    float newSize = (s1.size + s2.size) / 2.0f;

    vector<GLfloat> newVerts;
    makeShape(newVerts, newType, newX, newY, newSize);

    vector<GLfloat> newColors;
    for (int i = 0; i < newVertices; i++) {
        int i1 = i % s1.type;
        int i2 = i % s2.type;
        for (int j = 0; j < 3; j++) {
            float newColor = (s1.colors[i1 * 3 + j] + s2.colors[i2 * 3 + j]) / 2.0f;
            newColors.push_back(newColor);
        }
    }

    s1.type = newType;
    s1.x = newX;
    s1.y = newY;
    s1.size = newSize;
    s1.verts = newVerts;
    s1.colors = newColors;
    s1.anim = LINEAR;
    s1.dx = (dis(gen) * 0.02 - 0.01);
    s1.dy = (dis(gen) * 0.02 - 0.01);
    s1.isMerged = true;

    shapes.erase(shapes.begin() + idx2);
}

void mouse(int button, int state, int x, int y) {
    float nx = (2.0f * x) / width - 1.0f;
    float ny = 1.0f - (2.0f * y) / height;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int idx = findShape(nx, ny);
        if (idx != -1) {
            pickedIdx = idx;
            shapes[idx].picked = true;
            isDrag = true;
            dragX = nx - shapes[idx].x;
            dragY = ny - shapes[idx].y;
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (isDrag && pickedIdx != -1) {
            int targetIdx = findShape(nx, ny);
            if (targetIdx != -1 && targetIdx != pickedIdx) {
                mergeShapes(pickedIdx, targetIdx);
                pickedIdx = -1;
            }
        }
        isDrag = false;
        for (auto& s : shapes) {
            s.picked = false;
        }
    }
    glutPostRedisplay();
}

void motion(int x, int y) {
    if (isDrag && pickedIdx != -1) {
        float nx = (2.0f * x) / width - 1.0f;
        float ny = 1.0f - (2.0f * y) / height;
        shapes[pickedIdx].x = nx - dragX;
        shapes[pickedIdx].y = ny - dragY;
        makeShape(shapes[pickedIdx].verts, shapes[pickedIdx].type,
            shapes[pickedIdx].x, shapes[pickedIdx].y,
            shapes[pickedIdx].size);
        glutPostRedisplay();
    }
}

bool checkCollision(const Shape& s1, const Shape& s2) {
    float dx = s1.x - s2.x;
    float dy = s1.y - s2.y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < (s1.size + s2.size) * 0.1;
}

void updateShapes() {
    for (auto& s : shapes) {
        if (!s.isMerged) continue;  // 병합된 도형만 업데이트

        s.x += s.dx;
        s.y += s.dy;

        // 화면 경계에 도달하면 방향 전환
        if (s.x <= -1 || s.x >= 1) s.dx = -s.dx;
        if (s.y <= -1 || s.y >= 1) s.dy = -s.dy;

        // 화면 내에 유지
        s.x = std::max(-1.0f, std::min(1.0f, s.x));
        s.y = std::max(-1.0f, std::min(1.0f, s.y));

        makeShape(s.verts, s.type, s.x, s.y, s.size);
    }
    glutPostRedisplay();
}

void timer(int value) {
    updateShapes();
    glutTimerFunc(16, timer, 0); // 약 60 FPS
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("Animated Shape Merging");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW 초기화 실패" << std::endl;
        exit(EXIT_FAILURE);
    }

    makeShaders();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &cbo);

    initShapes();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    return 0;
}