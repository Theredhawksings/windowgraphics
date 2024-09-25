#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <random>

double R = 0;
double G = 255;
double B = 0;

bool timer = false;

GLvoid drawScene() {
    glClearColor(R/255.f,G/255.f,B/255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid TimerFunction(int value)
{   
    if (timer) {
        
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255);

    R = dis(gen);
    G = dis(gen);
    B = dis(gen);
    glutTimerFunc(100, TimerFunction, 1);
    glutPostRedisplay();

    }
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
    switch (key) {

        case 'c': {
            R = 0;
            G = 255;
            B = 255;
            break;
        }

        case 'm': {
            R = 255;
            G = 0;
            B = 255;
            break;
        }

        case 'y': {
            R = 255;
            G = 255;
            B = 0;
            break;
        }

        case 'a': {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dis(0, 255);

            R = dis(gen);
            G = dis(gen);
            B = dis(gen);
            break;
        }

        case 'w': {
            R = 255;
            G = 255;
            B = 255;
            break;
        }

        case 'k': {
            R = 0;
            G = 0;
            B = 0;
            break;
        }
        case 't':{
            timer = !timer;
            if (timer) {
                glutTimerFunc(100, TimerFunction, 1);
            }
            break;
        }
        case 's': {
            timer = !timer;
            break;
        }
        case 'q': {
            glutLeaveMainLoop();  
            exit(0);
            break;
        }
    }
    glutPostRedisplay();
}


void glutLeaveMainLoop() {

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
    glutKeyboardFunc(Keyboard);
    glutMainLoop();


    return 0;
}

