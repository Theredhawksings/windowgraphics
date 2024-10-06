#define _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace std;

GLuint width = 800, height = 600;
GLuint vertexShader, fragmentShader, shaderProgramID;
GLuint VAO, VBO;

enum DrawMode { POINTSS, LINE };
DrawMode currentDrawMode = POINTSS;

struct Vertex {
    float x, y, z;
};

int number = 0;

struct Spiral {
    std::vector<Vertex> vertices;
    int currentPoint;
    float centerX, centerY;
    bool isComplete;
};

std::vector<Spiral> spirals;

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

void make_vertexShaders(){
    GLchar* vertexSource;

    vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

    if(!result){
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        cerr << "ERROR: vertex shader 컴파일 실패\n"  << errorLog << endl;
        return;
    }
}

void make_fragmentShaders() {
    GLchar* fragmentSource;

    fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << endl;
        return;
    }
}

void make_shaderProgram(){
    make_vertexShaders();
    make_fragmentShaders();

    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgramID);
}

void generateSpiral(float centerX, float centerY) {
    Spiral newSpiral;
    newSpiral.centerX = centerX;
    newSpiral.centerY = centerY;
    newSpiral.currentPoint = 0;
    newSpiral.isComplete = false;

    float a = 0.01f; // 나선의 시작 크기
    float b = 0.01f; // 나선의 증가 속도


    // 바깥쪽으로 나가는 나선
    float endX = centerX, endY = centerY; // 끝점 좌표 초기화
    for (int i = 0; i < 1080; i+=10) { // 3바퀴 (360 * 3)
        float angle = i * 0.0174533f; // 1도를 라디안으로 변환
        float r = a + b * angle;
        float x = centerX + r * cos(angle);
        float y = centerY + r * sin(angle);

        float red = (sin(angle) + 1.0f) / 2.0f;
        float green = (cos(angle) + 1.0f) / 2.0f;
        float blue = (sin(angle * 0.5f) + 1.0f) / 2.0f;

        newSpiral.vertices.push_back({ x, y, 0.0f});

        endX = x; // 끝점 업데이트
        endY = y;
    }
    

    // 안쪽으로 들어가는 나선
    float lastR = a + b * (1079 * 0.0174533f); // 마지막 반지름
    endX += 0.2f;

    for (int i = 900; i >= 0; i-=10) { // 역순으로 진행
        float angle = i * 0.0174533f;
        float r = lastR * (float(i) / 900); // 반지름을 줄여가며 안쪽으로 들어감
        float x = endX + r * cos(angle);
        float y = endY + r * sin(angle);

        float red = (cos(angle) + 1.0f) / 2.0f;
        float green = (sin(angle) + 1.0f) / 2.0f;
        float blue = (cos(angle * 0.5f) + 1.0f) / 2.0f;

        newSpiral.vertices.push_back({ x, y, 0.0f});
    }

    spirals.push_back(newSpiral);
}

GLvoid drawScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID);
    glBindVertexArray(VAO);

    glPointSize(1.0f);

    for (auto& spiral : spirals) {

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, (spiral.currentPoint + 1) * sizeof(Vertex), spiral.vertices.data(), GL_DYNAMIC_DRAW);

        if (currentDrawMode == POINTSS) {
            glDrawArrays(GL_POINTS, 0, spiral.currentPoint + 1);
        }
        else {
            glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, spiral.currentPoint + 1);
        }
    }

    glutSwapBuffers(); 
}

void timer(int value) {
    bool needRedisplay = false;

    for (auto& spiral : spirals) {
        if (!spiral.isComplete && spiral.currentPoint < spiral.vertices.size()) {
            spiral.currentPoint++;
            needRedisplay = true;
        }
        else if (!spiral.isComplete) {
            spiral.isComplete = true;
        }
    }

    if (needRedisplay) {
        glutPostRedisplay();
        glutTimerFunc(16, timer, 0);  // 약 60fps
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float centerX = (2.0f * x) / width - 1.0f;
        float centerY = 1.0f - (2.0f * y) / height;

        for (int i = 0; i < number; i++) {
            generateSpiral(centerX, centerY);
            centerX += 0.1f;
            centerY += 0.1f;
        }

        glutTimerFunc(16, timer, 0); 
        glutPostRedisplay();
    }
}

GLvoid Reshape(int w, int h){
    glViewport(0, 0, w, h);
}

void InitBuffer() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
}

GLvoid keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'p':
        currentDrawMode = POINTSS;
        break;
    case 'l':
        currentDrawMode = LINE;
        break;
    case '1':
        number = 1;
        break;
    case '2':
        number = 2;
        break;
    case '3':
        number = 3;
        break;
    case '4':
        number = 4;
        break;
    case '5':
        number = 5;
        break;
    default:
        break;
    }
    glutPostRedisplay();

}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Example1");

    glewExperimental = GL_TRUE;
    glewInit();

    make_shaderProgram();
    InitBuffer();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMouseFunc(mouse);  

    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}