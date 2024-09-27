#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <random>
#include <algorithm>

struct Rectangles{
    float x, y, width, height;
    float r, g, b;
    bool selected;
};

std::vector<Rectangles> rectangles;
int windowWidth = 800, windowHeight = 600;
bool isDragging = false;
int dragIndex = -1;
float dragOffsetX, dragOffsetY;

// Random number generator
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1);

void drawRectangle(const Rectangles& rect) {
    glColor3f(rect.r, rect.g, rect.b);
    glBegin(GL_QUADS);
    glVertex2f(rect.x, rect.y);
    glVertex2f(rect.x + rect.width, rect.y);
    glVertex2f(rect.x + rect.width, rect.y + rect.height);
    glVertex2f(rect.x, rect.y + rect.height);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto& rect : rectangles) {
        drawRectangle(rect);
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

void createRandomRectangle() {
    if (rectangles.size() >= 10) return;

    Rectangles rect;
    rect.width = 50 + dis(gen) * 50;
    rect.height = 50 + dis(gen) * 50;
    rect.x = dis(gen) * (windowWidth - rect.width);
    rect.y = dis(gen) * (windowHeight - rect.height);
    rect.r = dis(gen);
    rect.g = dis(gen);
    rect.b = dis(gen);
    rect.selected = false;

    rectangles.push_back(rect);
    glutPostRedisplay();
}

int findRectangleAtPoint(int x, int y) {
    for (int i = rectangles.size() - 1; i >= 0; --i) {
        const auto& rect = rectangles[i];
        if (x >= rect.x && x <= rect.x + rect.width &&
            y >= rect.y && y <= rect.y + rect.height) {
            return i;
        }
    }
    return -1;
}

void mergeTouchingRectangles() {
    bool merged;
    do {
        merged = false;
        for (size_t i = 0; i < rectangles.size(); ++i) {
            for (size_t j = i + 1; j < rectangles.size(); ++j) {
                auto& rect1 = rectangles[i];
                auto& rect2 = rectangles[j];

                if (rect1.x < rect2.x + rect2.width && rect1.x + rect1.width > rect2.x &&
                    rect1.y < rect2.y + rect2.height && rect1.y + rect1.height > rect2.y) {
                    // Merge rectangles
                    float minX = std::min(rect1.x, rect2.x);
                    float minY = std::min(rect1.y, rect2.y);
                    float maxX = std::max(rect1.x + rect1.width, rect2.x + rect2.width);
                    float maxY = std::max(rect1.y + rect1.height, rect2.y + rect2.height);

                    rect1.x = minX;
                    rect1.y = minY;
                    rect1.width = maxX - minX;
                    rect1.height = maxY - minY;
                    rect1.r = dis(gen);
                    rect1.g = dis(gen);
                    rect1.b = dis(gen);

                    rectangles.erase(rectangles.begin() + j);
                    merged = true;
                    break;
                }
            }
            if (merged) break;
        }
    } while (merged);
}

void mouse(int button, int state, int x, int y) {
    y = windowHeight - y;  // Invert y coordinate

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            dragIndex = findRectangleAtPoint(x, y);
            if (dragIndex != -1) {
                isDragging = true;
                dragOffsetX = x - rectangles[dragIndex].x;
                dragOffsetY = y - rectangles[dragIndex].y;

                Rectangles selectedRect = rectangles[dragIndex];
                rectangles.erase(rectangles.begin() + dragIndex);
                rectangles.push_back(selectedRect);
                dragIndex = rectangles.size() - 1;
            }
        }
        else if (state == GLUT_UP) {
            if (isDragging) {
                isDragging = false;
                mergeTouchingRectangles();
                glutPostRedisplay();
            }
        }
    }
}

void motion(int x, int y) {
    y = windowHeight - y;  // Invert y coordinate

    if (isDragging && dragIndex != -1) {
        rectangles[dragIndex].x = x - dragOffsetX;
        rectangles[dragIndex].y = y - dragOffsetY;
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A') {
        createRandomRectangle();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Rectangle Manipulation");

    glewInit();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glutMainLoop();
    return 0;
}