#include "calc.hpp"
#include "image.hpp"
#include <iostream>

using namespace std;
using calculator::fraction;


int main() {
	//wcout << image::parse("image.jpg", "") << endl;
    auto f = fraction(-5, 4) + fraction(1, 4);
    cout << f * fraction(1, 2) << endl;
    cout << calculator::eval("2+2") << endl;
	return 0;
}
