#include "SeamCarving.hpp"
#include <iostream>

int main(int argc, char *argv[]) {

    if( argc == 2)
    {
        SeamCarvingVertical sm(argv[1]);
        sm.showImage();
    } else {
        std::cout << "usage ./exec <image>.png" << std::endl;
    }
    return 0;
}
