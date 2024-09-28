#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

using namespace std;


random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0, 1);

int windowWidth = 800, windowHeight = 600;

struct Rectangles {
    float x, y, width, height;
    float r, g, b;
};

vector<Rectangles> rectangles;

void createRandomRectangle(int x, int y) {
    if (rectangles.size() >= 5) return;

    Rectangles rect;
    rect.width = 0.2f;  // �簢���� ũ�⸦ ������ ����
    rect.height = 0.2f;

    // ��ǥ ��ȯ (OpenGL ��ǥ�� ��ȯ)
    rect.x = (2.0f * x) / windowWidth - 1.0f;  // -1 ~ 1 ������ ��ȯ
    rect.y = 1.0f - (2.0f * y) / windowHeight; // y ��ǥ�� ����

    rect.r = dis(gen);  // ���� ����
    rect.g = dis(gen);
    rect.b = dis(gen);

    rectangles.push_back(rect);
    glutPostRedisplay();
}


void drawRectangle(const Rectangles& rect) {
    glColor3f(rect.r, rect.g, rect.b);
    glBegin(GL_QUADS);
    glVertex2f(rect.x, rect.y);
    glVertex2f(rect.x + rect.width, rect.y);
    glVertex2f(rect.x + rect.width, rect.y + rect.height);
    glVertex2f(rect.x, rect.y + rect.height);
    glEnd();
}

GLvoid drawScene() {
    glClearColor(105 / 255.f, 105 / 255.f, 105 / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& rect : rectangles) {
        drawRectangle(rect);
    }


    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y) {

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        createRandomRectangle(x, y);
    }
}

GLvoid keyboard(unsigned char key, int x, int y) {
  
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Example1");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "GLEW Initialized\n";
    }

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
}