#include <iostream>
#include <stdexcept>

#include "parse_driver.h"

int main(int argc, char* argv[]) {
	using namespace std;

	try {
		Parse_driver driver;

		for(int i=1; i<argc; i++) {
			cout << "Parsing " << argv[i] << ":\n";
			if( driver.parse(argv[i]) ) {
				throw std::runtime_error("parse failed");
			}

			driver.ast_root().visit();
			cout << endl;
		}	
	} catch( std::runtime_error const& err ) {
		cerr << "Encountered runtime error: " << err.what() << endl;
		return 1;
	}

	return 0;
}
