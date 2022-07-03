#include<stdio.h>
#include<stdint.h>
#include<time.h>
#define print(TYPE) (printf("size of %s: %d\n",#TYPE,((int)(sizeof(TYPE)))))



int main(){
    print(char);
    print(int);
    print(short);
    print(short int);
    print(long int);
    print(unsigned int);
    print(void*);
    print(size_t);
    print(float);
    print(double);
    print(int8_t);
    print(int16_t);
    print(int32_t);
    print(int64_t);
    print(time_t);
    print(clock_t);
    print(struct tm);
    print(NULL);
   /* printf("size of char: %ld\n",sizeof(char));
    printf("size of int: %ld\n",sizeof(int));
    printf("size of short: %ld\n",sizeof(short));
    printf("sizeof short int: %ld\n",sizeof(short int));
    printf("size of long int:%ld\n",sizeof(long int));
    printf("size of unsigned int: %ld\n",sizeof(unsigned int));
    printf("size of void*: %ld\n",sizeof(void*));
    printf("size of size_t: %ld\n",sizeof(size_t));
    printf("size of float: %ld\n",sizeof(float));
    printf("size of double: %ld\n",sizeof(double));
    printf("size of int8_t: %ld\n",sizeof(int8_t));
    printf("size of int16_t: %ld\n",sizeof(int16_t));
    printf("size of int32_t: %ld\n",sizeof(int32_t));
    printf("size of int64_t: %ld\n",sizeof(int64_t));
    printf("size of time_t: %ld\n",sizeof(time_t));
    printf("size of clock_t: %ld\n",sizeof(clock_t));
    printf("size of struct tm: %ld\n",sizeof(struct tm));
    printf("size of NULL: %ld\n",sizeof(NULL));*/
    return 0;
}