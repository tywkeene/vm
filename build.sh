#!/bin/bash
gcc src/vm.c src/video.c -o vm -g -Wall -lncurses -I./include
