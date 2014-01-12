#include "api.h"

#include "ir/serialization.hpp"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/archive_exception.hpp>
#include <iostream>
#include <fstream>

void hello() {
  std::cout << "Hello World" << std::endl; 
}


ir::Namespace load(std::string const& filename) {
  try {
    ir::Namespace rv("default");
    std::ifstream ifs(filename.c_str());
    if( !ifs ) {
      std::cerr << "Failed to open '" << filename << "' for reading" << std::endl;
    } else {
      boost::archive::xml_iarchive iar(ifs);
      iar >> boost::serialization::make_nvp("top", rv);
    }
    return rv;
  } catch( boost::archive::archive_exception& err ) {
    std::cerr << "Encountered exception from boost::serialization:\n"
      << err.what()
      << std::endl;
    return ir::Namespace("default");
  }
}


void save(std::string const& filename, ir::Namespace& ns) {
  try {
    std::ofstream ofs(filename.c_str());
    if( !ofs ) {
      std::cerr << "Failed to open '" << filename << "' for writing" << std::endl;
    } else {
      boost::archive::xml_oarchive oar(ofs);
      oar << boost::serialization::make_nvp("top", ns);
    }
  } catch( boost::archive::archive_exception& err ) {
    std::cerr << "Encountered exception from boost::serialization:\n"
      << err.what()
      << std::endl;
  }
}
