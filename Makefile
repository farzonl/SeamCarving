#_____________G++ STUFF__________________________________________

INCPATH = -I/usr/local/Cellar/opencv/4.0.1/include/opencv4/
LIBPATH = -L/usr/local/Cellar/opencv/4.0.1/lib
OPTIONS =  -std=c++11 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs

CFLAGS = $(INCPATH) $(LIBPATH) $(OPTIONS)
UNAME := $(shell uname)
EXE     := $(UNAME)_SeamCarving
#_____________G++________________________________________________
ifeq ($(UNAME),Darwin)
CC = clang++
else
CC = g++
endif


run : build-release
	./$(EXE) sampleImg1.jpg


run-debug : build-debug
	./$(EXE) sampleImg1.jpg

run-gdb : build-debug
	g<F12>b --args ./$(EXE) sampleImg1.jpg

edit0 : 
	nano -c main.cpp

edit1 : 
ifeq ($(UNAME), Linux)
	kate *.cpp *.hpp &
endif
ifeq ($(UNAME), Darwin)
	@echo 'N/A'
endif

run-valgrind : build-debug
	valgrind --leak-check=yes --show-reachable=yes --tool=memcheck ./EXE

build-release : CFLAGS += -O3

build-release : SeamCarving.o
	$(CC) $(CFLAGS) SeamCarving.o main.cpp -o $(EXE)

build-debug : CFLAGS += -g -DDEBUG
build-debug : SeamCarving.o
	$(CC) $(CFLAGS) SeamCarving.o main.cpp -o $(EXE)

SeamCarving.o : SeamCarving.cpp SeamCarving.hpp
		$(CC) $(CFLAGS) -c SeamCarving.cpp

clean :
	rm *.o* $(EXE)*

