#define _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include <stdio.h>

GLint width, height;
GLuint vertexShader, fragmentShader, shaderProgramID;
GLint result;
GLchar errorLog[512];

char* filetobuf(const char* file){
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
    GLchar* vertexSource;
    vertexSource = filetobuf("vertex.glsl"); 

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

    if (!result) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }

}

void makeFragmentShaders() {
    GLchar* fragmentSource;
    fragmentSource = filetobuf("fragment.glsl");

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}

GLuint makeShaderProgram() {
    GLuint shaderID = glCreateProgram();

    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);

    glLinkProgram(shaderID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
        return false;
    }
    return shaderID;
}

GLvoid DrawScene() {
    GLfloat rColor = 0.0, gColor = 1.0, bColor = 0.0;

    glClearColor(rColor, gColor, bColor, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);

    glutCreateWindow("Example1");
    glewExperimental = GL_TRUE;
    glewInit();

    makeVertexShaders();
    makeFragmentShaders();
    shaderProgramID = makeShaderProgram();

    glutDisplayFunc(DrawScene);
    glutReshapeFunc(Reshape);
    glutMainLoop();
}
