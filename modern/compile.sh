#!/bin/bash
if [ -z "$1" ]; then
  file=fractal.cpp
else
  file=$1
fi

program=${file%.*}
g++ -std=c++11 -Wall -O3 -framework OpenGL -I/usr/local/include -L/usr/local/lib -lglfw3 -lGLEW $file -o $program