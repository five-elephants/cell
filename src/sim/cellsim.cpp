#include "sim/simulation_engine.h"
#include "sim/vcd_instrumenter.h"
#include "sim/cpp_gen.h"
#include "logging/logger.h"
#include "ir/time.h"

#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sstream>
#include <fstream>

namespace po = boost::program_options;


void wrap_cpp(std::string const& cpp_header, sim::Simulation_engine& engine) {
  std::ofstream ofs(cpp_header);
  if( !ofs )
    throw std::runtime_error("Failed to open file '" + cpp_header + "'");

  ofs << "#pragma once\n\n";

  auto insp = engine.inspect_module("");
  sim::write_cpp(ofs, insp.module()->socket);
}


void simulate(std::string const& sourcefile,
    std::string const& top_module,
    std::string const& vcd_dump,
    std::string const& cpp_header,
    std::string const& time,
    std::vector<std::string> const& lookup_path) {
  ir::Time t;
  if( !time.empty() ) {
    std::stringstream strm(time);
    strm >> t;
  }

  if( !vcd_dump.empty() ) {
    sim::Instrumented_simulation_engine engine(sourcefile,
        top_module,
        lookup_path);
    sim::Vcd_instrumenter instr(vcd_dump);
    engine.instrument(instr);
    engine.setup();

    if( !cpp_header.empty() )
      wrap_cpp(cpp_header, engine);

    if( t != ir::Time(0, ir::Time::ns) )
      engine.simulate(t);
    engine.teardown();
  } else {
    sim::Simulation_engine engine(sourcefile, top_module, lookup_path);
    engine.setup();

    if( !cpp_header.empty() )
      wrap_cpp(cpp_header, engine);

    if( t != ir::Time(0, ir::Time::ns) )
      engine.simulate(t);
    engine.teardown();
  }
}


int main(int argc, char* argv[]) {
  using namespace std;
  namespace bf = boost::filesystem;

  try {
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "print usage info")
      ("verbose,v", "more output")
      ("veryverbose,V", "even more output")
      ("vcd", po::value<std::string>()->default_value(""),
       "write VCD output to file")
      ("wrap-cpp", po::value<std::string>()->default_value(""),
       "write C++ header to file")
      ("file,f", po::value<std::string>(),
       "input source file")
      ("top_module,m", po::value<std::string>(),
       "top level module for elaboration")
      ("time,t", po::value<std::string>()->default_value(""),
       "simulated duration of simulation")
      ("lookup_path,L", po::value<std::vector<std::string>>(),
       "add a lookup path for namespace resolution (can be given multiple times)")
    ;
    po::positional_options_description pos_opts;
    pos_opts.add("file", 1);
    pos_opts.add("top_module", 1);
    pos_opts.add("time", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
        .options(desc)
        .positional(pos_opts)
        .run(),
        vm);
    po::notify(vm);

    if( vm.count("help")
        || !vm.count("file")
        || !vm.count("top_module")
        || !vm.count("time") ) {
      cout << "Usage: " << argv[0]
        << " [options] <source> <top-level module> <simulation time>\n\n"
        << "   <top-level module> : e.g. my_namespace::module_name\n"
        << "   <simulation time>  : <integer> [ms|us|ns|ps]\n"
        << "\n"
        << desc << endl;
      return 0;
    }

    init_logging();

    auto logger = log4cxx::Logger::getRootLogger();
    if( vm.count("verbose") )
      logger->setLevel(log4cxx::Level::getDebug());
    else if( vm.count("veryverbose") )
      logger->setLevel(log4cxx::Level::getTrace());
    else
      logger->setLevel(log4cxx::Level::getInfo());


    std::vector<std::string> lookup_path;
    if( vm.count("lookup_path") )
      lookup_path = vm["lookup_path"].as<std::vector<std::string>>();

    simulate(vm["file"].as<std::string>(),
        vm["top_module"].as<std::string>(),
        vm["vcd"].as<std::string>(),
        vm["wrap-cpp"].as<std::string>(),
        vm["time"].as<std::string>(),
        lookup_path);

  } catch( std::runtime_error const& err ) {
    cerr << "Encountered runtime error: " << err.what() << endl;
    return 1;
  }

  return 0;
}

/* vim: set et ff=unix sts=2 sw=2 ts=2 : */
