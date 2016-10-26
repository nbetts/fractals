// libraries
#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif
#include <math.h>
#include <float.h>
#include <sstream>
#include <X11/Xlib.h>
#include <string>
using namespace std;

typedef struct {
  float x;
  float y;
  float z;
} Point;

typedef struct {
  Point** points;
  int size;
  int pointCount;
  int depth;
  float yRange;
  float deviance;
} Fractal;

// Screen dimensions
int screenWidth;
int screenHeight;
int windowWidth;
int windowHeight;

// Text
stringstream stringStream;

// Fractal info
Fractal fractal;
int defaultTreeDepth = 3;
float defaultYRange = 0.4;
float defaultDeviance = 0.5;
int currentFractalCount;
int currentTreeDepth;
float currentYRange;
float currentDeviance;
float yRangeIncrement = 0.1;
float dFactorIncrement = 0.05;


// functions
double randomNumber(double min, double max);
void drawText(int lineNo, string text, float textOffset, float value, float valueOffset);
void drawHelpInfo();
void keyboard(unsigned char key, int x, int y);
void updateFractal(float yRange, float dFactor);
void updateYRange(float range);
void updateDeviance(float dFactor);
void initFractal(int treeDepth, float yRange, float deviance);
void createFractal(int size, float yRange);
float average(initializer_list<float> values);
void display();
void reshape();
void getScreenDimensions();
void initGraphics(int argc, char* argv[]);
int main(int argc, char* argv[]);



/**
 * Return a random real number in the given range.
 */
double randomNumber(double min, double max)
{
  return min + rand() / (RAND_MAX / (max - min) + 1.0);
}

/**
 * Draw text onto the screen.
 */
void drawText(int lineNo, string text, float textOffset, float value, float valueOffset)
{
  glRasterPos2f(10, windowHeight - (lineNo * 100) - textOffset);

  unsigned int i;
  for (i = 0; i < text.length(); i++) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
  }

  stringStream << value;
  string val(stringStream.str());
  glRasterPos2f(valueOffset, windowHeight - (lineNo * 100) - textOffset);

  for (i = 0; i < val.length(); i++) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, val[i]);
  }

  stringStream.str("");
}

/**
 * Draw the help info at the bottom of the window.
 */
void drawHelpInfo()
{
  glColor3f(0.5, 0.5, 0.5);
  drawText(0, "Keyboard: S/W = -/+ Y range, A/D = -/+ deviance, Q/E = -/+ tree depth, R = reset values     Tree depth = ", windowHeight - 10, currentTreeDepth, 850);
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
      if (currentYRange - yRangeIncrement >= 0) {
        currentYRange -= yRangeIncrement;
      }
      break;
    case 'w':
      currentYRange += yRangeIncrement;
      break;
    case 'a':
      if (currentDeviance - dFactorIncrement >= 0) {
        currentDeviance -= dFactorIncrement;
      }
      break;
    case 'd':
      currentDeviance += dFactorIncrement;
      break;
    case 'q':
      if (currentTreeDepth - 1 >= 0) {
        currentTreeDepth--;
        initFractal(currentTreeDepth, currentYRange, currentDeviance);
      }
      break;
    case 'e':
      currentTreeDepth++;
      initFractal(currentTreeDepth, currentYRange, currentDeviance);
      break;
    case 'r':
      currentTreeDepth = defaultTreeDepth;
      currentYRange = defaultYRange;
      currentDeviance = defaultDeviance;
      initFractal(defaultTreeDepth, defaultYRange, defaultDeviance);
      break;
    default:
      return;
  }

  updateFractal(currentYRange, currentDeviance);
  glutPostRedisplay();
}

/**
 * Update the fractal Y-range, deviance and point location.
 */
void updateFractal(float yRange, float deviance)
{
  updateYRange(yRange);
  updateDeviance(deviance);
  // createFractal(fractal.depth, 0, fractal.size - 1,
  //               0, fractal.size - 1, fractal.yRange);
  createFractal(fractal.size, fractal.yRange);
}

/**
 * Update the Y-range of the fractal using the following function:
 * yR = (n + 1) * baseRange
 */
void updateYRange(float baseRange)
{
  currentYRange = baseRange;
  fractal.yRange = baseRange;
}

/**
 * Update the deviances of the fractal using the following function:
 * d = (n * 0.05) + baseDeviance
 */
void updateDeviance(float baseDeviance)
{
  currentDeviance = baseDeviance;
  fractal.deviance = baseDeviance;
}

/**
 * Initialise the fractal with the given properties.
 */
void initFractal(int treeDepth, float yRange, float deviance)
{
  currentTreeDepth = treeDepth;
  currentYRange = yRange;
  currentDeviance = deviance;
  int pointCount2d = pow(2, treeDepth) + 1;
  int pointCount3d = pow(pointCount2d, 2);

  Point** points = new Point*[pointCount2d];
  float segmentLength = 1.0 / (float)pointCount2d;
  int i, j;
  for (i = 0; i < pointCount2d; i++) {
    points[i] = new Point[pointCount2d];

    for (j = 0; j < pointCount2d; j++) {
      points[i][j].x = i * segmentLength;
      points[i][j].y = 0;
      points[i][j].z = j * segmentLength;
    }
  }
  
  fractal.size = pointCount2d;
  fractal.points = points;
  fractal.pointCount = pointCount3d;
  fractal.depth = treeDepth;
  fractal.yRange = yRange;
  fractal.deviance = deviance;
}

/**
 * Create the fractal by using the midpoint displacement algorithm in 3D.
 */
void createFractal(int size, float yRange)
{
  int halfSize = size / 2;

  if (size == fractal.size) {
    fractal.points[0][0].y = randomNumber(-yRange, yRange);
    fractal.points[0][size - 1].y = randomNumber(-yRange, yRange);
    fractal.points[size - 1][0].y = randomNumber(-yRange, yRange);
    fractal.points[size - 1][size - 1].y = randomNumber(-yRange, yRange);

    yRange *= fractal.deviance;
  } else if (size <= 0) {
    return;
  }

  printf("size=%d\n", size);

  int i, j, counter = 1;
  for (i = halfSize; i > 0; i /= 2) {
    for (j = 0; j < pow(counter, 2); j *= 2) {
      fractal.points[i][i].y = randomNumber(-yRange, yRange);
    }
  }
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

/**
 * Update the display on screen.
 */
void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // position and orient the camera
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(10, (GLfloat)screenWidth / (GLfloat)screenHeight,
                 0.1, 20.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(7, 2.5, 5,
            0, 0, 0,
            0, 1, 0);

  // draw the fractal
  float yDifference = fractal.yRange * 2;
  int i, j;
  for (i = 0; i < fractal.size - 1; i++) {
    glBegin(GL_LINE_STRIP);
    for (j = 0; j < fractal.size; j++) {
      glColor3f(0, 1, (fractal.points[i][j].y / yDifference) + 0.5);
      glVertex3f(fractal.points[i][j].x,
                 fractal.points[i][j].y,
                 fractal.points[i][j].z);

      glColor3f(0, 1, (fractal.points[i + 1][j].y / yDifference) + 0.5);
      glVertex3f(fractal.points[i + 1][j].x,
                 fractal.points[i + 1][j].y,
                 fractal.points[i + 1][j].z);
    }
    glEnd();
  }
  for (i = 0; i < fractal.size; i++) {
    glBegin(GL_LINES);
    for (j = 0; j < fractal.size - 1; j++) {
      glColor3f(0, 1, (fractal.points[i][j].y / yDifference) + 0.5);
      glVertex3f(fractal.points[i][j].x,
                 fractal.points[i][j].y,
                 fractal.points[i][j].z);

      glColor3f(0, 1, (fractal.points[i][j + 1].y / yDifference) + 0.5);
      glVertex3f(fractal.points[i][j + 1].x,
                 fractal.points[i][j + 1].y,
                 fractal.points[i][j + 1].z);
    }
    glEnd();
  }

  // draw the interface
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
  glColor3f(0.8, 0.8, 0);
  drawText(0, "Y range = ", 20, fractal.yRange, 89);
  drawText(0, "deviance = ", 40, fractal.deviance, 97);
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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  // Set various interface functions.
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);

  gluPerspective(80, (GLfloat)screenWidth / (GLfloat)screenHeight, 10.0, 5000.0);
}

/**
 * Initiate the application.
 */
int main(int argc, char* argv[])
{
  srand(time(NULL));
  initGraphics(argc, argv);

  initFractal(defaultTreeDepth, defaultYRange, defaultDeviance);
  updateFractal(currentYRange, currentDeviance);
  glutMainLoop();

  return 0;
}
