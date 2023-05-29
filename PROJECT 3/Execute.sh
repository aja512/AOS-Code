#!/bin/bash

rm $2
gcc -o main *.c -pthread
./main $1 > $2
