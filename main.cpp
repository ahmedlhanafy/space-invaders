// IMPORTS

#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include "Coordinates.h"
#include "Rotation.h"
#include "Spaceship.h"

// CONSTANTS

#define PI 3.14159265

// FIXED CONFIGURATIONS

int WINDOW_WIDTH = 700;
int WINDOW_HEIGHT = 700;

int WINDOW_POSITION_X = 150;
int WINDOW_POSITION_Y = 150;

// VARIABLE CONFIGURATIONS

Coordinates observerCoordinates(0, 3, 5);
Coordinates observedCoordinates(0, 0, 0);
Coordinates mouseCoordinates(0, 0, 0);

Spaceship player(0, 0, 2.5, 0, 0, 0, 0);
Spaceship opponent(0, 0, -3, 0, 0, 0, 0);

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
  GLfloat ambient[] = { 0.7f, 1.0f, 0.7, 1.0f };
  GLfloat diffuse[] = { 1.0f, 0.6f, 0.6, 1.0f };
  GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
  GLfloat shininess[] = { 50 };

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

  GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
  GLfloat lightPosition[] = { 3.0f, 8.0f, 15.0f, 0.0f };
  GLfloat lightDirection[] = { 0.0f, 0.0f, -1.0f, 0.0f };
  glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
}

// HANDLERS

void mouseHandler(int x, int y) {
  mouseCoordinates.x = x;
  mouseCoordinates.y = WINDOW_HEIGHT - y;

  glutPostRedisplay();
}

void keyboardHandler(unsigned char k, int x, int y) {
  if(k == 27)
      exit(0);
  if(k == 'w')
      observerCoordinates.z--;
  if(k == 's')
      observerCoordinates.z++;
  if(k == 'r')
      observerCoordinates.y++;
  if(k == 'f')
      observerCoordinates.y--;
  if(k == 'a')
      observerCoordinates.x--;
  if(k == 'd')
      observerCoordinates.x++;

  glutPostRedisplay();
}

void specialKeyboardHandler(int k, int x, int y) {
  if(k == GLUT_KEY_RIGHT)
    player.coordinates->x += 0.1;
  if(k == GLUT_KEY_LEFT)
    player.coordinates->x -= 0.1;

  glutPostRedisplay();
}

void drawOpponent(float length) {
  glPushMatrix();
  glTranslated(opponent.coordinates->x, opponent.coordinates->y, opponent.coordinates->z);
  glutSolidCube(length);
  glPopMatrix();
}

void transformOpponent() {
  srand(time(NULL));

  if(rand() % 2 == 0)
    opponent.coordinates->x += 0.01;
  else
    opponent.coordinates->x -= 0.01;

  if(opponent.coordinates->x > 3.5)
    opponent.coordinates->x -= 7;
  if(opponent.coordinates->x < -3.5)
    opponent.coordinates->x += 7;
}

// DISPLAY & ANIMATION

void display() {
  // setupLights();
  setupCamera();

  glPushMatrix();
  glTranslated(player.coordinates->x, player.coordinates->y, player.coordinates->z);
  glutSolidCube(0.7);
  glPopMatrix();

  drawOpponent(0.7);

  glFlush();
}

void animation() {
  transformOpponent();
  glutPostRedisplay();
}

// MAIN

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
  glutCreateWindow("Game");
  glutDisplayFunc(display);
  glutIdleFunc(animation);
  glutPassiveMotionFunc(mouseHandler);
  glutKeyboardFunc(keyboardHandler);
  glutSpecialFunc(specialKeyboardHandler);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);
  glutFullScreen();
  glutMainLoop();
}
