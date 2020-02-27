#include "image.hpp"
#include <iostream>

using namespace std;


int main(int argc, char** argv) {
    std::cout << image::parse12lvl("image.jpg");
	return 0;
}
