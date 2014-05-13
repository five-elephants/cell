#include <iostream>


extern "C"
int print(char* msg) {
	std::cout << msg << std::endl;
	return 0;
}
