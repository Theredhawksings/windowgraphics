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

void createRandomRectangle(int count);  // 프로토타입 선언

void randomRectangle() {
    srand(static_cast<unsigned int>(time(0)));
    int a = 20 + rand() % 21;
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

void createRandomRectangle(int count) {
    for (int i = 0; i < count; i++) {
        Rectangles rect;
        rect.width = 0.1f;
        rect.height = 0.1f;
        rect.x = dis(gen) * 2.0f - 1.0f;
        rect.y = dis(gen) * 2.0f - 1.0f;
        rect.r = dis(gen);
        rect.g = dis(gen);
        rect.b = dis(gen);
        rectangles.push_back(rect);
    }
    glutPostRedisplay();
}

GLvoid drawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto& rect : rectangles) {
        drawRectangle(rect);
    }

    if (isDragging) {
        drawRectangle(eraser); 
    }

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
}

float toGLX(int x) {
    return (x / 400.0f) - 1.0f;
}

float toGLY(int y) {
    return -(y / 300.0f) + 1.0f;//https://stackoverflow.com/questions/33851030/convert-glut-mouse-coordinates-to-opengl
}

bool isOverlapping(const Rectangles& r1, const Rectangles& r2) {
    return r1.x < r2.x + r2.width && r1.x + r1.width > r2.x && r1.y < r2.y + r2.height && r1.y + r1.height > r2.y;//AABB 충돌 https://mathmakeworld.tistory.com/106
}

void mouse(int button, int state, int x, int y) {
    float glX = toGLX(x);
    float glY = toGLY(y);

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            eraser.x = glX - 0.1f; 
            eraser.y = glY - 0.1f;
            eraser.width = 0.2f; 
            eraser.height = 0.2f;
            eraser.r = 0.0f; 
            eraser.g = 0.0f;
            eraser.b = 0.0f;

            isDragging = true; 
        }
        else if (state == GLUT_UP) {
            isDragging = false;  
        }
    }

    glutPostRedisplay();
}

GLvoid motion(int x, int y) {
    if (isDragging) {
        float glX = toGLX(x);
        float glY = toGLY(y);

        eraser.x = glX - eraser.width / 2; 
        eraser.y = glY - eraser.height / 2;

        for (int i = rectangles.size() - 1; i >= 0; i--) {
            if (isOverlapping(eraser, rectangles[i])) {
                eraser.r = rectangles[i].r;
                eraser.g = rectangles[i].g;
                eraser.b = rectangles[i].b;

                eraser.width += 0.02f;
                eraser.height += 0.02f;

                rectangles.erase(rectangles.begin() + i);
            }
        }

        glutPostRedisplay();
    }
}

GLvoid Keyboard(unsigned char key, int x, int y){

    switch (key) {

    case 'r': {
        rectangles.clear();
        randomRectangle();
        break;
    }

    case 'q': {
        glutLeaveMainLoop();
        break;
    }

    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Eraser Rectangle");

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
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(Keyboard);
    glutMainLoop();

    return 0;
}
