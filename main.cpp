// IMPORTS

#include <stdio.h>
#include <time.h>
#include <vector>
#include <GL/glut.h>
#include "Coordinates.h"
#include "Rotation.h"
#include "Spaceship.h"

using namespace std;

// CONSTANTS

#define PI 3.14159265

// DEFINITIONS

void setupCamera();
void setupLights(float playerx, float playery, float playerz);
void mouseHandler(int x, int y);
void keyboardHandler(unsigned char k, int x, int y);
void specialKeyboardHandler(int k, int x, int y);
void specialKeyboardUpHandler(int k, int x, int y);
void drawPlayer(float length, Spaceship &spaceship);
void drawOpponent(float length, Spaceship &spaceship);
void drawBullet(Coordinates &coordinates);
void transformOpponent(Spaceship &spaceship);
void display();
void animation();

// FIXED CONFIGURATIONS

int WINDOW_WIDTH = 700;
int WINDOW_HEIGHT = 700;

int WINDOW_POSITION_X = 150;
int WINDOW_POSITION_Y = 150;

// VARIABLE CONFIGURATIONS

Coordinates observedCoordinates(0, 0, 0);
Coordinates observerCoordinates(0, 3, 5);
Coordinates mouseCoordinates(0, 0, 0);

Spaceship player(0, 0, 2.5, 0, 0, 0, 0);
Spaceship opponent(0, 0, -3, 0, 0, 0, 0);

Coordinates spotlights(0, 0, 0);
unsigned long int opponentBulletCounter = 0;

// CAMERA & LIGHTS

void setupCamera() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, 300 / 300, 0.1f, 300.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(observerCoordinates.x, observerCoordinates.y, observerCoordinates.z,
            observedCoordinates.x,observedCoordinates.y , observedCoordinates.z,
            0, 1, 0);
}

void setupLights(float playerx, float playery, float playerz) {
  GLfloat lmodel_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient); // fv -->float vector

	GLfloat l0Diffuse[] = { 1.0, 1.0f, 1.0f, 1.0f };
	GLfloat l0Position[] = { playerx - 0.45, playery + 0.45, playerz - 0.45, 1 };
	GLfloat l0Direction[] = { 0.0, 0.0, -1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, l0Position);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 20.0);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l0Direction);

	GLfloat l1Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat l1Position[] = { playerx + 0.45, playery + 0.45, playerz - 0.45, 1};//s homogeneous bit (sunlight 0 vs. spotlight 1 ) differene in ambient (fading/ non fading)
	GLfloat l1Direction[] = { 0.0, 0.0, -1.0 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1Diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, l1Position);// vector
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20.0);// number in (angle)
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l1Direction);
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
  if(k == 32) {
    Coordinates newCoordinates(player.coordinates->x, player.coordinates->y, player.coordinates->z);
    player.bullets.push_back(newCoordinates);
  }

  glutPostRedisplay();
}

void specialKeyboardHandler(int k, int x, int y) {
  if(k == GLUT_KEY_RIGHT) {
    player.coordinates->x += 0.1;
    player.rotation->angle += 15;
  }
  if(k == GLUT_KEY_LEFT) {
    player.coordinates->x -= 0.1;
    player.rotation->angle -= 15;
  }
}

void specialKeyboardUpHandler(int k, int x, int y) {
  if(k == GLUT_KEY_RIGHT) {
    player.rotation->angle = 0;
  }
  if(k == GLUT_KEY_LEFT) {
    player.rotation->angle = 0;
  }
}

// DRAWABLES

void drawPlayer(float length, Spaceship &spaceship) {
  glPushMatrix();
  glTranslated(spaceship.coordinates->x, spaceship.coordinates->y, spaceship.coordinates->z);
  glRotated(spaceship.rotation->angle, 0,0,-1);
  glutSolidCube(length);
  glPopMatrix();
}

void drawBullet(Coordinates &coordinates) {
  glPushMatrix();
  glTranslated(coordinates.x, coordinates.y, coordinates.z);
  glutSolidCube(0.2);
  glPopMatrix();
}

void drawOpponent(float length, Spaceship &spaceship) {
  glPushMatrix();
  glTranslated(spaceship.coordinates->x, spaceship.coordinates->y, spaceship.coordinates->z);
  glutSolidCube(length);
  glPopMatrix();
}

// TRANSFORMATIONS

void transformOpponent(Spaceship &spaceship) {
  srand(time(NULL));

  if(rand() % 2 == 0)
    spaceship.coordinates->x += 0.001;
  else
    spaceship.coordinates->x -= 0.001;

  if(spaceship.coordinates->x > 3.5)
    spaceship.coordinates->x -= 7;
  if(spaceship.coordinates->x < -3.5)
    spaceship.coordinates->x += 7;
}

void transformPlayerBullets() {
  for (unsigned char i = 0; i < player.bullets.size(); i++) {
    // TODO: remove out-of-bounds bullet objects
    player.bullets[i].z -= 0.1;
  }
}

void transformOpponentBullets() {
  for (unsigned char i = 0; i < opponent.bullets.size(); i++) {
    // TODO: remove out-of-bounds bullet objects
    opponent.bullets[i].z += 0.05;
  }

  if(opponentBulletCounter++ == 200) {
    Coordinates newCoordinates(opponent.coordinates->x, opponent.coordinates->y, opponent.coordinates->z);
    opponent.bullets.push_back(newCoordinates);
    opponentBulletCounter = 0;
  }
}

// DISPLAY & ANIMATION

void display() {
  setupLights(player.coordinates->x, player.coordinates->y, player.coordinates->z);
  setupCamera();
  drawPlayer(0.7, player);
  drawOpponent(0.7, opponent);

  for (unsigned char i = 0; i < player.bullets.size(); i++) {
    drawBullet(player.bullets[i]);
  }
	for (unsigned char i = 0; i < opponent.bullets.size(); i++) {
    drawBullet(opponent.bullets[i]);
  }

  glFlush();
}

void animation() {
  transformOpponent(opponent);
  transformPlayerBullets();
  transformOpponentBullets();

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
  glutSpecialUpFunc(specialKeyboardUpHandler);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);
  glutFullScreen();
  glutMainLoop();
}
