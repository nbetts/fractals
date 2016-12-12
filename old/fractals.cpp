/**
 * [program description here]
 */

#include "fractals.h"

// Screen info
int screenWidth;
int screenHeight;
int windowWidth;
int windowHeight;

// Text info
stringstream stringStream;

// Fractal info
const int defaultDepth = 6;
const float defaultYRange = 0.2;
const float defaultDeviance = 0.5;
const float yRangeIncrement = 0.02;
const float dFactorIncrement = 0.02;
float sumYValues;
Fractal fractal;

// Camera info
const float defaultRadius = 5;
const float defaultLatitude = 30;
const float defaultLongitude = -30;
const float defaultFov = 10;
const float defaultFovIncrement = 1;
float fovIncrement;
Camera camera;

int isHelpDisplayed = true;

/**
 * Return a random real number in the given range.
 */
double randomNumber(double min, double max)
{
  return min + rand() / (RAND_MAX / (max - min) + 1.0);
}

float average(initializer_list<float> values)
{
  float sum = 0;
  int count = 0;

  for(auto value : values) {
    sum += value;
    count++;
  }

  return sum / (float)count;
}

float getY(int x, int z)
{
  int pos = (x & (fractal.size - 1)) + (z & (fractal.size - 1)) * fractal.size;
  return fractal.yValues[pos];
}

void setY(int x, int z, float value)
{
  int pos = (x & (fractal.size - 1)) + (z & (fractal.size - 1)) * fractal.size;
  fractal.yValues[pos] = value;
  sumYValues += value;
}

/**
 * Draw text onto the screen.
 */
void drawText(string text, int widthOffset, int heightOffset)
{
  glRasterPos2f(widthOffset, windowHeight - heightOffset);

  unsigned int i;
  for (i = 0; i < text.length(); i++) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
  }
}

/**
 * Draw the help info at the bottom of the window.
 */
void drawHelpInfo()
{
  // draw the interface
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);

  glColor3f(0.8, 0.8, 0);
  drawText("Y range = ", 10, 20);
  drawText("deviance = ", 10, 40);
  drawText("tree depth = ", 10, 60);
  drawText(to_string(fractal.yRange), 89, 20);
  drawText(to_string(fractal.deviance), 97, 40);
  drawText(to_string(fractal.depth), 113, 60);

  glColor3f(0.5, 0.5, 0.5);
  if (isHelpDisplayed) {
    drawText("H: toggle help info", 10, windowHeight - 115);
    drawText("R: reset values", 10, windowHeight - 100);
    drawText("S,W: -/+ Y range", 10, windowHeight - 85);
    drawText("A,D: -/+ deviance", 10, windowHeight - 70);
    drawText("Q,E: -/+ tree depth", 10, windowHeight - 55);
    drawText("-,=: -/+ zoom", 10, windowHeight - 40);
    drawText("Spacebar: regenerate fractal", 10, windowHeight - 25);
    drawText("Mouse: left click and drag to rotate the fractal", 10, windowHeight - 10);
  } else {
    drawText("H: toggle help info", 10, windowHeight - 10);
  }
}

/**
 * Initialise the fractal with the given properties.
 */
void initFractal(int depth, float yRange, float deviance)
{
  fractal.depth = depth;
  fractal.size = pow(2, depth);
  fractal.pointCount = pow(fractal.size, 2);
  fractal.yRange = yRange;
  fractal.deviance = deviance;
  fractal.yValues = new float[fractal.pointCount];
}

/**
 * Recursively update the points in the fractal using the midpoint displacement
 * algorithm.
 */
void createFractal()
{
  sumYValues = 0;
  int fractalSize = fractal.size;
  float yRange = fractal.yRange;

  while (fractalSize > 1) {
    int halfStep = fractalSize / 2;

    for (int y = halfStep; y < fractal.size + halfStep; y += fractalSize) {
      for (int x = halfStep; x < fractal.size + halfStep; x += fractalSize) {
        int hs = fractalSize / 2;
        float a = getY(x - hs, y - hs);
        float b = getY(x + hs, y - hs);
        float c = getY(x - hs, y + hs);
        float d = getY(x + hs, y + hs);
        setY(x, y, average({a, b, c, d}) + randomNumber(-yRange, yRange));
      }
    }

    for (int y = 0; y < fractal.size; y += fractalSize) {
      for (int x = 0; x < fractal.size; x += fractalSize) {
        int hs = fractalSize / 2;
        float newX = x + halfStep;
        float a = getY(newX - hs, y);
        float b = getY(newX + hs, y);
        float c = getY(newX, y - hs);
        float d = getY(newX, y + hs);
        setY(newX, y, average({a, b, c, d}) + randomNumber(-yRange, yRange));
   
        float newY = y + halfStep;
        a = getY(x - hs, newY);
        b = getY(x + hs, newY);
        c = getY(x, newY - hs);
        d = getY(x, newY + hs);
        setY(x, newY, average({a, b, c, d}) + randomNumber(-yRange, yRange));
      }
  }

    fractalSize /= 2;
    yRange *= fractal.deviance;
  }

  // reset the camera's focal point.
  camera.focus.y = 0;
}

/**
 * Allow keyboard input to directly modify the program whilst it is running.
 */
void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case 27: // Escape; exit the program
      exit(0);
      break;
    case ' ': // Spacebar; hold down for cool visual effects
      break;
    case 's':
      if (fractal.yRange - yRangeIncrement >= 0) {
        fractal.yRange -= yRangeIncrement;
      }
      break;
    case 'w':
      fractal.yRange += yRangeIncrement;
      break;
    case 'a':
      if (fractal.deviance - dFactorIncrement >= 0) {
        fractal.deviance -= dFactorIncrement;
      }
      break;
    case 'd':
      fractal.deviance += dFactorIncrement;
      break;
    case 'q':
      if (fractal.depth - 1 > 0) {
        fractal.depth--;
        initFractal(fractal.depth, fractal.yRange, fractal.deviance);
      }
      break;
    case 'e':
      fractal.depth++;
      initFractal(fractal.depth, fractal.yRange, fractal.deviance);
      break;
    case 'r':
      initFractal(defaultDepth, defaultYRange, defaultDeviance);
      camera.fov = defaultFov;
      fovIncrement = defaultFovIncrement;
      break;
    case '-':
      // camera.focus.y += 0.01;
      camera.fov += fovIncrement;
      fovIncrement += 0.05;
      glutPostRedisplay();
      return;
    case '=':
      // camera.focus.y -= 0.01;
      camera.fov -= fovIncrement;
      fovIncrement -= 0.05;
      glutPostRedisplay();
      return;
    case 'h':
      isHelpDisplayed = !isHelpDisplayed;
      glutPostRedisplay();
      return;
    default:
      return;
  }

  createFractal();
  glutPostRedisplay();
}

/**
 * Capture mouse input.
 */
void mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    camera.lastMouseX = x;
    camera.lastMouseY = y;
    camera.enableMouseMotion = true;
  } else {
    camera.enableMouseMotion = false;
  }
}

/**
 * Capture the motion of the mouse.
 */
void mouseMotion(int x, int y)
{
  if (camera.enableMouseMotion) {
    camera.latitude += x - camera.lastMouseX;
    camera.lastMouseX = x;
    camera.longitude -= y - camera.lastMouseY;
    camera.lastMouseY = y;

    // prevent the longitude from rotating the model upside-down.
    if (camera.longitude >= 90) {
      camera.longitude = 89;
    } else if (camera.longitude <= -90) {
      camera.longitude = -89;
    }

    glutPostRedisplay();
  }
}

/**
 * Update various properties of the camera and the project matrix.
 */
void updateCamera()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(camera.fov, (GLfloat)screenWidth / (GLfloat)screenHeight,
                 0.1, 20.0);

  int focusY = camera.focus.y + (sumYValues / fractal.pointCount);
  gluLookAt(camera.radius, 0, 0,
            camera.focus.x, focusY, camera.focus.z,
            0, 1, 0);
}

/**
 * Update the display on screen.
 */
void display()
{
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT);

  // position and orient the camera
  updateCamera();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotatef(camera.longitude, 0, 0, 1);
  glRotatef(camera.latitude, 0, 1, 0);
  glTranslatef(-0.5, 0, -0.5);

  // draw the fractal
  int i, j;
  float yDifference = fractal.yRange * 2;

  for (i = 0; i < fractal.size - 1; i++) {
    glBegin(GL_LINE_STRIP);
    for (j = 0; j < fractal.size; j++) {
      float x = (float)i / fractal.size;
      float y = getY(i, j);
      float z = (float)j / fractal.size;

      glColor3f(0, 1, (y / yDifference) + 0.5);
      glVertex3f(x, y, z);

      x = (float)(i + 1) / fractal.size;
      y = getY(i + 1, j);

      glColor3f(0, 1, (y / yDifference) + 0.5);
      glVertex3f(x, y, z);
    }
    glEnd();
  }

  for (i = 0; i < fractal.size; i++) {
    glBegin(GL_LINES);
    for (j = 0; j < fractal.size - 1; j++) {
      float x = (float)i / fractal.size;
      float y = getY(i, j);
      float z = (float)j / fractal.size;

      glColor3f(0, 1, (y / yDifference) + 0.5);
      glVertex3f(x, y, z);

      y = getY(i, j + 1);
      z = (float)(j + 1) / fractal.size;

      glColor3f(0, 1, (y / yDifference) + 0.5);
      glVertex3f(x, y, z);
    }
    glEnd();
  }

  drawHelpInfo();
  glutSwapBuffers();
}

/**
 * Update the display if the window is resized.
 */
void reshape(int width, int height)
{
  windowWidth = width;
  windowHeight = height;

  glClearColor(0, 0, 0, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

/**
 * Get the dimensions of the screen and update the screen width/height
 * variables.
 *
 * This solution originates from Stack Overflow, specifically here:
 * http://stackoverflow.com/a/25875849
 *
 * The answer to the posted question was provided by the following author:
 * http://stackoverflow.com/users/2858398/thomas-leclercq
 *
 * The code is licensed under CC-BY-SA:
 * https://creativecommons.org/licenses/by-sa/3.0/
 */
void getScreenDimensions()
{
  Display* d = XOpenDisplay(NULL);
  Screen* screen = DefaultScreenOfDisplay(d);
  screenWidth = screen->width;
  screenHeight = screen->height;
}

/**
 * Initiate the graphics environment.
 */
void initGraphics(int argc, char* argv[])
{
  glutInit(&argc, argv);
  getScreenDimensions();

  // Create the window.
  windowWidth = 900;
  windowHeight = 550;
  glutInitWindowPosition((screenWidth - windowWidth) / 2,
                         (screenHeight - windowHeight) / 2);
  glutInitWindowSize(windowWidth, windowHeight);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Midpoint Displacement Algorithm Demo in 3D");

  // Set blending.
  // glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  // Set various interface functions.
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);

  initCamera();
  initFractal(defaultDepth, defaultYRange, defaultDeviance);
  createFractal();
}

/**
 * Initialise the camera with default values.
 */
void initCamera()
{
  Point position = {0, 0, 0};
  Point focusPoint = {0, 0, 0};
  camera.position = position;
  camera.focus = focusPoint;
  camera.fov = defaultFov;
  fovIncrement = defaultFovIncrement;
  camera.radius = defaultRadius;
  camera.latitude = defaultLatitude;
  camera.longitude = defaultLongitude;
  camera.lastMouseX = 0;
  camera.lastMouseY = 0;
  camera.enableMouseMotion = false;
}

/**
 * Initiate the application.
 */
int main(int argc, char* argv[])
{
  srand(time(NULL));
  initGraphics(argc, argv);
  glutMainLoop();

  return 0;
}
