// IMPORTS

#include <GL/glut.h>
#include <stdio.h>

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

void setupCamera() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 300 / 300, 0.1f, 300.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    printf("Camera -> (%f, %f, %f)\n", observerCoordinates.x, observerCoordinates.y, observerCoordinates.z);

    gluLookAt(observerCoordinates.x, observerCoordinates.y, observerCoordinates.z,
              observedCoordinates.x,observedCoordinates.y , observedCoordinates.z,
              0, 1, 0);
}

void setupLights() {
    GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
    GLfloat shininess[] = { 50 };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
    GLfloat lightPosition[] = { 0.0f, 0.0f, 5.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}

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
  setupLights();
  setupCamera();
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
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);
  glutMainLoop();
}
