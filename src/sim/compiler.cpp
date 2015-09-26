#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/program_options.hpp>

#include "parse_driver.h"
#include "sim/compile.h"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  using namespace std;

  try {
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "print usage info")
      ("output,o", po::value<std::string>(),
       "select output filename")
      ("file,f", po::value<std::string>(),
       "select source filename")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if( vm.count("help") ) {
      cout << desc << endl;
      return 0;
    }

    if( vm.count("file") ) {
      string outfn = "a.out";
      auto fn = vm["file"].as<string>();

      if( vm.count("output") )
        outfn = vm["output"].as<string>();

      cout << "compiling '" << fn 
        << "' -> '" << outfn
        << "'..." << endl;

      Parse_driver driver;
      if( driver.parse(fn) )
        throw std::runtime_error("parse failed");

      auto compiled = sim::compile(driver.ast_root());
    }


  } catch( std::runtime_error const& err ) {
    cerr << "Encountered runtime error: " << err.what() << endl;
    return 1;
  }

  return 0;
}
