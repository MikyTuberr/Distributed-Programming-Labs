#!/bin/bash

# Ustawienie zmiennych ścieżki do katalogów
LIB_PATH="./lib"
BIN_PATH="./bin"
SRC_PATH="./src"

echo "Kompilowanie bibliotek statycznych..."
gcc -c $SRC_PATH/scalc1.c -o $BIN_PATH/scalc1.o
gcc -c $SRC_PATH/scalc2.c -o $BIN_PATH/scalc2.o
ar r $LIB_PATH/lib_scalc.a $BIN_PATH/scalc1.o $BIN_PATH/scalc2.o

echo "Kompilowanie bibliotek dynamicznych..."
gcc -c -Wall -fPIC -D_GNU_SOURCE $SRC_PATH/dcalc1.c -o $BIN_PATH/dcalc1.o
gcc -c -Wall -fPIC -D_GNU_SOURCE $SRC_PATH/dcalc2.c -o $BIN_PATH/dcalc2.o
gcc $BIN_PATH/dcalc1.o $BIN_PATH/dcalc2.o -shared -o $LIB_PATH/lib_dcalc.so

echo "Kompilowanie programu i linkowanie bibliotek..."
gcc -c $SRC_PATH/main.c -o $BIN_PATH/main.o
gcc -o main $BIN_PATH/main.o -L$LIB_PATH -l_dcalc -L$LIB_PATH -l_scalc
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$LIB_PATH"

if [ $? -eq 0 ]; then
  echo "Kompilacja zakończona sukcesem."
  ./main
else
  echo "Błąd podczas kompilacji."
fi

