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

float ANIMATION_SPEED = 0.0001f; // 애니메이션 속도 제어 (값이 작을수록 느려짐)

struct Rectangles {
    float x, y, width, height;
    float r, g, b;
    int animation;
    float dx, dy;
};

vector<Rectangles> rectangles;

void createRandomRectangle(int count) {
    for (int i = 0; i < count; i++) {
        Rectangles rect;
        rect.width = dis(gen) * 0.1f + 0.3f;
        rect.height = dis(gen) * 0.1f + 0.3f;
        rect.x = dis(gen) * 1.2f - 0.6f; // -0.6 ~ 0.6
        rect.y = dis(gen) * 1.2f - 0.6f;
        rect.r = dis(gen);
        rect.g = dis(gen);
        rect.b = dis(gen);
        rect.animation = i % 4 + 1;
        rect.dx = 0.0f;
        rect.dy = 0.0f;
        rectangles.push_back(rect);
    }
    glutPostRedisplay();
}

void randomRectangle() {
    srand(static_cast<unsigned int>(time(0)));
    int a = rand() % 6 + 5;
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

float toGLX(int x) {
    return (x / 400.f) - 1.0f;
}

float toGLY(int y) {
    return -(y / 300.f) + 1.0f;
}

int checkpoint(float x, float y) {
    for (int i = rectangles.size() - 1; i >= 0; i--) {
        const auto& rect = rectangles[i];
        float left = rect.x;
        float right = rect.x + rect.width;
        float bottom = rect.y;
        float top = rect.y + rect.height;

        if (x >= left && x <= right && y >= bottom && y <= top) {
            return i;
        }
    }
    return -1;
}

void animateRectangles() {
    for (int i = rectangles.size() - 1; i >= 0; i--) {
        Rectangles& rect = rectangles[i];

        if (rect.animation >= 5) {
            rect.x += rect.dx * ANIMATION_SPEED;
            rect.y += rect.dy * ANIMATION_SPEED;

            rect.width -= 0.2f * ANIMATION_SPEED;
            rect.height -= 0.2f * ANIMATION_SPEED;

            if (rect.width <= 0.01f || rect.height <= 0.01f) {
                rectangles.erase(rectangles.begin() + i);
            }
        }
    }
}

void splitRectangle(int index, int animationumber) {
    Rectangles& rect = rectangles[index];
    float halfWidth = rect.width / 2.0f;
    float halfHeight = rect.height / 2.0f;

    // 새로운 사각형들을 먼저 추가
    /*rectangles.push_back({ rect.x, rect.y, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 0.0f, 1.0f });
    rectangles.push_back({ rect.x + halfWidth, rect.y, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 1.0f, 0.0f });
    rectangles.push_back({ rect.x, rect.y + halfHeight, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 0.0f, -1.0f });
    rectangles.push_back({ rect.x + halfWidth, rect.y + halfHeight, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, -1.0f, 0.0f });

    rectangles.erase(rectangles.begin() + index);*/

    if (animationumber == 1) {
        vector<Rectangles> newRects = {
            { rect.x, rect.y, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, -1.0f, 0.0f },
            { rect.x + halfWidth, rect.y, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 0.0f, 1.0f },
            { rect.x, rect.y + halfHeight, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 1.0f, 0.0f },
            { rect.x + halfWidth, rect.y + halfHeight, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 0.0f, -1.0f }
        };
        rectangles.erase(rectangles.begin() + index);
        rectangles.insert(rectangles.end(), newRects.begin(), newRects.end());
    }

    else if (animationumber == 2) {
        vector<Rectangles> newRects = {
            { rect.x, rect.y, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, -1.0f, 1.0f },
            { rect.x + halfWidth, rect.y, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, -1.0f, -1.0f },
            { rect.x, rect.y + halfHeight, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 1.0f, 1.0f },
            { rect.x + halfWidth, rect.y + halfHeight, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 1.0f, -1.0f }
        };
        rectangles.erase(rectangles.begin() + index);
        rectangles.insert(rectangles.end(), newRects.begin(), newRects.end());
    }

    else if (animationumber == 3) {
        vector<Rectangles> newRects = {
            { rect.x, rect.y, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 0.0f, 1.0f },
            { rect.x + halfWidth, rect.y, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 0.0f, 1.0f },
            { rect.x, rect.y + halfHeight, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 0.0f, 1.0f },
            { rect.x + halfWidth, rect.y + halfHeight, halfWidth, halfHeight, rect.r, rect.g, rect.b, 5, 0.0f, 1.0f }
        };
        rectangles.erase(rectangles.begin() + index);
        rectangles.insert(rectangles.end(), newRects.begin(), newRects.end());
    }

    else if (animationumber == 4) {
        float quaterWidth = rect.width / 4.0f;


        vector<Rectangles> newRects = {
            { rect.x, rect.y, quaterWidth, halfHeight, rect.r, rect.g, rect.b, 5, -1.0f, -1.0f },
            { rect.x + quaterWidth, rect.y, quaterWidth, halfHeight, rect.r, rect.g, rect.b, 5, 0.0f, -1.0f },
            { rect.x + quaterWidth * 2, rect.y, quaterWidth, halfHeight, rect.r, rect.g, rect.b, 5, 1.0f, -1.0f },
            { rect.x + quaterWidth * 3, rect.y, quaterWidth, halfHeight, rect.r, rect.g, rect.b, 5, 1.0f, 0.0f },
            { rect.x, rect.y + halfHeight, quaterWidth, halfHeight, rect.r, rect.g, rect.b, 5, 0.0f, -1.0f },
            { rect.x + quaterWidth, rect.y + halfHeight, quaterWidth, halfHeight, rect.r, rect.g, rect.b, 5, -1.0f, 1.0f },
            { rect.x + quaterWidth * 2, rect.y + halfHeight, quaterWidth, halfHeight, rect.r, rect.g, rect.b, 5, 0.0f, 1.0f },
            { rect.x + quaterWidth * 3, rect.y + halfHeight, quaterWidth, halfHeight, rect.r, rect.g, rect.b, 5, 1.0f, 1.0f }
        };

        rectangles.erase(rectangles.begin() + index);
        rectangles.insert(rectangles.end(), newRects.begin(), newRects.end());
    }
}

GLvoid mouse(int button, int state, int x, int y) {
    float glX = toGLX(x);
    float glY = toGLY(y);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int clickedRectangle = checkpoint(glX, glY);

        if (clickedRectangle != -1) {
            splitRectangle(clickedRectangle, rectangles[clickedRectangle].animation);
        }
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Rectangle Splitting");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Unable to initialize GLEW" << endl;
        exit(EXIT_FAILURE);
    }
    else {
        cout << "GLEW Initialized" << endl;
    }

    randomRectangle();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(mouse);

    glutIdleFunc([] {
        animateRectangles();
        glutPostRedisplay();
        });

    glutMainLoop();
}