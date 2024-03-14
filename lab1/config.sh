#!/bin/bash

SRC_PATH="./src" 

gcc $SRC_PATH/main.c $SRC_PATH/node.c $SRC_PATH/menu.c -o main -fsanitize=leak 
./main
