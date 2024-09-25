#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <random>

#pragma comment(lib, "glew32.lib")

double R = 0;
double G = 0;
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

double X11 = -1.0f;
double Y11 = 0.0f;
double X21 = 0.0f;
double Y21 = 1.0f;

double X12 = 0.0f;
double Y12 = 0.0f;
double X22 = 1.0f;
double Y22 = 1.0f;

double X13 = -1.0f;
double Y13 = -1.0f;
double X23 = 0.0f;
double Y23 = 0.0f;

double X14 = 0.0f;
double Y14 = -1.0f;
double X24 = 1.0f;
double Y24 = 0.0f;




void Rectange1changecolor() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255);

    rectagleR1 = dis(gen);
    rectagleG1 = dis(gen);
    rectagleB1 = dis(gen);
};

void Rectange2changecolor() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255);

    rectagleR2 = dis(gen);
    rectagleG2 = dis(gen);
    rectagleB2 = dis(gen);
};

void Rectange3changecolor() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255);

    rectagleR3 = dis(gen);
    rectagleG3 = dis(gen);
    rectagleB3 = dis(gen);
};

void Rectange4changecolor() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255);

    rectagleR4 = dis(gen);
    rectagleG4 = dis(gen);
    rectagleB4 = dis(gen);
};

void clearchangecolor() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255);

    R = dis(gen);
    G = dis(gen);
    B = dis(gen);
};

GLvoid Mouse(int button, int state, int x, int y) {

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        bool insideRectangle = false;  // 클릭이 사각형 안에 있었는지 여부

        if (x > 100 && x < 300 && y > 75 && y < 235) {
            Rectange1changecolor();
            insideRectangle = true;
        }

        if (x > 500 && x < 700 && y > 75 && y < 235) {
            Rectange2changecolor();
            insideRectangle = true;
        }

        if (x > 100 && x < 300 && y > 375 && y < 525) {
            Rectange3changecolor();
            insideRectangle = true;
        }

        if (x > 500 && x < 700 && y > 375 && y < 525) {
            Rectange4changecolor();
            insideRectangle = true;
        }

        if (!insideRectangle) {
            clearchangecolor();
        }

        glutPostRedisplay();
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

        if (x > 100 && x < 300) {
            if (y > 75 && y < 325) {
                if (X11 < -0.85f) {
                    X11 += 0.01f;
                    Y11 += 0.01f;
                    X21 -= 0.01f;
                    Y21 -= 0.01f;
                }
            }

            if (y < 75) {
                X11 -= 0.01f;
                Y11 -= 0.01f;
                X21 += 0.01f;
                Y21 += 0.01f;
            }
        }

        if (x > 500 && x < 700 && y > 75 && y < 325) {
            if (X22 > 0.85f) {
                X12 += 0.01f;
                Y12 += 0.01f;
                X22 -= 0.01f;
                Y22 -= 0.01f;
            }
        }

        if (x > 100 && x < 300 && y > 375 && y < 525) {
            if (X13 < -0.85f) {
                X13 += 0.01f;
                Y13 += 0.01f;
                X23 -= 0.01f;
                Y23 -= 0.01f;
            }
        }

        if (x > 500 && x < 700 && y > 375 && y < 525) {
            if (X24 > 0.85f) {
                X14 += 0.01f;
                Y14 += 0.01f;
                X24 -= 0.01f;
                Y24 -= 0.01f;
            }
        }

        if (x > 100 && x < 300 && y>75 && y < 325) {
            if (X11 < -0.85f) {
                X11 += 0.01f;
                Y11 += 0.01f;
                X21 -= 0.01f;
                Y21 -= 0.01f;
            }
        }

        if (x > 100 && x < 300 && y > 375 && y < 525) {
            if (X13 < -0.85f) {
                X13 += 0.01f;
                Y13 += 0.01f;
                X23 -= 0.01f;
                Y23 -= 0.01f;
            }
        }

        if (x > 500 && x < 700 && y > 375 && y < 525) {
            if (X24 > 0.85f) {
                X14 += 0.01f;
                Y14 += 0.01f;
                X24 -= 0.01f;
                Y24 -= 0.01f;
            }
        }
    }

}


GLvoid drawScene() {
    glClearColor(R / 255.f, G / 255.f, B / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(rectagleR1 / 255.f, rectagleG1 / 255.f, rectagleB1 / 255.f);
    glRectf(X11, Y11, X21, Y21);

    glColor3f(rectagleR2 / 255.f, rectagleG2 / 255.f, rectagleB2 / 255.f);
    glRectf(X12, Y12, X22, Y22);

    glColor3f(rectagleR3 / 255.f, rectagleG3 / 255.f, rectagleB3 / 255.f);
    glRectf(X13, Y13, X23, Y23);

    glColor3f(rectagleR4 / 255.f, rectagleG4 / 255.f, rectagleB4 / 255.f);
    glRectf(X14, Y14, X24, Y24);

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