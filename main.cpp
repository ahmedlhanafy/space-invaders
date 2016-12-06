// IMPORTS

#include <GL/glut.h>

// CONSTANTS

#define PI 3.14159265

// CLASSES

class Coordinates {
public:
    float x, y, z;
    Coordinates(float x, float y, float z);
};

Coordinates::Coordinates(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

class Rotation {
public:
    float angle, x, y, z;
    Rotation(float angle, float x, float y, float z);
};

Rotation::Rotation(float angle, float x, float y, float z) {
    this->angle = angle;
    this->x = x;
    this->y = y;
    this->z = z;
}

// FIXED CONFIGURATIONS

int WINDOW_WIDTH = 700;
int WINDOW_HEIGHT = 700;

int WINDOW_POSITION_X = 150;
int WINDOW_POSITION_Y = 150;

// VARIABLE CONFIGURATIONS

Coordinates observerCoordinates(0, 0, 0);
Coordinates observedCoordinates(0, 0, 0);
Coordinates mouseCoordinates(0, 0, 0);

// CAMERA & LIGHTS

// HANDLERS

void mouseHandler(int x, int y) {
    mouseCoordinates.x = x;
    mouseCoordinates.y = WINDOW_HEIGHT - y;

    glutPostRedisplay();
}

void keyboardHandler(unsigned char k, int x, int y) {
    glutPostRedisplay();
}

void specialKeyboardHandler(int k, int x, int y) {
    glutPostRedisplay();
}

// DISPLAY & ANIMATION

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glFlush();
}

void animation() {
    glutPostRedisplay();
}

// MAIN

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glutCreateWindow("Game");
    glutDisplayFunc(display);
    glutIdleFunc(animation);
    glutPassiveMotionFunc(mouseHandler);
    glutKeyboardFunc(keyboardHandler);
    glutSpecialFunc(specialKeyboardHandler);
    glutMainLoop();
}
