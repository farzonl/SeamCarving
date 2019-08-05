#_____________STATIC STUFF__________________________________________
FILES := $(shell echo *.cpp  | sed -e 's/cpp/o/g')
UNAME := $(shell uname)
EXE   := $(UNAME)_SeamCarving
OPTIONS := -coverage
#_____________STATIC STUFF________________________________________________
ifeq ($(UNAME),Darwin)
CXX := clang++
OPTIONS +=  -std=c++11
opencvLib:= $(shell dirname $(shell brew ls opencv | grep .dylib  | head -1))
opencvInclude := $(shell dirname $(opencvLib))/include/opencv4/
INCPATH := -I$(opencvInclude)
LIBPATH := -L$(opencvLib) -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
else
CXX := g++
OPTIONS += -std=c++17 -lstdc++fs
INCPATH := `pkg-config opencv --cflags`
LIBPATH := `pkg-config opencv --libs`
endif
CFLAGS := $(INCPATH) $(LIBPATH) $(OPTIONS)

build: build-release

run : build-release
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

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

$(EXE) : $(FILES)
	$(CXX) $^ -o $(EXE) $(CFLAGS)

build-debug : CFLAGS += -g -DDEBUG
build-debug : $(EXE)

clean :
	rm -rf *.o* $(EXE)* *.gcov *.gcda *.gcno

rebuild : clean build-release

