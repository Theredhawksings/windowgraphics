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

double rectagleR1 = 125;
double rectagleG1 = 125;
double rectagleB1 = 124;

double rectagleR2 = 205;
double rectagleG2 = 5;
double rectagleB2 = 6;

double rectagleR3 = 95;
double rectagleG3 = 51;
double rectagleB3 = 61;

double rectagleR4 = 195;
double rectagleG4 = 151;
double rectagleB4 = 161;

GLvoid Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        if (x > 100 && x < 300 && y>75 && y < 325) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dis(0, 255);

            rectagleR1 = dis(gen);
            rectagleG1 = dis(gen);
            rectagleB1 = dis(gen);
        }
    }

}


GLvoid drawScene() {
    glClearColor(R / 255.f, G / 255.f, B / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //왼쪽 하단 오른쪽 상단
    glColor3f(rectagleR1 / 255.f, rectagleG1 / 255.f, rectagleB1 / 255.f);
    glRectf(X/-0.2f, 0, 0, Y/0.2f);

    glColor3f(rectagleR2 / 255.f, rectagleG2 / 255.f, rectagleB2 / 255.f);
    glRectf(0, 0, X/0.2f, Y/0.2f); 

    glColor3f(rectagleR3 / 255.f, rectagleG3 / 255.f, rectagleB3 / 255.f);
    glRectf(X / -0.2f, Y / -0.2f, 0, 0);

    glColor3f(rectagleR4 / 255.f, rectagleG4 / 255.f, rectagleB4 / 255.f);
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
    glutMouseFunc(Mouse);
    glutMainLoop();


    return 0;
}