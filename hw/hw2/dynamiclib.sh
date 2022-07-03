#First,generate test.o from test.c
gcc -Wpedantic -Wall -Wextra -Werror -std=c89  -c test.c -o test.o 
#gcc -Wpedantic -Wall -Wextra -Werror -std=c89  -c ringbuffer.c  -o ringbuffer.o 
#Next,geberate libringbuffer.so
gcc  -fPIC -shared -Wpedantic -Wall -Wextra -Werror -std=c89 -c ringbuffer.c -o libringbuffer.so
#create exe
gcc -o dynamicringbuffer test.o -L. -Bdynamic -lringbuffer