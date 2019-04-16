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

int main(int argc, char *argv[]) {

    if( argc == 5) {
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
