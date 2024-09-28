#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <cstdlib> 
#include <ctime>  

using namespace std;

int windowWidth = 800, windowHeight = 600;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0, 1);

struct Rectangles {
    float x, y, width, height;
    float r, g, b;
};

vector<Rectangles> rectangles;

void drawRectangle(const Rectangles& rect) {
    glColor3f(rect.r, rect.g, rect.b);
    glBegin(GL_QUADS);
    glVertex2f(rect.x, rect.y);
    glVertex2f(rect.x + rect.width, rect.y);
    glVertex2f(rect.x + rect.width, rect.y + rect.height);
    glVertex2f(rect.x, rect.y + rect.height);
    glEnd();
}

void createRandomRectangle(int count) {
    for (int i = 0; i < count; i++) {
        Rectangles rect;

        rect.width = dis(gen) * 0.1f + 0.2f;
        rect.height = dis(gen) * 0.1f + 0.2f;

        rect.x = (rand() % (windowWidth)) / (float)windowWidth * 2.0f - 1.0f;
        rect.y = (rand() % (windowHeight)) / (float)windowHeight * 2.0f - 1.0f;

        rect.r = dis(gen);
        rect.g = dis(gen);
        rect.b = dis(gen);

        rectangles.push_back(rect);
        glutPostRedisplay();
    }
}

GLvoid drawScene() {
    glClearColor(255/255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f);
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
    srand(static_cast<unsigned int>(time(0))); // 현재 시간을 기반으로 시드 설정

    int a = 20 + rand() % 21; 
    createRandomRectangle(a);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMainLoop();
}