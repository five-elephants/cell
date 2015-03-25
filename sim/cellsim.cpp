#include "sim/simulation_engine.h"
#include "sim/vcd_instrumenter.h"
#include "logging/logger.h"

#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;


void simulate() {
  sim::Instrumented_simulation_engine engine("test/simulator_test/demo_lif.cell",
      "demo.lif_neuron");
  sim::Vcd_instrumenter instr("demos__lif_neuron.vcd");

  engine.instrument(instr);
  engine.setup();
  engine.simulate(ir::Time(1000, ir::Time::ms));
  engine.teardown();
}


int main(int argc, char* argv[]) {
  using namespace std;

  try {
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "print usage info")
      ("verbose,v", "more output")
      ("veryverbose,V", "even more output")
      ("vcd", po::value<std::string>()->default_value(""),
       "write VCD output to file")
      ("file,f", po::value<std::string>()->required(),
       "input source file")
      ("top_module,t", po::value<std::string>()->required(),
       "top level module for elaboration")
    ;
    po::positional_options_description pos_opts;
    pos_opts.add("file", 1);
    pos_opts.add("top_module", 1);

    po::variables_map vm;
    //po::store(po::parse_command_line(argc, argv, desc), vm);
    po::store(po::command_line_parser(argc, argv)
        .options(desc)
        .positional(pos_opts)
        .run(),
        vm);
    po::notify(vm);

    if( vm.count("help") ) {
      cout << desc << endl;
      return 0;
    }
  } catch( std::runtime_error const& err ) {
    cerr << "Encountered runtime error: " << err.what() << endl;
    return 1;
  }

  return 0;
}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
