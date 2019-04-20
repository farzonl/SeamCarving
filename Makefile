#_____________G++ STUFF__________________________________________


OPTIONS =  -std=c++11 
FILES := SeamCarving.o SeamCarvingHorizontal.o SeamCarvingVertical.o main.o

CFLAGS = $(INCPATH) $(LIBPATH) $(OPTIONS)
UNAME := $(shell uname)
EXE     := $(UNAME)_SeamCarving
#_____________G++________________________________________________
ifeq ($(UNAME),Darwin)
CC = clang++
INCPATH = -I/usr/local/Cellar/opencv/4.0.1/include/opencv4/
LIBPATH = -L/usr/local/Cellar/opencv/4.0.1/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
else
CC = g++
INCPATH = `pkg-config opencv --cflags`
LIBPATH = `pkg-config opencv --libs`
endif

#Small enough project so lets rebuild everytime
run : rebuild
	./$(EXE) img/sampleImg2.jpg


run-debug : build-debug
	./$(EXE) img/sampleImg1.jpg

run-gdb : build-debug
	gdb --args ./$(EXE) img/sampleImg1.jpg

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

build-release : $(EXE)

$(EXE) : $(FILES)
	$(CC) $^ -o $(EXE) $(CFLAGS)

build-debug : CFLAGS += -g -DDEBUG
build-debug : $(EXE)

clean :
	rm -rf *.o* $(EXE)*

rebuild : clean build-release

