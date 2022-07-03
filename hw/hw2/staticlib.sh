#First,generate test.o from test.c
gcc -Wpedantic -Wall -Wextra -Werror -std=c89  -c test.c -o test.o 
#Next,generate ringbuffer.o from ringbuffer.c
gcc -Wpedantic -Wall -Wextra -Werror -std=c89  -c ringbuffer.c -o ringbuffer.o
#generate libringbuffer.a from ringbuffer.o
ar -r libringbuffer.a ringbuffer.o
#link them together
ld -o staticringbuffer test.o libringbuffer.a -lc 