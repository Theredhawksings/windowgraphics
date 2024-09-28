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
int dragIndex = -1;
float dragOffsetX, dragOffsetY;
bool isDragging = false;

struct Rectangles {
    float x, y, width, height;
    float r, g, b;
};

vector<Rectangles> rectangles;

float toGLX(int x) {
    return (x / static_cast<float>(windowWidth)) * 2.0f - 1.0f;
}

float toGLY(int y) {
    return 1.0f - (y / static_cast<float>(windowHeight)) * 2.0f;
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
        rect.width = dis(gen) * 0.1f + 0.1f;  // Changed from 0.0f to 0.1f
        rect.height = dis(gen) * 0.1f + 0.1f;  // Changed from 0.0f to 0.1f
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
    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
}

int findRectangleAtPoint(float x, float y) {
    for (int i = rectangles.size() - 1; i >= 0; i--) {  // Changed loop direction
        const auto& rect = rectangles[i];
        if (x >= rect.x && x <= rect.x + rect.width &&
            y >= rect.y && y <= rect.y + rect.height) {
            return i;
        }
    }
    return -1;
}

void mouse(int button, int state, int x, int y) {
    float glX = toGLX(x);
    float glY = toGLY(y);

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            dragIndex = findRectangleAtPoint(glX, glY);
            if (dragIndex != -1) {
                isDragging = true;
                dragOffsetX = glX - rectangles[dragIndex].x;
                dragOffsetY = glY - rectangles[dragIndex].y;
                rectangles[dragIndex].width *= 2;
                rectangles[dragIndex].height *= 2;
            }
        }
        else if (state == GLUT_UP) {
            if (isDragging && dragIndex != -1) {
                rectangles[dragIndex].width /= 2;
                rectangles[dragIndex].height /= 2;
            }
            isDragging = false; 
        }
    }
    glutPostRedisplay();
}

void motion(int x, int y) {
    if (isDragging && dragIndex != -1) {
        float glX = toGLX(x);
        float glY = toGLY(y);

        for (int i = 0; i < rectangles.size(); i++) {
            if (dragIndex != i) {
                bool overlapX = rectangles[dragIndex].x < rectangles[i].x + rectangles[i].width &&
                    rectangles[dragIndex].x + rectangles[dragIndex].width > rectangles[i].x;
                bool overlapY = rectangles[dragIndex].y < rectangles[i].y + rectangles[i].height &&
                    rectangles[dragIndex].y + rectangles[dragIndex].height > rectangles[i].y;

                if (overlapX && overlapY) {
                    rectangles[dragIndex].r = rectangles[i].r;
                    rectangles[dragIndex].g = rectangles[i].g;
                    rectangles[dragIndex].b = rectangles[i].b;

                    rectangles.erase(rectangles.begin() + i);

                    if (i < dragIndex) {
                        dragIndex--; 
                    }

                    break;
                }
            }
        }

        if (dragIndex < rectangles.size()) {  
            rectangles[dragIndex].x = glX - dragOffsetX;
            rectangles[dragIndex].y = glY - dragOffsetY;
        }
        else {
            isDragging = false;
            dragIndex = -1;
        }

        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Draggable Rectangles");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "GLEW Initialized\n";
    }

    srand(static_cast<unsigned int>(time(0)));
    int a = 20 + rand() % 21;
    createRandomRectangle(a);

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();

    return 0;
}