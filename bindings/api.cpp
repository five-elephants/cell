#include "api.h"

#include "parse_driver.h"
#include "ir/serialization.hpp"
#include "ir/analyze.h"
#include "ir/find.hpp"
#include "ir/find_hierarchy.h"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/export.hpp>
#include <iostream>
#include <fstream>

BOOST_CLASS_EXPORT(ir::Type)
BOOST_CLASS_EXPORT(ir::Object)
BOOST_CLASS_EXPORT(ir::Port_assignment)
BOOST_CLASS_EXPORT(ir::Instantiation)
BOOST_CLASS_EXPORT(ir::Function)
BOOST_CLASS_EXPORT(ir::Namespace)
BOOST_CLASS_EXPORT(ir::Socket)
BOOST_CLASS_EXPORT(ir::Module)

void hello() {
  std::cout << "Hello World" << std::endl; 

  std::string path = "this.isa.test";
  auto path_elems = ir::parse_path(path, ".");

  std::cout << "PATH: " << path << "\n";
  for(auto i : path_elems) {
    std::cout << "   '" << i << "'\n";
  } 
}


ir::Namespace analyze(std::string const& filename) {
  try {
    Parse_driver parse_driver;

    if( parse_driver.parse(filename) )
      throw std::runtime_error("parse failed");

    auto rv = ir::analyze(parse_driver.ast_root());

    return rv;
  } catch( std::runtime_error const& err ) {
    std::cerr << "Encountered runtime error: " << err.what() << std::endl;
    return ir::Namespace();
  }
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



ir::Module find_module(ir::Namespace const& ns, ir::Label const& name) {
  auto rv = ir::find_module(ns, name); 
  if( rv ) 
    return *rv;
  else
    return ir::Module();
}
