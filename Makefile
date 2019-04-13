#_____________G++ STUFF__________________________________________


OPTIONS =  -std=c++11 

CFLAGS = $(INCPATH) $(LIBPATH) $(OPTIONS)
UNAME := $(shell uname)
EXE     := $(UNAME)_SeamCarving
#_____________G++________________________________________________
ifeq ($(UNAME),Darwin)
CC = clang++
OPTIONS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
INCPATH = -I/usr/local/Cellar/opencv/4.0.1/include/opencv4/
LIBPATH = -L/usr/local/Cellar/opencv/4.0.1/lib
else
CC = g++
#OPTIONS +=  $(pkg-config --libs-only-l opencv)
#INCPATH = $(pkg-config --cflags opencv)
#LIBPATH = $(pkg-config --libs-only-L opencv)
INCPATH = -I/usr/include/opencv
OPTIONS += -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_datasets -lopencv_dpm -lopencv_face -lopencv_freetype -lopencv_fuzzy -lopencv_hdf -lopencv_line_descriptor -lopencv_optflow -lopencv_video -lopencv_plot -lopencv_reg -lopencv_saliency -lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_rgbd -lopencv_viz -lopencv_surface_matching -lopencv_text -lopencv_ximgproc -lopencv_calib3d -lopencv_features2d -lopencv_flann -lopencv_xobjdetect -lopencv_objdetect -lopencv_ml -lopencv_xphoto -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_photo -lopencv_imgproc -lopencv_core
LIBPATH =  -L/usr/lib/x86_64-linux-gnu
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

