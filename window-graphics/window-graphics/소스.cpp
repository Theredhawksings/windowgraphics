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

bool timer1 = false;
bool timer2 = false;
bool timer3 = false;


struct Rectangles {
    float x, y, width, height;
    float r, g, b;
    float dx, dy;  
};

vector<Rectangles> rectangles;

void createRandomRectangle(int x, int y) {
    if (rectangles.size() >= 5) return;

    Rectangles rect;
    rect.width = 0.2f;  
    rect.height = 0.2f; 

    rect.x = (2.0f * x) / windowWidth - 1.0f - rect.width / 2.0f;  
    rect.y = 1.0f - (2.0f * y) / windowHeight - rect.height / 2.0f; 

    rect.r = dis(gen);
    rect.g = dis(gen);
    rect.b = dis(gen);

    rect.dx = 0.1f;  
    rect.dy = 0.1f;

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

void diagonal() {

    for (auto& rect : rectangles) {
            rect.x += rect.dx;
            rect.y += rect.dy;

            if (rect.x <= -1.0f || rect.x + rect.width >= 1.0f) {
                rect.dx = -rect.dx;
            }
            if (rect.y <= -1.0f || rect.y + rect.height >= 1.0f) {
                rect.dy = -rect.dy;
            }
     }

    
}

void zigzag() {


    for (auto& rect : rectangles) {
        rect.x += rect.dx;

        if (rect.x <= -1.0f || rect.x + rect.width >= 1.0f) {
            rect.dx = -rect.dx;
        }
  
    }

    
}

void sizechange(){


    for (auto& rect : rectangles) {
        // 기본 크기
        float originalWidth = 0.2f;
        float originalHeight = 0.2f;

        rect.width = originalWidth + (dis(gen) * 1.0f);  
        rect.height = originalHeight + (dis(gen) * 1.0f); 
    }



}

GLvoid TimerFunction(int value){
    if (timer1) {
        diagonal();
    }

    if (timer2) {
        zigzag();
    }

    if (timer3) {
        sizechange();
    }

    glutTimerFunc(100, TimerFunction, 1);
    glutPostRedisplay();
}

GLvoid keyboard(unsigned char key, int x, int y) {

    switch (key) {

    case '1': {
        timer1 = !timer1;

        if (timer1) {
            glutTimerFunc(100, TimerFunction, 1);
        }
        break;
    }
    
    case '2': {
        timer2 = !timer2;

        if (timer2) {
            glutTimerFunc(100, TimerFunction, 1);
        }
       
        break;
    }

    case '3': {
        timer3 = !timer3;

        if (timer3) {
            glutTimerFunc(100, TimerFunction, 1);
        }

        break;
    }

    }
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