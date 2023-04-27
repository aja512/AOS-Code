#!/bin/bash

rm results.txt
gcc -o main *.c
./main > results.txt
