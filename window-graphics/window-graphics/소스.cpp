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

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

GLuint width = 800, height = 600;
GLuint vertexShader, fragmentShader, shaderProgramID;
GLint result;
GLchar errorLog[512];
GLuint VAO, VBO, CBO;


int countTriangle[4];
int count = 4;

enum DrawMode { FILL, LINE };
DrawMode currentDrawMode = FILL;

struct Triangle {
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
};

std::vector<Triangle> triangles(16);

void initTriangles() {
    triangles[0].vertices = { -0.6f, 0.5f, 0.0f, -0.4f, 0.5f, 0.0f, -0.5f, 0.8f, 0.0f };
    triangles[0].colors = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

    triangles[1].vertices = { 0.4f, 0.5f, 0.0f, 0.6f, 0.5f, 0.0f, 0.5f, 0.8f, 0.0f };
    triangles[1].colors = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };

    triangles[2].vertices = { -0.6f, -0.9f, 0.0f, -0.4f, -0.9f, 0.0f, -0.5f, -0.6f, 0.0f };
    triangles[2].colors = { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f };

    triangles[3].vertices = { 0.4f, -0.9f, 0.0f, 0.6f, -0.9f, 0.0f, 0.5f, -0.6f, 0.0f };
    triangles[3].colors = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };
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

void createNewTriangle(float x, float y) {

    int quadrant = (x > 0) + 2 * (y < 0);
    float size = 0.1f + static_cast<float>(dis(gen)) * 0.2f;

    // 삼각형이 항상 위쪽을 향하게 그리기
    triangles[quadrant].vertices = {
        x - size, y, 0.0f, x + size, y, 0.0f, x, y + abs(size * 1.5f), 0.0f
    };

    triangles[quadrant].colors = {
        static_cast<float>(dis(gen)), static_cast<float>(dis(gen)), static_cast<float>(dis(gen)),
        static_cast<float>(dis(gen)), static_cast<float>(dis(gen)), static_cast<float>(dis(gen)),
        static_cast<float>(dis(gen)), static_cast<float>(dis(gen)), static_cast<float>(dis(gen))
    };

}

void countNewTriangle(float x, float y) {
    int quadrant = (x > 0) + 2 * (y < 0);
    float size = 0.1f + static_cast<float>(dis(gen)) * 0.2f;

    if (countTriangle[quadrant] >= 3) {
        return;  // 이미 최대 개수에 도달한 경우
    }

    countTriangle[quadrant]++;

    Triangle newTriangle;

    newTriangle.vertices = {
        x - size, y, 0.0f,  // 왼쪽 아래
        x + size, y, 0.0f,  // 오른쪽 아래
        x, y + std::abs(size * 1.5f), 0.0f  // 위쪽
    };

    // 색상 랜덤 생성
    newTriangle.colors = {
        static_cast<float>(dis(gen)), static_cast<float>(dis(gen)), static_cast<float>(dis(gen)),
        static_cast<float>(dis(gen)), static_cast<float>(dis(gen)), static_cast<float>(dis(gen)),
        static_cast<float>(dis(gen)), static_cast<float>(dis(gen)), static_cast<float>(dis(gen))
    };

    // 삼각형을 벡터에 추가
    triangles.push_back(newTriangle);
}



float toGLX(int x) {
    return (x / 400.0f) - 1.0f;
}

float toGLY(int y) {
    return -(y / 300.0f) + 1.0f;
    //https://stackoverflow.com/questions/33851030/convert-glut-mouse-coordinates-to-opengl
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        createNewTriangle(toGLX(x), toGLY(y));
        glutPostRedisplay();
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        countNewTriangle(toGLX(x), toGLY(y));
        glutPostRedisplay();
    }
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

        if (currentDrawMode == FILL) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 면으로 그리기
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 선으로 그리기
        }

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':
        currentDrawMode = FILL;
        break;
    case 'b':
        currentDrawMode = LINE;
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
    glutInitWindowPosition(100, 100);
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

    for (int i = 0; i <= 3; i++) {
        countTriangle[i] = 1; // 모든 요소를 0으로 초기화
    }

    glutDisplayFunc(DrawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}