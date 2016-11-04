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

#define true    1
#define false   0
#define RADIANS 0.0174533

typedef struct {
  float x;
  float y;
  float z;
} Point;

typedef struct {
  float* yValues;
  int depth;
  int size;
  int pointCount;
  float yRange;
  float deviance;
} Fractal;

typedef struct {
  Point position;
  Point focus;
  float radius;
  float latitude;
  float longitude;
  int lastMouseX;
  int lastMouseY;
  int enableMouseMotion;
} Camera;

// functions
double randomNumber(double min, double max);
float average(initializer_list<float> values);
float getY(int x, int z);
void setY(int x, int z, float value);
void drawText(int lineNo, string text, float textOffset, float value, float valueOffset);
void drawHelpInfo();
void initFractal(int depth, float yRange, float deviance);
void createFractal();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void updateCamera();
void display();
void reshape();
void getScreenDimensions();
void initGraphics(int argc, char* argv[]);
void initCamera();
int main(int argc, char* argv[]);
