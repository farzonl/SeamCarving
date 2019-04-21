#include "SeamCarving.hpp"
#include <iostream>

void arg3Behavior(int argc, char *argv[], int n=100) {
    if(strcmp(argv[2],"-v") == 0) { 
        SeamCarvingVertical sm(argv[1],n);
        sm.showImage();
    }
    else if(strcmp(argv[2],"-h") == 0) { 
        SeamCarvingHorizontal sm(argv[1],n);
        sm.showImage();
    }
    else {
         std::cout << "usage ./exec <image>.png -h|-v" << std::endl;
    }
}

void arg6Behavior(int argc, char *argv[]) {

    int verticle = 0;
    int grow = 0;
    int numberSeams = 0;

    char* imgPath = argv[1];
    std::cout << imgPath << std::endl;
    for (int i = 2; i < argc; i++) {
        char* curParam = argv[i];
        
        if( strcmp(argv[i],"-h") == 0 ) {
            verticle = false;
        } 
        else if ( strcmp(argv[i],"-v") == 0 ) {
            verticle = true;
        }
        else if ( strcmp(argv[i],"-g") == 0 ) {
            grow = true;
        }
        else if ( strcmp(argv[i],"-r") == 0 ) {
            grow = false;
        }
        else if ( strcmp(argv[i],"-n") == 0 ) {
            int n = atoi(argv[++i]);
            if(n != 0) {
                numberSeams = n;
            } else {
                std::cout << "error: value after -n is not a number" << std::endl;
                std::cout << "usage ./exec <image>.png -h|-v -n <number>" << std::endl;
            }
        }
        else {
            std::cout << "error: unrecognized parameter" << std::endl;
            std::cout << "usage ./exec <image>.png -h|-v -r|-g -n <number>" << std::endl;
        }
    }

    if ( verticle  ) {
        SeamCarvingVertical sm(imgPath, numberSeams, grow);
        sm.showImage();
    }
    else {
        SeamCarvingHorizontal sm(imgPath, numberSeams, grow);
        sm.showImage();
    }

}

int main(int argc, char *argv[]) {

    if(argc == 6) {
        arg6Behavior(argc, argv);
    }
    else if( argc == 5) {
        if(strcmp(argv[3],"-n") == 0) {
         int n = atoi(argv[4]);
         if(n != 0) {
            arg3Behavior(argc,argv,n);
         } else {
             std::cout << "error: value after -n is not a number" << std::endl;
             std::cout << "usage ./exec <image>.png -h|-v -n <number>" << std::endl;
         }
        }
        else {
            std::cout << "usage ./exec <image>.png -h|-v -n <number>" << std::endl;
        }
    }
    else if( argc == 3) {
        arg3Behavior(argc,argv);
    }
    else if( argc == 2)
    {
        SeamCarvingHorizontal sm(argv[1]);
        sm.showImage();
    } else {
        std::cout << "usage ./exec <image>.png" << std::endl;
    }
    return 0;
}
