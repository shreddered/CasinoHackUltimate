#include <regex>
#include <iostream>

using namespace std;


int main(int argc, char** argv) {
    std::string s(argv[1]);
    std::regex r("([0-9]+)(\\()", std::regex::extended);
    std::cout << std::regex_replace(s, r, "\\1*\\2", std::regex_constants::format_sed) << std::endl;
	return 0;
}
