#! /bin/sh
cc main.c -Wall -Wextra `pkg-config --libs --cflags raylib` -o Main
./Main
