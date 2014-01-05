#include "namespace.h"

#include "ast/ast_find.h"
#include "builtins.h"

#include <stdexcept>
#include <sstream>
//--------------------------------------------------------------------------------

namespace ir {

  //--------------------------------------------------------------------------------
  Module::Module(ast::Module_def const& mod) {
    name = dynamic_cast<ast::Identifier const*>(&(mod.identifier()))->identifier();
  }

  void
  Module::scan_ast(ast::Node_if const& tree) {
    // find declared variables
    auto vars = ast::find_by_type<ast::Variable_def>(tree);
    for(auto i : vars) {
      std::shared_ptr<Object> obj(new Object);
      obj->name = dynamic_cast<ast::Identifier const*>(&(i->identifier()))->identifier();
      auto type_name = dynamic_cast<ast::Identifier const*>(&(i->type()))->identifier();
      obj->type = find_type(*this, type_name);
      if( !obj->type ) {
        std::stringstream strm;
        strm << i->type().location();
        strm << ": typename '" << type_name << "' not found.";
        throw std::runtime_error(strm.str());
      }
      objects[obj->name] = obj;
    }

    // find defined functions 
    auto funcs = ast::find_by_type<ast::Function_def>(tree);
    for(auto i : funcs) {
      std::shared_ptr<Function> func(new Function);
      func->name = dynamic_cast<ast::Identifier const*>(&(i->identifier()))->identifier();
      auto type_name = dynamic_cast<ast::Identifier const*>(&(i->return_type()))->identifier();
      func->return_type = find_type(*this, type_name);
      if( !func->return_type ) {
        std::stringstream strm;
        strm << i->return_type().location();
        strm << ": return type '" << type_name << "' not found.";
        throw std::runtime_error(strm.str());
      }
      functions[func->name] = func;
    }
  }
  //--------------------------------------------------------------------------------
  void
  Namespace::insert_module(ast::Module_def const& mod) {
    auto m = std::shared_ptr<Module>(new Module(mod));
    if( modules.count(m->name) > 0 )
      throw std::runtime_error(std::string("Module with name ")+ m->name +std::string(" already exists"));

    m->scan_ast(mod);
    modules[m->name] = m;
  }
  //--------------------------------------------------------------------------------
  void
  Namespace::insert_namespace(ast::Namespace_def const& ns) {
    auto n = std::shared_ptr<Namespace>(new Namespace(ns));
    if( namespaces.count(n->name) > 0 )
      throw std::runtime_error(std::string("Namespace with name ")+ n->name +std::string(" already exists"));

    n->scan_ast(ns);
    modules[m->name] = m;
  }
  //--------------------------------------------------------------------------------
  void
  Namespace::scan_ast(ast::Node_if const& tree) {
    if( typeid(tree) == typeid(ast::Namespace_def) ) {
      
    } else if( typeid(tree) == typeid(ast::Module_def) ) {
      
    }
  }
  //--------------------------------------------------------------------------------


  //--------------------------------------------------------------------------------
  /** Find a type by name in the hierarchy
   * */
  std::shared_ptr<Type> find_type(Module const& m, Label type_name) {
    // search module types
    {
      auto it = m.types.find(type_name);
      if( it != m.types.end() ) 
        return it->second;
    }

    // search builtin types
    {
      auto it = Builtins::types.find(type_name);
      if( it != Builtins::types.end() )
        return it->second;
    }
    
    // not found
    return std::shared_ptr<Type>(nullptr);
  }
  //--------------------------------------------------------------------------------
  
}
