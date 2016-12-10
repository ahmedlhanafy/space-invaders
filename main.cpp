// IMPORTS
#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <stdio.h>
#include <time.h>
#include <vector>
#include <glut.h>
#include "Coordinates.h"
#include "Rotation.h"
#include "Bullet.h"
#include "Spaceship.h"

using namespace std;

// CONSTANTS

#define PI 3.14159265

// DEFINITIONS

void display();
void animation();
void setupCamera();
void setupLights(float playerx, float playery, float playerz);
void mouseHandler(int x, int y);
void keyboardHandler(unsigned char k, int x, int y);
void specialKeyboardHandler(int k, int x, int y);
void specialKeyboardUpHandler(int k, int x, int y);
void drawSpaceship(float length, Spaceship &spaceship);
void drawBullet(Bullet &bullet);
void drawSpaceshipBullets(Spaceship &spaceship);
void transformOpponent(Spaceship &spaceship);
void propelSpaceshipBullets(Spaceship &spaceship);
void shootBlankOrLiveBullet(Spaceship &spaceship);
void drawSkybox();

// FIXED CONFIGURATIONS

int WINDOW_WIDTH = 700;
int WINDOW_HEIGHT = 700;

int WINDOW_POSITION_X = 150;
int WINDOW_POSITION_Y = 150;

// VARIABLE CONFIGURATIONS

vector<Coordinates> playerBullets;
vector<Coordinates> opponentBullets;
GLTexture tex;
Model_3DS model_spaceship;

Coordinates observedCoordinates(0, 0, 0);
Coordinates observerCoordinates(0, 3, 5);
Coordinates mouseCoordinates(0, 0, 0);

Spaceship player(false, 0, 0, 2.5, 0, 0, 0, 0);
Spaceship opponent(true, 0, 0, -3, 0, 0, 0, 0);

Coordinates spotlights(0, 0, 0);

// DISPLAY & ANIMATION

void display() {
  setupLights(player.coordinates->x, player.coordinates->y, player.coordinates->z);
  setupCamera();

  drawSpaceship(0.7, player);
  drawSpaceship(0.7, opponent);

  glPushMatrix();
  glScaled(0.005, 0.005, 0.005);
  model_spaceship.Draw();
  glPopMatrix();

  drawSkybox();
  drawSpaceshipBullets(player);
  drawSpaceshipBullets(opponent);

  glFlush();
}

void animation() {
  transformOpponent(opponent);
  shootBlankOrLiveBullet(opponent);

  propelSpaceshipBullets(player);
  propelSpaceshipBullets(opponent);

  glutPostRedisplay();
}

void LoadAssets()
{
	model_spaceship.Load("player3d/fighter.3DS");
	tex.Load("stars.bmp"); // Loads a bitmap
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
  if(k == ' ') {
    player.bullets.push_back(Bullet(true, player.coordinates->x, player.coordinates->y, player.coordinates->z));
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

void drawSpaceship(float length, Spaceship &spaceship) {
  glPushMatrix();
  glTranslated(spaceship.coordinates->x, spaceship.coordinates->y , spaceship.coordinates->z);
  glRotated(spaceship.rotation->angle, 0, 0, -1);
  glutSolidCube(length);
  glPopMatrix();
}

void drawBullet(Bullet &bullet) {
  glPushMatrix();
  glTranslated(bullet.coordinates->x, bullet.coordinates->y, bullet.coordinates->z);
  glutSolidCube(0.2);
  glPopMatrix();
}

void drawSpaceshipBullets(Spaceship &spaceship) {
  for (unsigned int i = 0; i < spaceship.bullets.size(); i++) {
    drawBullet(spaceship.bullets[i]);
  }
}

void drawSkybox() {
	glPushMatrix();

	GLUquadricObj * qobj;
	qobj = gluNewQuadric();
	glTranslated(50,0,0);
	glRotated(90,1,0,1);
	tex.Use();	
	gluQuadricTexture(qobj,true);
	gluQuadricNormals(qobj,GL_SMOOTH);
	gluSphere(qobj,100,100,100);
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(qobj);
	
	glPopMatrix();
}
// TRANSFORMATIONS

void transformOpponent(Spaceship &spaceship) {
  srand(time(NULL));

  if(rand() % 2 == 0) {
      spaceship.coordinates->x += 0.001;
  } else {
    spaceship.coordinates->x -= 0.001;
  }

  if(spaceship.coordinates->x > 3.5) {
    spaceship.coordinates->x -= 7;
  }
  if(spaceship.coordinates->x < -3.5) {
    spaceship.coordinates->x += 7;
  }
}

void propelSpaceshipBullets(Spaceship &spaceship) {
  for (unsigned int i = 0; i < spaceship.bullets.size(); i++) {
    if(spaceship.bullets[i].isAirborne) {
      spaceship.bullets[i].coordinates->z += (spaceship.isHostile)? 0.1 : -0.1;
    }
  }
}

void shootBlankOrLiveBullet(Spaceship &spaceship) {
  if(spaceship.firingDelay++ == 200) {
    spaceship.bullets.push_back(Bullet(true, spaceship.coordinates->x, spaceship.coordinates->y, spaceship.coordinates->z));
    spaceship.firingDelay = 0;
  }
}

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

	GLfloat mat_ambient[] = { 0.4f, 0.4, 0.4, 1.0f };
	GLfloat mat_diffuse[] = { 0.5f, 0.5f, 0.5, 1.0f };
	GLfloat mat_specular[] = { 0.6f, 0.6, 0.6, 1.0f };
	GLfloat mat_shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

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

	GLfloat lightIntensity[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat light_position[] = { 0.0f,-10.0f,0.0f,0 };
	GLfloat light_direction[] = { 0, 1, 0, 0 };
	glLightfv(GL_LIGHT2, GL_POSITION, light_position);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightIntensity);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_direction);
	
	GLfloat light_position_1[] = { 0.0f,10.0f,0.0f,0.0f };
	GLfloat light_direction_1[] = { 0, -1, 0, 0 };
	glLightfv(GL_LIGHT3, GL_POSITION, light_position_1);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, lightIntensity);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light_direction_1);
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
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);
  glEnable(GL_LIGHT3);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_SMOOTH);
  LoadAssets();
  glutFullScreen();
  glutMainLoop();
}
