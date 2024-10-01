#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Shape {
    GLenum type;
    std::vector<GLfloat> vertices;
    GLfloat color[4];
    GLfloat size;
    bool isRectangle;
};

std::vector<Shape> shapes;
GLint width = 800, height = 600;
char currentMode = 'p';

void DrawShapes() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& shape : shapes) {
        glColor4f(shape.color[0], shape.color[1], shape.color[2], shape.color[3]);

        if (shape.type == GL_POINTS) {
            glPointSize(shape.size * 10.0f);  // 점의 크기를 조정
            glBegin(GL_POINTS);
            glVertex2f(shape.vertices[0], shape.vertices[1]);
            glEnd();
        }
        else {
            glBegin(shape.type);
            for (size_t i = 0; i < shape.vertices.size(); i += 2) {
                glVertex2f(shape.vertices[i], shape.vertices[i + 1]);
            }
            glEnd();
        }

        if (shape.isRectangle) {
            glLineWidth(3.0f);  // 대각선을 굵게 설정
            glColor3f(0.0f, 0.0f, 0.0f);  // 대각선 색상을 검은색으로 설정
            glBegin(GL_LINES);
            glVertex2f(shape.vertices[0], shape.vertices[1]); // 왼쪽 아래
            glVertex2f(shape.vertices[4], shape.vertices[5]); // 오른쪽 위
            glEnd();
            glLineWidth(1.0f);  // 선 굵기를 원래대로 복원
        }
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

    // 1.0f에서 3.0f 사이의 랜덤 크기 생성
    GLfloat randomSize = 1.0f + static_cast<GLfloat>(rand()) / (static_cast<GLfloat>(RAND_MAX / 2.0f));

    std::copy(randomColor, randomColor + 4, newShape.color);
    newShape.isRectangle = false;
    newShape.size = randomSize;

    switch (currentMode) {
    case 'p':
        newShape.type = GL_POINTS;
        newShape.vertices = { openglX, openglY };
        break;
    case 'l':
        newShape.type = GL_LINES;
        newShape.vertices = { openglX, openglY,
                              openglX + 0.1f * randomSize, openglY + 0.1f * randomSize };
        break;
    case 't':
        newShape.type = GL_TRIANGLES;
        newShape.vertices = {
            openglX, openglY,
            openglX + 0.1f * randomSize, openglY + 0.1f * randomSize,
            openglX - 0.1f * randomSize, openglY + 0.1f * randomSize
        };
        break;
    case 'r':
        newShape.type = GL_QUADS;
        newShape.vertices = {
            openglX - 0.1f * randomSize, openglY - 0.1f * randomSize,
            openglX + 0.1f * randomSize, openglY - 0.1f * randomSize,
            openglX + 0.1f * randomSize, openglY + 0.1f * randomSize,
            openglX - 0.1f * randomSize, openglY + 0.1f * randomSize
        };
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

GLvoid Reshape(int w, int h) {
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
    glewInit();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // 배경색을 흰색으로 설정

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

    return 0;
}