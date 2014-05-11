#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/program_options.hpp>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/TargetSelect.h>

#include "parse_driver.h"
#include "sim/compile.h"
#include "ir/find_hierarchy.h"

namespace po = boost::program_options;


static llvm::ExecutionEngine* exe;

void simulate(std::shared_ptr<sim::Llvm_codegen> code,
    std::shared_ptr<ir::Module> toplevel) {
  using namespace std;

  cout << "starting simulation..." << endl;

  // generate wrapper functions to setup, run and teardown the simulation
  code->create_setup(toplevel);
  code->emit();

  auto setup_func = code->module()->getFunction("setup");
  if( !setup_func ) {
    throw std::runtime_error("failed to find function setup()");
  }

  void* ptr = exe->getPointerToFunction(setup_func);
  void(*func)() = (void(*)())(ptr);
  func();
  //cout << "__init__: " << func() << endl;
}

int main(int argc, char* argv[]) {
  using namespace std;
  using namespace llvm;

  InitializeNativeTarget();

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

      Parse_driver driver;
      if( driver.parse(fn) )
        throw std::runtime_error("parse failed");

      ir::Namespace topns;
      std::shared_ptr<sim::Llvm_codegen> code;
      std::tie(topns, code) = sim::compile(driver.ast_root());

      //auto toplevel = find_module(topns, vm["top"].as<string>());
      auto toplevel = find_by_path(topns, &ir::Namespace::modules, vm["top"].as<string>());
      if( !toplevel ) {
        cerr << "Can not find top level module '"
          << vm["top"].as<string>()
          << "'\n";
        cerr << "The following modules were found in toplevel namespace '"
          << topns.name 
          << "':\n";
        for(auto m : topns.modules) {
          cerr << "    " << m.first << '\n';
        }
        return 1;
      }

      {
        EngineBuilder exe_bld(code->module().get());
        std::string err_str;
        exe_bld.setErrorStr(&err_str);
        exe_bld.setEngineKind(EngineKind::JIT);
        exe = exe_bld.create();
        if( !exe ) {
          cerr << "Failed to create execution engine!" << endl;
          cerr << err_str << endl;
          return 1;
        }
      }

      simulate(code, toplevel);
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

