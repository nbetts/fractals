#!/bin/bash
file=$1
program=${file%.*}

if [[ "$OSTYPE" == "linux-gnu" ]]; then
  g++ -std=c++11 -Wall -Wno-deprecated -O3 -fomit-frame-pointer -pipe -DFX -DXMESA -lGL -lGLU -lglut -lX11 -lm -g $file -o $program
elif [[ "$OSTYPE" == "darwin"* ]]; then
  g++ -std=c++11 -Wall -Wno-deprecated -O3 -framework GLUT -framework OpenGL -I/usr/X11/include -L/usr/X11/lib -lX11 $file -o $program
else
  echo "OS not supported"
fi