
// IMPORTS
#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <glut.h>
#include "Coordinates.h"
#include "Rotation.h"
#include "Bullet.h"
#include "Spaceship.h"
#include "Token.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

// CONSTANTS

#define PI 3.14159265

// DEFINITIONS

void display();
void animation();
void generateNewWaveOfOpponents();
void activateNuke();
void drawScoreAndLevel(int score, int level);
void drawGameOver();
void setupCamera();
void setupLights(float playerx, float playery, float playerz);
void mouseHandler(int x, int y);
void keyboardHandler(unsigned char k, int x, int y);
void specialKeyboardHandler(int k, int x, int y);
void specialKeyboardUpHandler(int k, int x, int y);
void drawPlayerSpaceship(Spaceship &spaceship);
void drawOpponentSpaceship(Spaceship &spaceship);
void drawBullet(Bullet &bullet);
void drawToken(Token &token);
void drawAirborneTokens(vector<Token> &tokens);
void drawSpaceshipBullets(Spaceship &spaceship);
void transformOpponent(Spaceship &spaceship, int randomNumber,
                       int opponentsCount);
void propelSpaceshipBullets(Spaceship &spaceship);
void transformTokens(vector<Token> &tokens);
void shootBlankOrLiveBullet(Spaceship &spaceship, int index);
bool detectSpaceshipHit(Spaceship &player, Spaceship &opponent);
void drawSkybox();
vector<Spaceship> initializeOpponents(int opponentsCount, int levelOfGame);
void tokenCaptured(Spaceship &spaceship, vector<Token> &tokens);
void enableToken(int type);
void disableToken(int type);
void drawNuke(Bullet &bullet);
bool propelNuke(Bullet &bullet);
// FIXED CONFIGURATIONS

int WINDOW_WIDTH = 1080;
int WINDOW_HEIGHT = 720;

const int WINDOW_POSITION_X = 150;
const int WINDOW_POSITION_Y = 150;

int opponentsCount = 5;
int opponentsBulletFiringDelay = 2;
double opponentsSpeed = 0.001;

// VARIABLE CONFIGURATIONS
GLTexture tex;
Model_3DS model_spaceship_player;
Model_3DS model_spaceship_opponent;
Model_3DS model_bullet;
Model_3DS model_token;
Model_3DS model_bomb;
bool gameOver = false;

Coordinates observedCoordinates(0, 0, 0);
Coordinates observerCoordinates(34, 3, 14);
Coordinates mouseCoordinates(0, 0, 0);

Spaceship player(false, 0, 0, 2.5, 0, 0, 0, 0);
vector<Spaceship> opponents;
vector<Token> tokens;

Coordinates spotlights(0, 0, 0);
Bullet nuke(false, 0, 0, 0);

int score = 0;

// This determines the number of opponents and the speed they shoot with
int level = 0;

int cameraMode = 0;
const int CAMERA_MODE_ONE = 0;
const int CAMERA_MODE_TWO = 1;
const int CAMERA_MODE_THREE = 2;
bool entranceAnimation = true;
bool threeBulletsMode = false;
bool nukeMode = false;
bool reverseDirectionMode = false;
bool fasterFiringRateMode = false;
// DISPLAY & ANIMATION

void display() {
  setupLights(player.coordinates->x, player.coordinates->y,
              player.coordinates->z);
  setupCamera();
  drawSkybox();

  drawPlayerSpaceship(player);
  drawSpaceshipBullets(player);

  drawAirborneTokens(tokens);

  for (unsigned int i = 0; i < opponents.size(); i++) {
    drawOpponentSpaceship(opponents[i]);
    drawSpaceshipBullets(opponents[i]);
  }

  if(nuke.isAirborne && nukeMode)
	  drawNuke(nuke);


  /*  This has to be the last method to be called in display method
      because it converts the drawing to be 2D
  */
  drawScoreAndLevel(score, level);
  if (gameOver)
    drawGameOver();

  glFlush();
}

void animation() {
  if (entranceAnimation) {
    double xLowerLimit = 0;
    double yLowerLimit = 3;
    double zLowerLimit = 5;

    observerCoordinates.x -= observerCoordinates.x <= xLowerLimit ? 0 : 0.036;
    observerCoordinates.y -= observerCoordinates.y <= yLowerLimit ? 0 : 0.022;
    observerCoordinates.z -= observerCoordinates.z <= zLowerLimit ? 0 : 0.007;

    if (observerCoordinates.x <= xLowerLimit &&
        observerCoordinates.y <= yLowerLimit &&
        observerCoordinates.z <= zLowerLimit) {
      entranceAnimation = false;
    }
  } else {
    if (!gameOver) {
      for (unsigned int i = 0; i < opponents.size(); i++) {
        transformOpponent(opponents[i], i, opponentsCount);
        shootBlankOrLiveBullet(opponents[i], i);

        if (detectSpaceshipHit(player, opponents[i])) {
          gameOver = true;
        }

        if (detectSpaceshipHit(opponents[i], player)) {
          score++;
          PlaySound("audio/Impact.wav", NULL, SND_ASYNC | SND_FILENAME);
          printf("%d\n", score);
        }
      }
      opponents.erase(
          remove_if(opponents.begin(), opponents.end(),
                    [](Spaceship &spaceship) { return spaceship.isHit; }),
          opponents.end());
      transformTokens(tokens);
      tokenCaptured(player, tokens);
    }
    propelSpaceshipBullets(player);

	  if(nukeMode && propelNuke(nuke)){
      activateNuke();
    }

    for (unsigned int i = 0; i < opponents.size(); i++) {
      propelSpaceshipBullets(opponents[i]);
    }
    generateNewWaveOfOpponents();
  }
  glutPostRedisplay();
}

void activateNuke(){
  score += opponents.size();
  opponents.clear();
  nukeMode = false;
  nuke.isAirborne = false;
  nuke.coordinates = new Coordinates(player.coordinates->x ,player.coordinates->y,player.coordinates->z);
}

void generateNewWaveOfOpponents() {
  if (opponents.empty()) {
    opponentsCount += 4;
    opponentsBulletFiringDelay += 1;
    opponentsSpeed += 0.003;
    opponents = initializeOpponents(opponentsCount, level);
    level++;
  }
}

void drawNuke(Bullet &bullet) {
	glPushMatrix();
	glTranslated(bullet.coordinates->x, bullet.coordinates->y, bullet.coordinates->z);
	glScaled(0.05, 0.05, 0.05);
	model_bomb.Draw();
	glPopMatrix();
}

bool propelNuke(Bullet &bullet) {
	if(bullet.isAirborne)
		bullet.coordinates->z -= 0.07;

  return bullet.coordinates->z < -3;
}

void drawGameOver() {
  glDisable(GL_TEXTURE_2D);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRasterPos2i(WINDOW_WIDTH / 2 - 40, WINDOW_HEIGHT / 2);
  string s = "Game Over";
  void *font = GLUT_BITMAP_HELVETICA_18;
  for (string::iterator i = s.begin(); i != s.end(); ++i) {
    char c = *i;
    glutBitmapCharacter(font, c);
  }
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopMatrix();

  glEnable(GL_TEXTURE_2D);
}

void drawScoreAndLevel(int score, int level) {
  glDisable(GL_TEXTURE_2D);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRasterPos2i(10, 10);
  std::ostringstream o;
  o << "Score: " << score << " | Level: " << level;
  string s = o.str();
  void *font = GLUT_BITMAP_HELVETICA_18;
  for (string::iterator i = s.begin(); i != s.end(); ++i) {
    char c = *i;
    glutBitmapCharacter(font, c);
  }
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopMatrix();

  glEnable(GL_TEXTURE_2D);
}

bool detectSpaceshipHit(Spaceship &spaceship1, Spaceship &spaceship2) {
  for (unsigned int i = 0; i < spaceship2.bullets.size(); i++) {
    Coordinates *spaceship1Coordinates = spaceship1.coordinates;
    Coordinates *player2BulletCoordinates = spaceship2.bullets[i].coordinates;

    // TODO: Make dimensions dynamic
    if ((int)player2BulletCoordinates->z == (int)spaceship1Coordinates->z &&
        spaceship1Coordinates->x - 0.25 < player2BulletCoordinates->x &&
        spaceship1Coordinates->x + 0.25 > player2BulletCoordinates->x &&
        spaceship2.bullets[i].isAirborne) {
      spaceship2.bullets[i].isAirborne = false;
      spaceship1.isHit = true;
      return true;
    }
  }
  return false;
}

void tokenCaptured(Spaceship &spaceship, vector<Token> &tokens) {
  for (unsigned int i = 0; i < tokens.size(); i++) {
    Coordinates *spaceshipCoordinates = spaceship.coordinates;
    Coordinates *tokenCoordinates = tokens[i].coordinates;
    if ((int)spaceshipCoordinates->z == (int)tokenCoordinates->z &&
        spaceshipCoordinates->x - 0.25 < tokenCoordinates->x &&
        spaceshipCoordinates->x + 0.25 > tokenCoordinates->x &&
        tokens[i].isAirborne) {
      tokens[i].isAirborne = false;
      if (tokens[i].type == 0 || tokens[i].type == 1) {
        PlaySound("audio/Bell.wav", NULL, SND_ASYNC | SND_FILENAME);
      } else
        PlaySound("audio/schade.wav", NULL, SND_ASYNC | SND_FILENAME);

      enableToken(tokens[i].type);
      glutTimerFunc(10000, disableToken, tokens[i].type);
      break;
    }
  }
}

void enableToken(int type) {
  switch (type) {
  case 0:
    threeBulletsMode = true;
    break;
  case 1:
    nukeMode = true;
    break;
  case 2:
    reverseDirectionMode = true;
    break;
  case 3:
    fasterFiringRateMode = true;
    break;
  default:
    break;
  }
}
void disableToken(int type) {
  switch (type) {
  case 0:
    threeBulletsMode = false;
    break;
  case 1:
    nukeMode = false;
    break;
  case 2:
    reverseDirectionMode = false;
    break;
  case 3:
    fasterFiringRateMode = false;
    break;
  default:
    break;
  }
}

void LoadAssets() {
  model_spaceship_player.Load("models/player3d/fighter.3DS");
  model_spaceship_opponent.Load("models/opponent3d/fighter.3DS");
  model_bullet.Load("models/bullet3d/Bullet.3DS");
  model_token.Load("models/token3d/token.3DS");
  model_bomb.Load("models/bomb3d/Files/Bomb.3dS");
  tex.Load("img/stars.bmp"); // Loads a bitmap
}

// HANDLERS

void mouseHandler(int x, int y) {
  mouseCoordinates.x = x;
  mouseCoordinates.y = WINDOW_HEIGHT - y;

  glutPostRedisplay();
}

void keyboardHandler(unsigned char k, int x, int y) {
  if (k == 27)
    exit(0);
  if (k == 'w')
    observerCoordinates.z--;
  if (k == 's')
    observerCoordinates.z++;
  if (k == 'r')
    observerCoordinates.y++;
  if (k == 'f')
    observerCoordinates.y--;
  if (k == 'a')
    observerCoordinates.x--;
  if (k == 'd')
    observerCoordinates.x++;
  if (k == ' ' && !gameOver) {
    if (threeBulletsMode) {
      player.bullets.push_back(Bullet(true, player.coordinates->x,
                                    player.coordinates->y,
                                    player.coordinates->z));
      player.bullets.push_back(Bullet(true, player.coordinates->x + 0.5,
                                      player.coordinates->y,
                                      player.coordinates->z));
      player.bullets.push_back(Bullet(true, player.coordinates->x - 0.5,
                                      player.coordinates->y,
                                      player.coordinates->z));
    }else if (nukeMode) {
      nuke.isAirborne = true;
      nuke.coordinates->x = player.coordinates->x;
      nuke.coordinates->y = player.coordinates->y;
      nuke.coordinates->z = player.coordinates->z;
    }else{
      player.bullets.push_back(Bullet(true, player.coordinates->x,
                                    player.coordinates->y,
                                    player.coordinates->z));
    }
    PlaySound("audio/playerShoots.wav", NULL, SND_ASYNC | SND_FILENAME);
  }
  if (k == 'c') {
    cameraMode++;
    if (cameraMode > CAMERA_MODE_THREE)
      cameraMode = 0;

    if (cameraMode == CAMERA_MODE_ONE) {
      observerCoordinates.y = -1.0;
      observerCoordinates.y = 3.0;
      observerCoordinates.z = 5.0;
    } else if (cameraMode == CAMERA_MODE_TWO) {
      observerCoordinates.y = -1.0;
      observerCoordinates.y = 3.0;
      observerCoordinates.z = 9.0;
    } else if (cameraMode == CAMERA_MODE_THREE) {
      observerCoordinates.y = -1.0;
      observerCoordinates.y = 10.0;
      observerCoordinates.z = 3.0;
    }
  }
  glutPostRedisplay();
}

void specialKeyboardHandler(int k, int x, int y) {
  if (!gameOver) {
    if (k == GLUT_KEY_RIGHT) {
      if (reverseDirectionMode) {
        player.coordinates->x -= 0.15;
        if (player.rotation->angle >= -45)
          player.rotation->angle -= 15;
      } else {
        player.coordinates->x += 0.15;
        if (player.rotation->angle <= 45)
          player.rotation->angle += 15;
      }
    }
    if (k == GLUT_KEY_LEFT) {
      if (reverseDirectionMode) {
        player.coordinates->x += 0.15;
        if (player.rotation->angle <= 45)
          player.rotation->angle += 15;
      } else {
        player.coordinates->x -= 0.15;
        if (player.rotation->angle >= -45)
          player.rotation->angle -= 15;
      }
    }
  }
}

void specialKeyboardUpHandler(int k, int x, int y) {
  if (k == GLUT_KEY_RIGHT) {
    player.rotation->angle = 0;
  }
  if (k == GLUT_KEY_LEFT) {
    player.rotation->angle = 0;
  }
}

// DRAWABLES

void drawOpponentSpaceship(Spaceship &spaceship) {
  glPushMatrix();
  glTranslated(spaceship.coordinates->x, spaceship.coordinates->y,
               spaceship.coordinates->z);
  glRotated(90, 0, -1, 0);
  glScaled(0.02, 0.02, 0.02);
  model_spaceship_opponent.Draw();
  glPopMatrix();
}

void drawPlayerSpaceship(Spaceship &spaceship) {
  glPushMatrix();
  glTranslated(spaceship.coordinates->x, spaceship.coordinates->y,
               spaceship.coordinates->z);
  glRotated(spaceship.rotation->angle, 0, 0, -1);
  glScaled(0.002, 0.002, 0.002);
  model_spaceship_player.Draw();
  glPopMatrix();
}

void drawBullet(Bullet &bullet) {
  glPushMatrix();
  glTranslated(bullet.coordinates->x, bullet.coordinates->y,
               bullet.coordinates->z);
  glScaled(0.001, 0.001, 0.001);
  glRotated(bullet.rotation->angle, bullet.rotation->x, bullet.rotation->y,
            bullet.rotation->z);
  model_bullet.Draw();
  glPopMatrix();
}

void drawSpaceshipBullets(Spaceship &spaceship) {
  for (unsigned int i = 0; i < spaceship.bullets.size(); i++) {
    if (spaceship.bullets[i].isAirborne)
      drawBullet(spaceship.bullets[i]);
  }
}

void drawToken(Token &token) {
  glPushMatrix();
  glTranslated(token.coordinates->x, token.coordinates->y,
               token.coordinates->z);
  glScaled(0.0002, 0.0001, 0.0004);
  model_token.Draw();
  glPopMatrix();
}

void drawAirborneTokens(vector<Token> &tokens) {
  for (unsigned int i = 0; i < tokens.size(); i++) {
    if (tokens[i].isAirborne)
      drawToken(tokens[i]);
  }
}

void drawSkybox() {
  glPushMatrix();
  GLUquadricObj *qobj;
  qobj = gluNewQuadric();
  glTranslated(50, 0, 0);
  glRotated(90, 1, 0, 1);
  tex.Use();
  gluQuadricTexture(qobj, true);
  gluQuadricNormals(qobj, GL_SMOOTH);
  gluSphere(qobj, 100, 100, 100);
  glDisable(GL_TEXTURE_2D);
  gluDeleteQuadric(qobj);

  glPopMatrix();
}
// TRANSFORMATIONS

void transformOpponent(Spaceship &spaceship, int randomNumber,
                       int opponentsCount) {
  srand(time(0));
  spaceship.coordinates->x += rand() % (opponentsCount + 1) == randomNumber
                                  ? opponentsSpeed
                                  : -1 * opponentsSpeed;
  if (spaceship.coordinates->x > 3.5) {
    spaceship.coordinates->x -= 7;
  }
  if (spaceship.coordinates->x < -3.5) {
    spaceship.coordinates->x += 7;
  }
}

void propelSpaceshipBullets(Spaceship &spaceship) {
  for (unsigned int i = 0; i < spaceship.bullets.size(); i++) {
    if (spaceship.bullets[i].isAirborne) {
      if (spaceship.isHostile) {
        spaceship.bullets[i].coordinates->z +=
            fasterFiringRateMode ? 0.03 : 0.01;
        spaceship.bullets[i].rotation->angle = 90;
        spaceship.bullets[i].rotation->y = -1;
      } else {
        spaceship.bullets[i].coordinates->z -= 0.01;
        spaceship.bullets[i].rotation->angle = 90;
        spaceship.bullets[i].rotation->y = 1;
      }
    }
  }
}

void transformTokens(vector<Token> &tokens) {
  for (unsigned int i = 0; i < tokens.size(); i++) {
    if (tokens[i].isAirborne)
      tokens[i].coordinates->z += 0.004;
  }
}

void shootBlankOrLiveBullet(Spaceship &spaceship, int index) {
  srand(time(0));
  if (!spaceship.isHit) {
    spaceship.firingDelay += index + 1;
    if (spaceship.firingDelay % (rand() / opponentsBulletFiringDelay) == 0) {
      spaceship.bullets.push_back(Bullet(true, spaceship.coordinates->x,
                                         spaceship.coordinates->y,
                                         spaceship.coordinates->z));
      spaceship.firingDelay = 0;
    }
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
            observedCoordinates.x, observedCoordinates.y, observedCoordinates.z,
            0, 1, 0);
}

void setupLights(float playerx, float playery, float playerz) {
  GLfloat lmodel_ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient); // fv -->float vector

  GLfloat mat_ambient[] = {0.7, 0.7, 0.7, 1.0f};
  GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8, 1.0f};
  GLfloat mat_specular[] = {0.9f, 0.9, 0.9, 1.0f};
  GLfloat mat_shininess[] = {20};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  GLfloat l0Diffuse[] = {1.0, 1.0, 1.0, 1.0f};
  GLfloat l0Position[] = {playerx - 1, playery, playerz, 1};
  GLfloat l0Direction[] = {0.0, 0.0, -1.0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, l0Diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, l0Position);
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 20.0);
  glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, l0Direction);

  GLfloat l1Diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat l1Position[] = {playerx + 1, playery, playerz,
                          1}; // s homogeneous bit (sunlight 0 vs. spotlight 1 )
                              // differene in ambient (fading/ non fading)
  GLfloat l1Direction[] = {0.0, 0.0, -1.0};
  glLightfv(GL_LIGHT1, GL_DIFFUSE, l1Diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, l1Position); // vector
  glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20.0);     // number in (angle)
  glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
  glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l1Direction);

  GLfloat lightIntensity[] = {0.3f, 0.3f, 0.3f, 1.0f};
  GLfloat light_position[] = {0.0f, -10.0f, 0.0f, 0};
  GLfloat light_direction[] = {0, 1, 0, 0};
  glLightfv(GL_LIGHT2, GL_POSITION, light_position);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, lightIntensity);
  glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_direction);

  GLfloat light_position_1[] = {0.0f, 10.0f, 0.0f, 0.0f};
  GLfloat light_direction_1[] = {0, -1, 0, 0};
  glLightfv(GL_LIGHT3, GL_POSITION, light_position_1);
  glLightfv(GL_LIGHT3, GL_DIFFUSE, lightIntensity);
  glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light_direction_1);
}

vector<Spaceship> initializeOpponents(int opponentsCount, int levelOfGame) {
  vector<Spaceship> opponents;
  for (unsigned int i = 0; i < opponentsCount; i++) {
    double interpolationUpperLimit = opponentsCount / 2;
    double xCoordinate =
        i < interpolationUpperLimit
            ? (i * 4) / interpolationUpperLimit
            : ((i - interpolationUpperLimit) * -4) / interpolationUpperLimit;
    double zCoordinate = -3;
    if (levelOfGame < 1) {
      zCoordinate = -3;
    } else if (levelOfGame <= 2) {
      zCoordinate = i % 2 == 0 ? -3 : -1;
    } else {
      zCoordinate = i % 2 == 0 ? -3 : i % 3 == 0 ? -1 : -4.5;
    }
    opponents.push_back(
        Spaceship(true, xCoordinate, 0, zCoordinate, 0, 0, 0, 0));
  }
  return opponents;
}

void generateToken(int val) {
  int sign = rand() % 2;
  if (!gameOver) {
    int tokenType = rand() % 4;
    float xCoordinate = rand() % 4;
    Token token(true, tokenType, (sign == 0) ? xCoordinate : -xCoordinate, 0,
                -7);
    tokens.push_back(token);
    drawToken(token);
    glutPostRedisplay();
    glutTimerFunc(20000, generateToken, 0);
  }
}
// MAIN

int main(int argc, char **argv) {
  // Opponents Initilization
  opponents = initializeOpponents(opponentsCount, level);

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
  glutTimerFunc(10000, generateToken, 0);
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
