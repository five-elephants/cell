#include <iostream>

#include "parse_driver.h"

int main(int argc, char* argv[]) {
	using namespace std;

	Parse_driver driver;

	for(int i=1; i<argc; i++) {
		cout << "Parsing " << argv[i] << ":\n";
		driver.parse(argv[i]);
		driver.ast_root().visit();
		cout << endl;
	}	

	return 0;
}
