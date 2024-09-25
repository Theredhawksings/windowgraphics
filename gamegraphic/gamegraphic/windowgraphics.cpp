#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <random>

double R = 0;
double G = 255;
double B = 0;

bool timer = false;

#define X 800
#define Y 600

GLvoid drawScene() {
    glClearColor(R / 255.f, G / 255.f, B / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //왼쪽 하단 오른쪽 상단
    glColor3f(1.0f, 0.0f, 0.0f);
    glRectf(X/-0.2f, 0, 0, Y/0.2f);

    glColor3f(1.0f, 0.0f, 0.0f);
    glRectf(0, 0, X/0.2f, Y/0.2f); 

    glColor3f(1.0f, 0.0f, 0.0f);
    glRectf(X / -0.2f, Y / -0.2f, 0, 0);

    glColor3f(1.0f, 0.0f, 0.0f);
    glRectf(0, Y/-0.2f, X/0.2f, 0);

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

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMainLoop();


    return 0;
}