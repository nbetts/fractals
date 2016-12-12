// libraries
#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif
#include <math.h>
#include <sstream>
#include <X11/Xlib.h>
#include <string>
using namespace std;

typedef struct {
  float x;
  float y;
} Point;

typedef struct {
  Point* points;
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
Fractal* fractals;
int defaultFractalCount = 5;
int defaultTreeDepth = 10;
float defaultYRange = 10;
float defaultDeviance = 0.5;
int currentFractalCount;
int currentTreeDepth;
float currentYRange;
float currentDeviance;
float yRangeIncrement = 2;
float dFactorIncrement = 0.05;


// functions
double randomNumber(double min, double max);
void drawText(int lineNo, string text, float textOffset, float value, float valueOffset);
void drawHelpInfo();
void keyboard(unsigned char key, int x, int y);
void updateFractals(float yRange, float dFactor);
void updateYRanges(float range);
void updateDeviances(float dFactor);
void initFractals(int fractalCount, int treeDepth, float yRange, float deviance);
void createFractal(int fractalNumber, int depth, int point1, int point2, float yRange);
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
        initFractals(currentFractalCount, currentTreeDepth,
                     currentYRange, currentDeviance);
      }
      break;
    case 'e':
      currentTreeDepth++;
      initFractals(currentFractalCount, currentTreeDepth,
                   currentYRange, currentDeviance);
      break;
    case 'r':
      currentFractalCount = defaultFractalCount;
      currentTreeDepth = defaultTreeDepth;
      currentYRange = defaultYRange;
      currentDeviance = defaultDeviance;
      initFractals(defaultFractalCount, defaultTreeDepth,
                   defaultYRange, defaultDeviance);
      break;
    default:
      return;
  }

  updateFractals(currentYRange, currentDeviance);
  glutPostRedisplay();
}

/**
 * Update the fractal Y-ranges, deviances and point locations.
 */
void updateFractals(float yRange, float deviance)
{
  updateYRanges(yRange);
  updateDeviances(deviance);

  int i;
  for (i = 0; i < currentFractalCount; i++) {
    createFractal(i, fractals[i].depth, 0,
                  fractals[i].pointCount - 1, fractals[i].yRange);
  }
}

/**
 * Update the Y-ranges of all the fractals using the following function:
 * yR = (n + 1) * baseRange
 */
void updateYRanges(float baseRange)
{
  currentYRange = baseRange;

  int i;
  for (i = 0; i < currentFractalCount; i++) {
    fractals[i].yRange = (i + 1) * baseRange;
  }
}

/**
 * Update the deviances of off al of the fractals using the following function:
 * d = (n * 0.05) + baseDeviance
 */
void updateDeviances(float baseDeviance)
{
  currentDeviance = baseDeviance;

  int i;
  for (i = 0; i < currentFractalCount; i++) {
    fractals[i].deviance = (i * 0.05) + baseDeviance;
  }
}

/**
 * Initialise the fractals with the given properties.
 */
void initFractals(int fractalCount, int treeDepth, float yRange, float deviance)
{
  currentFractalCount = fractalCount;
  currentTreeDepth = treeDepth;
  currentYRange = yRange;
  currentDeviance = deviance;
  int pointCount = pow(2, treeDepth) + 1;

  fractals = new Fractal[fractalCount];
  int i;
  for (i = 0; i < fractalCount; i++) {    
    Point* points = new Point[pointCount];
    points[0].x = 150.0;
    points[0].y = (fractalCount - i) * 100.0;
    points[pointCount - 1].x = windowWidth - 50.0;
    points[pointCount - 1].y = (fractalCount - i) * 100.0;
    
    fractals[i].points = points;
    fractals[i].pointCount = pointCount;
    fractals[i].depth = treeDepth;
    fractals[i].yRange = yRange;
    fractals[i].deviance = deviance;
  }
}

/**
 * Recursively update the points in the fractal using the midpoint displacement
 * algorithm.
 */
void createFractal(int fractalNumber, int depth, int startPoint, int endPoint, float yRange)
{
  if (depth == 0) {
    return;
  }

  int midpoint = (startPoint + endPoint) / 2;
  fractals[fractalNumber].points[midpoint].x = (fractals[fractalNumber].points[startPoint].x +
                                                fractals[fractalNumber].points[endPoint].x) / 2;
  fractals[fractalNumber].points[midpoint].y = (fractals[fractalNumber].points[startPoint].y +
                                                fractals[fractalNumber].points[endPoint].y) / 2;
  fractals[fractalNumber].points[midpoint].y += randomNumber(-yRange, yRange);

  yRange *= fractals[fractalNumber].deviance;
  createFractal(fractalNumber, depth - 1, startPoint, midpoint, yRange);
  createFractal(fractalNumber, depth - 1, midpoint, endPoint, yRange);
}

/**
 * Update the display on screen.
 */
void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);

  int i, j;
  for (i = 0; i < currentFractalCount; i++) {
    glColor3f(0.8, 0.8, 0);
    drawText(i, "Y range = ", 50, fractals[i].yRange, 89);
    drawText(i, "deviance = ", 70, fractals[i].deviance, 97);

    glColor3f(0, 1, (i * 0.2));
    glBegin(GL_LINE_STRIP);
    for (j = 0; j < fractals[i].pointCount; j++) {
      glVertex2f(fractals[i].points[j].x, fractals[i].points[j].y); 
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
  glutCreateWindow("Midpoint Displacement Algorithm Demo in 2D");

  // Set blending.
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  // Set various interface functions.
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);
}

/**
 * Initiate the application.
 */
int main(int argc, char* argv[])
{
  srand(time(NULL));
  initGraphics(argc, argv);

  initFractals(defaultFractalCount, defaultTreeDepth,
               defaultYRange, defaultDeviance);
  updateFractals(currentYRange, currentDeviance);
  glutMainLoop();

  return 0;
}
