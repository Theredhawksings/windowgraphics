#define _CRT_SECURE_NO_WARNINGS 

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <random>
#include <ctime>
#include <algorithm>
#include<math.h>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

GLuint width = 800, height = 800;
GLuint vertexShader, fragmentShader, shaderProgramID;
GLint result;
GLchar errorLog[512];
GLuint VAO, VBO, CBO;


int countTriangle[4];
int count = 4;
const double PI = 3.14159265358979323846;

enum DrawMode { Nonee, bounce, zigzag, SquareSpiral, CircleSpiral };
DrawMode currentDrawMode = Nonee;

struct Triangle {
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    GLfloat dx, dy;
    GLfloat speed;
    GLfloat angle;
};

std::vector<Triangle> triangles(4);

void initTriangles() {
    triangles[0].vertices = { -0.6f, 0.5f, 0.0f, -0.4f, 0.5f, 0.0f, -0.5f, 0.8f, 0.0f };
    triangles[0].colors = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
    triangles[0].dx = 0.01f;
    triangles[0].dy = 0.01f;

    triangles[1].vertices = { 0.4f, 0.5f, 0.0f, 0.6f, 0.5f, 0.0f, 0.5f, 0.8f, 0.0f };
    triangles[1].colors = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
    triangles[1].dx = 0.01f;
    triangles[1].dy = 0.01f;

    triangles[2].vertices = { -0.6f, -0.9f, 0.0f, -0.4f, -0.9f, 0.0f, -0.5f, -0.6f, 0.0f };
    triangles[2].colors = { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f };
    triangles[2].dx = 0.01f;
    triangles[2].dy = 0.01f;

    triangles[3].vertices = { 0.4f, -0.9f, 0.0f, 0.6f, -0.9f, 0.0f, 0.5f, -0.6f, 0.0f };
    triangles[3].colors = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };
    triangles[3].dx = 0.01f;
    triangles[3].dy = 0.01f;

    triangles[0].speed = 0.01f;
    triangles[1].speed = 0.02f;
    triangles[2].speed = 0.03f;
    triangles[3].speed = 0.04f;

    triangles[0].angle = 0.0f;
    triangles[1].angle = PI / 2;
    triangles[2].angle = PI;
    triangles[3].angle = 3 * PI / 2;

}

char* filetobuf(const char* file) {
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb");
    if (!fptr)
        return NULL;
    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (char*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);
    buf[length] = 0;
    return buf;
}

void makeVertexShaders() {
    GLchar* vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        exit(EXIT_FAILURE);
    }
}

void makeFragmentShaders() {
    GLchar* fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
        exit(EXIT_FAILURE);
    }
}

GLuint makeShaderProgram() {
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
        exit(EXIT_FAILURE);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderID;
}

float toGLX(int x) {
    return (x / 400.0f) - 1.0f;
}

float toGLY(int y) {
    return -(y / 300.0f) + 1.0f;
    //https://stackoverflow.com/questions/33851030/convert-glut-mouse-coordinates-to-opengl
}

GLvoid DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    for (const auto& triangle : triangles) {
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, triangle.vertices.size() * sizeof(GLfloat), triangle.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, triangle.colors.size() * sizeof(GLfloat), triangle.colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glutSwapBuffers();
}

void bounceMovement(Triangle& triangle) {
    float moveX = triangle.dx * triangle.speed * 100;
    float moveY = triangle.dy * triangle.speed * 100;

    if (triangle.vertices[0] + moveX <= -1.0f || triangle.vertices[3] + moveX >= 1.0f) {
        triangle.dx = -triangle.dx;
        moveX = -moveX;
    }
    if (triangle.vertices[1] + moveY <= -1.0f || triangle.vertices[7] + moveY >= 1.0f) {
        triangle.dy = -triangle.dy;
        moveY = -moveY;
    }

    for (size_t i = 0; i < triangle.vertices.size(); i += 3) {
        triangle.vertices[i] += moveX;
        triangle.vertices[i + 1] += moveY;
    }
}

void updateZigzagMovement(Triangle& triangle) {
    const float LEFT_WALL = -1.0f;
    const float RIGHT_WALL = 1.0f;
    const float BOTTOM = -1.0f;
    const float MOVE_DOWN = 0.1f;

    for (size_t i = 0; i < triangle.vertices.size(); i += 3) {
        triangle.vertices[i] += triangle.dx * triangle.speed * 100;
    }

    if (triangle.vertices[0] <= LEFT_WALL || triangle.vertices[3] >= RIGHT_WALL) {
        triangle.dx = -triangle.dx;  // 방향 전환

        if (triangle.vertices[1] > -1.0f && triangle.vertices[4] > -1.0f) {
            for (size_t i = 1; i < triangle.vertices.size(); i += 3) {
                triangle.vertices[i] -= MOVE_DOWN;
            }
        }
    }

    if (triangle.vertices[1] < -1.0f || triangle.vertices[4] < -1.0f) {
        triangle.vertices[1] = -1.0f;
        triangle.vertices[4] = -1.0f;
    }


}

const int LEFT = 0, DOWN = 1, RIGHT = 2, UP = 3;
std::vector<int> directions;
std::vector<float> stepsLeftVec;
std::vector<int> laps;
const float initialSpeed = 5.0f;

void initializeSquareSpiralMovement() {
    directions = std::vector<int>(triangles.size(), LEFT);
    stepsLeftVec = std::vector<float>(triangles.size(), initialSpeed);
    laps = std::vector<int>(triangles.size(), 0);
}

void updateSquareSpiralMovement(std::vector<Triangle>& triangles) {

    for (size_t i = 0; i < triangles.size(); i++) {
        Triangle& currentTriangle = triangles[i];
        int& direction = directions[i];
        float& stepsLeft = stepsLeftVec[i];
        int& lap = laps[i];

        switch (direction) {
        case LEFT:
            for (size_t j = 0; j < currentTriangle.vertices.size(); j += 3) {
                currentTriangle.vertices[j] -= currentTriangle.speed;
                if (stepsLeft <= 0) {
                    direction = DOWN;
                    lap++;
                    stepsLeft = initialSpeed - 0.3f * lap;
                    break;
                }
                stepsLeft -= currentTriangle.speed;
            }
            break;
        case DOWN:
            for (size_t j = 1; j < currentTriangle.vertices.size(); j += 3) {
                currentTriangle.vertices[j] -= currentTriangle.speed;
                if (stepsLeft <= 0) {
                    direction = RIGHT;
                    lap++;
                    stepsLeft = initialSpeed - 0.3f * lap;
                    break;
                }
                stepsLeft -= currentTriangle.speed;
            }
            break;
        case RIGHT:
            for (size_t j = 0; j < currentTriangle.vertices.size(); j += 3) {
                currentTriangle.vertices[j] += currentTriangle.speed;
                if (stepsLeft <= 0) {
                    direction = UP;
                    lap++;
                    stepsLeft = initialSpeed - 0.3f * lap;
                    break;
                }
                stepsLeft -= currentTriangle.speed;
            }
            break;
        case UP:
            for (size_t j = 1; j < currentTriangle.vertices.size(); j += 3) {
                currentTriangle.vertices[j] += currentTriangle.speed;
                if (stepsLeft <= 0) {
                    direction = LEFT;
                    lap++;
                    stepsLeft = initialSpeed - 0.3f * lap;
                    break;
                }
                stepsLeft -= currentTriangle.speed;
            }
            break;
        }
    }
}

static std::vector<double> radii(triangles.size(), 400.0);  // 각 삼각형별 반지름
void updateCircleSpiralMovement(std::vector<Triangle>& triangles) {
    const float radiusDecreaseRate = 80;  // 반지름 감소 속도

    for (size_t i = 0; i < triangles.size(); i++) {
        Triangle& currentTriangle = triangles[i];

        currentTriangle.angle += currentTriangle.speed * 5;

        radii[i] -= currentTriangle.speed * 5;
        if (radii[i] < 10) radii[i] = 10;  // 완전히 0이 되지 않도록

        currentTriangle.vertices[0] = radii[i] * cos(currentTriangle.angle) / 400.0f;
        currentTriangle.vertices[1] = radii[i] * sin(currentTriangle.angle) / 400.0f;

        currentTriangle.vertices[3] = currentTriangle.vertices[0] + 0.2f;
        currentTriangle.vertices[4] = currentTriangle.vertices[1];

        currentTriangle.vertices[6] = currentTriangle.vertices[0] + 0.1f;
        currentTriangle.vertices[7] = currentTriangle.vertices[1] + 0.3f;
    }
}

void update(int value) {
    switch (currentDrawMode) {
    case bounce:
        for (auto& triangle : triangles) {
            bounceMovement(triangle);
        }
        break;
    case zigzag:
        for (auto& triangle : triangles) {
            updateZigzagMovement(triangle);
        }
        break;
    case SquareSpiral:
        updateSquareSpiralMovement(triangles);
        break;
    case CircleSpiral:
        updateCircleSpiralMovement(triangles);
        break;
    default:
        break;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    initTriangles();

    switch (key) {
    case '1':
        currentDrawMode = bounce;
        break;
    case '2':
        currentDrawMode = zigzag;
        break;
    case '3':
        currentDrawMode = SquareSpiral;
        initializeSquareSpiralMovement();
        for (size_t i = 0; i < triangles.size(); i++) {
            Triangle& currentTriangle = triangles[i];

            // 각 삼각형의 초기 위치 설정
            if (i == 0) {
                for (size_t j = 0; j < 3; j += 3) {
                    currentTriangle.vertices[j] = 0.7f;
                    currentTriangle.vertices[j + 1] = 0.6f;
                    currentTriangle.vertices[j + 3] = currentTriangle.vertices[j] + 0.2f;
                    currentTriangle.vertices[j + 4] = currentTriangle.vertices[j + 1];
                    currentTriangle.vertices[j + 6] = currentTriangle.vertices[j] + 0.1f;
                    currentTriangle.vertices[j + 7] = currentTriangle.vertices[j + 1] + 0.3f;
                }
                currentTriangle.speed = 0.01f;  // 첫 번째 삼각형의 속도 설정
            }
            else {
                Triangle& previousTriangle = triangles[i - 1];
                for (size_t j = 0; j < currentTriangle.vertices.size(); j += 3) {
                    currentTriangle.vertices[j] = previousTriangle.vertices[j] - 0.001f;
                    currentTriangle.vertices[j + 1] = previousTriangle.vertices[j + 1] - 0.001f;
                }
                currentTriangle.speed = previousTriangle.speed * 0.8f;  // 이전 삼각형 속도의 80%로 설정
            }
        }
        break;
    case '4':
        currentDrawMode = CircleSpiral;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}



GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(width, height);
    glutCreateWindow("Four Triangles Example");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Unable to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }

    makeVertexShaders();
    makeFragmentShaders();
    shaderProgramID = makeShaderProgram();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CBO);

    initTriangles();

    glutDisplayFunc(DrawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(100, update, 0);
    glutMainLoop();

    return 0;
}