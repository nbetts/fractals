#!/bin/bash
file=main.cpp
program=${file%.*}

function compile {
  if [[ "$OSTYPE" == "linux"* ]]; then
    output="$((g++ -std=c++11 -Wall -Wno-deprecated -O3 -fomit-frame-pointer -pipe -DFX -DXMESA -lGL -lGLU -lglut -lX11 -lm -g $file -o $program) 2>&1)"
  elif [[ "$OSTYPE" == "darwin"* ]]; then
    output="$((g++ -std=c++11 -Wall -O3 -framework OpenGL -I/usr/local/include -L/usr/local/lib -lglfw3 -lGLEW $file -o $program) 2>&1)"
  else
    echo "OS not supported"
    exit -1
  fi
}

# check for flags
while getopts ":xr" opt; do
  case $opt in
    x) # immediately run the program
      ./$program "${@:2}" &
      exit 0
      ;;
    r) # compile then run the program if there are no compile errors/warnings
      compile
      if [[ -z "${output//$'[[:space:]]'/}" ]]
        then
          ./$program "${@:2}" &
        else
          echo "$output"
          exit -1
      fi
      exit 0
      ;;
  esac
done

# if no flags are present, compile the program.
compile
