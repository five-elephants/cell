#include "sim/simulation_engine.h"
#include "sim/cpp_gen.h"
#include "logging/logger.h"
#include "ir/time.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sstream>
#include <fstream>

namespace po = boost::program_options;


void wrap(std::string const& sourcefile,
    std::string const& cpp_header,
    std::vector<std::string> const& lookup_path) {
  std::ofstream ofs(cpp_header);
  if( !ofs )
    throw std::runtime_error("Failed to open file '" + cpp_header + "'");

  ofs << "#pragma once\n\n";

  sim::Simulation_engine engine(sourcefile, lookup_path);
  auto lib = engine.library();

  for(auto m : lib->ns->modules) {
    engine.setup(m.first);
    auto insp = engine.inspect_module("");
    sim::write_cpp(ofs, insp.module()->socket);
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
      ("output,o", po::value<std::string>(),
       "output C++ header file")
      ("file,f", po::value<std::string>(),
       "input source file")
      ("lookup_path,L", po::value<std::vector<std::string>>(),
       "add a lookup path for namespace resolution (can be given multiple times)")
    ;
    po::positional_options_description pos_opts;
    pos_opts.add("file", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
        .options(desc)
        .positional(pos_opts)
        .run(),
        vm);
    po::notify(vm);

    if( vm.count("help")
        || !vm.count("file") ) {
      cout << "Usage: " << argv[0]
        << " [options] <source>\n"
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

    wrap(vm["file"].as<std::string>(),
        vm["output"].as<std::string>(),
        lookup_path);

  } catch( std::runtime_error const& err ) {
    cerr << "Encountered runtime error: " << err.what() << endl;
    return 1;
  }

  return 0;
}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */

