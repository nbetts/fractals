#!/bin/bash
filepath=src/main.cpp
program=${filepath##src/}
output=${program%.*}

function compile {
  if [[ "$OSTYPE" == "linux"* ]]; then
    errs="$((g++ -std=c++11 -Wall -Wno-deprecated -O3 -fomit-frame-pointer -pipe -DFX -DXMESA -lGL -lGLU -lglut -lX11 -lm -g $filepath -o $output) 2>&1)"
  elif [[ "$OSTYPE" == "darwin"* ]]; then
    errs="$((g++ -std=c++11 -Wall -O3 -framework OpenGL -I/usr/local/include -L/usr/local/lib -lglfw3 -lGLEW $filepath -o $output) 2>&1)"
  else
    echo "OS not supported"
    exit -1
  fi
}

# check for flags
while getopts ":xr" opt; do
  case $opt in
    x) # immediately run the program
      ./$output "${@:2}" &
      exit 0
      ;;
    r) # compile then run the program if there are no compile errors/warnings
      compile
      if [[ -z "${errs//$'[[:space:]]'/}" ]]
        then
          ./$output "${@:2}" &
        else
          echo "$errs"
          exit -1
      fi
      exit 0
      ;;
  esac
done

# if no flags are present, compile the program.
compile
