#_____________STATIC STUFF__________________________________________
OPTIONS =  -std=c++17
FILES := SeamCarving.cpp SeamCarvingHorizontal.cpp SeamCarvingVertical.cpp main.cpp
UNAME := $(shell uname)
EXE   := $(UNAME)_SeamCarving
#_____________STATIC STUFF________________________________________________
ifeq ($(UNAME),Darwin)
#OPTIONS += -lc++fs
CC := /usr/local/Cellar/llvm/8.0.0/bin/clang++
INCPATH := -I/usr/local/Cellar/opencv/4.0.1/include/opencv4/
LIBPATH := -L/usr/local/Cellar/opencv/4.0.1/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
else
CC := g++-8
OPTIONS += -lstdc++fs
INCPATH := `pkg-config opencv --cflags`
LIBPATH := `pkg-config opencv --libs`
endif
CFLAGS := $(INCPATH) $(LIBPATH) $(OPTIONS)
#Small enough project so lets rebuild everytime
run : rebuild
	./$(EXE) img/sampleImg2.jpg -h -n 20


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

