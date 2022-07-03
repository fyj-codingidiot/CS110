CXX = g++
CXX_FLAGS = -Wpedantic -Wall -Wextra -std=c++14 -g
INC_FILES = ringbuffer.hpp ringbuffer.ipp
SRC_FILES = main.cpp

all: ${INC_FILES} ${SRC_FILES}
	${CXX} ${CXX_FLAGS} -o main ${SRC_FILES}

clean:
	rm -f main
submit:
	tar -cvf hw7.tar ringbuffer.ipp
.PHONY: clean