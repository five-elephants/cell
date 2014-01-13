#include <iostream>
#include <stdexcept>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/export.hpp>

#include "parse_driver.h"
//#include "gen/gen_text.h"
//#include "gen/gen_cpp.h"
#include "gen/gen_m4.h"
#include "ir/serialization.hpp"
#include "ir/analyze.h"

BOOST_CLASS_EXPORT(ir::Type)
BOOST_CLASS_EXPORT(ir::Object)
BOOST_CLASS_EXPORT(ir::Port_assignment)
BOOST_CLASS_EXPORT(ir::Instantiation)
BOOST_CLASS_EXPORT(ir::Function)
BOOST_CLASS_EXPORT(ir::Namespace)
BOOST_CLASS_EXPORT(ir::Socket)
BOOST_CLASS_EXPORT(ir::Module)

namespace po = boost::program_options;

gen::Generator_if* make_generator(std::string const& type, std::ostream& outstream) {
  /*if( type == "txt" )
    return new gen::Text_generator(outstream);
  else if( type == "cpp" )
    return new gen::Cpp_generator(outstream);
	else*/ if( type == "m4" )
		return new gen::M4_generator(outstream);
  else
    throw std::runtime_error("Undefined generator type selected");
}

int main(int argc, char* argv[]) {
	using namespace std;

	try {
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "print usage info")
      ("gen,g", po::value<std::string>()->default_value("m4"),
       "select generator in {'txt', 'cpp', 'm4'}")
      ("file,f", po::value<std::string>(),
       "source file")
      ("analyze,a", "analyze source file and print result")
      ("ir,i", po::value<std::string>(),
       "output intermediate representation to file")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if( vm.count("help") ) {
      cout << desc << endl;
      return 0;
    }

		Parse_driver driver;
    
    //gen::Cpp_generator gen(cout);
    //gen::Text_generator gen(cout);
    std::unique_ptr<gen::Generator_if> gen(make_generator(vm["gen"].as<std::string>(), cout));   

		if(vm.count("file")) {
      auto fn = vm["file"].as<std::string>();

			//cout << "Parsing " << argv[i] << ":\n";
			if( driver.parse(fn) ) {
				throw std::runtime_error("parse failed");
			}

      if( vm.count("analyze") ) {
        auto defns = ir::analyze(driver.ast_root());
        cout << "List of modules:\n";
        for(auto m : defns.modules) {
          cout << "* " << m.first << ": " << m.second->name << '\n';
          cout << "  +-Objects:\n";
          for(auto o : m.second->objects) {
            cout << "    +-" << o.first << " : " << o.second->type->name << '\n';
          }
          cout << "  +-Functions:\n";
          for(auto f : m.second->functions) {
            cout << "    +-" << f.first << " : " << f.second->return_type->name << '\n';
          }
        }

        if( vm.count("ir") ) {
          ofstream ofs(vm["ir"].as<std::string>());
          boost::archive::xml_oarchive oar(ofs);
          oar << boost::serialization::make_nvp("top", defns);
        }
      } else {
        driver.ast_root().set_generator(*gen);
        driver.ast_root().visit();
        cout << endl;
      }
		} else {
      throw std::runtime_error("no input files specified");
    }
	} catch( std::runtime_error const& err ) {
		cerr << "Encountered runtime error: " << err.what() << endl;
		return 1;
	}

	return 0;
}
