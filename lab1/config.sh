gcc main.c node.c menu.c -o main -fsanitize=address -static-libasan -g
./main
