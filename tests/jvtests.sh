#!bin/bash

gcc jvsec.c -o jvsec
gcc -fopenmp jvomp.c -o jvomp

time ./jvsec && time ./jvomp

