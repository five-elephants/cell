#include <iostream>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <boost/program_options.hpp>
#include <llvm/Support/TargetSelect.h>

#include "sim/simulation_engine.h"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  using namespace std;
  using namespace llvm;

  InitializeNativeTarget();
  ir::Builtins::init();

  try {
    po::positional_options_description pd;
    pd.add("file", -1);

    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "print usage info")
      ("top,t", po::value<std::string>(),
       "Name of the top module")
      ("file,f", po::value<std::string>(),
       "select source filename")
    ;

    po::variables_map vm;
    //po::store(po::parse_command_line(argc, argv, desc), vm);
    po::store(po::command_line_parser(argc, argv)
        .options(desc)
        .positional(pd).run(),
        vm);
    po::notify(vm);

    if( vm.count("help") ) {
      cout << desc << endl;
      return 0;
    }

    if( vm.count("file") && vm.count("top") ) {
      string outfn = "a.out";
      auto fn = vm["file"].as<string>();

      sim::Simulation_engine engine(vm["file"].as<string>(),
          vm["top"].as<string>());
      engine.setup();
      engine.simulate_cycle();
      engine.teardown();
    } else {
      cout << desc << endl;
      return 1;
    }
  } catch( std::runtime_error const& err ) {
    cerr << "Encountered runtime error: " << err.what() << endl;
    return 1;
  }

  return 0;
}

