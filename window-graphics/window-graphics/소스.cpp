#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <random>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int windowWidth = 800, windowHeight = 600;
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0, 1);
bool isDragging = false;

struct Rectangles {
    float x, y, width, height;
    float r, g, b;
};

Rectangles eraser;
vector<Rectangles> rectangles;

void createRandomRectangle(int count) {
    for (int i = 0; i < count; i++) {
        Rectangles rect;
        rect.width = dis(gen) * 1.0f - 0.4f;
        rect.height = dis(gen) * 1.0f - 0.4f;
        rect.x = dis(gen) * 2.0f - 1.0f;
        rect.y = dis(gen) * 2.0f - 1.0f;
        rect.r = dis(gen);
        rect.g = dis(gen);
        rect.b = dis(gen);
        rectangles.push_back(rect);
    }
    glutPostRedisplay();
}

void randomRectangle() {
    srand(static_cast<unsigned int>(time(0)));
    int a = 5 + rand() % 6;
    createRandomRectangle(a);
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
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& rect : rectangles) {
        drawRectangle(rect);
    }

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
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
    randomRectangle();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMainLoop();
}