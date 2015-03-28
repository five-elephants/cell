#include "llvm_function_scanner.h"

#include "ir/find.hpp"
#include "ir/find_hierarchy.h"
#include "ir/builtins.h"

#include <functional>


  namespace sim {

    Llvm_function_scanner::Llvm_function_scanner(Llvm_namespace& ns, Llvm_function& function)
      : m_ns(ns),
        m_function(function),
        m_builder(llvm::getGlobalContext()),
        m_logger(log4cxx::Logger::getLogger("cell.scan")) {
      init_function();
      init_scanner();
    }


    Llvm_function_scanner::Llvm_function_scanner(Llvm_module& mod, Llvm_function& function)
      : m_ns(mod),
        m_mod(&mod),
        m_function(function),
        m_builder(llvm::getGlobalContext()),
        m_logger(log4cxx::Logger::getLogger("cell.scan")) {
      init_function();
      init_scanner();
    }


    void
    Llvm_function_scanner::init_function() {
      using namespace llvm;

      auto lib = ir::find_library(m_ns);
      auto name = ir::hierarchical_name(m_ns, m_function.name);

      LOG4CXX_TRACE(m_logger, "creating function '" << name << "'");

      // create function type
      m_function.impl.func_type = get_function_type(m_function);

      // create function itself
      m_function.impl.code = Function::Create(m_function.impl.func_type,
          Function::ExternalLinkage,
          name,
          lib->impl.module.get());

      // create function entry code
      auto bb = BasicBlock::Create(getGlobalContext(), "entry", m_function.impl.code);
      m_builder.SetInsertPoint(bb);

      // name arguments
      auto arg_i = m_function.impl.code->arg_begin();
      if( m_mod ) {
        arg_i->setName("this_out");
        m_named_values["this_out"] = arg_i;
        (++arg_i)->setName("this_in");
        m_named_values["this_in"] = arg_i;
        (++arg_i)->setName("this_prev");
        m_named_values["this_prev"] = arg_i;
        (++arg_i)->setName("read_mask");
        m_named_values["read_mask"] = arg_i;
      }

      // allocate other arguments on stack
      for(auto arg_name : m_function.parameters) {
        (++arg_i)->setName(arg_name->name);
        auto ptr = m_builder.CreateAlloca(arg_i->getType(), 0, arg_i->getName());
        auto v = m_builder.CreateStore(arg_i, ptr);

        m_named_values[arg_i->getName().str()] = ptr;
        m_named_types[arg_name->name] = arg_name->type;
      }

      // create function body
      auto bb_body = BasicBlock::Create(getGlobalContext(), "body", m_function.impl.code);
      m_builder.CreateBr(bb_body);
      m_builder.SetInsertPoint(bb_body);

      // set target type
      m_type_targets.push_back(m_function.return_type);
    }


    void
    Llvm_function_scanner::init_scanner() {
      this->template on_leave_if_type<ast::Return_statement>(&Llvm_function_scanner::insert_return);
      this->template on_enter_if_type<ast::Variable_ref>(&Llvm_function_scanner::enter_variable_ref);
      this->template on_leave_if_type<ast::Variable_ref>(&Llvm_function_scanner::insert_variable_ref);
      this->template on_leave_if_type<ast::Op_element>(&Llvm_function_scanner::leave_op_element);
      this->template on_enter_if_type<ast::Name_lookup>(&Llvm_function_scanner::enter_name_lookup);
      this->template on_leave_if_type<ast::Variable_def>(&Llvm_function_scanner::leave_variable_def);
      this->template on_visit_if_type<ast::Literal<int>>(&Llvm_function_scanner::insert_literal_int);
      this->template on_visit_if_type<ast::Literal<double>>(&Llvm_function_scanner::insert_literal_double);
      this->template on_visit_if_type<ast::Literal<bool>>(&Llvm_function_scanner::insert_literal_bool);
      this->template on_visit_if_type<ast::Literal<std::string>>(&Llvm_function_scanner::insert_literal_string);
      this->template on_enter_if_type<ast::Phys_literal>(&Llvm_function_scanner::insert_phys_literal);
      this->template on_enter_if_type<ast::Op_at>(&Llvm_function_scanner::enter_op_at);
      this->template on_leave_if_type<ast::Op_at>(&Llvm_function_scanner::insert_op_at);
      this->template on_leave_if_type<ast::Op_not>(&Llvm_function_scanner::insert_op_not);
      this->template on_leave_if_type<ast::Op_equal>(&Llvm_function_scanner::insert_op_equal);
      this->template on_leave_if_type<ast::Op_plus>(&Llvm_function_scanner::insert_op_plus);
      this->template on_leave_if_type<ast::Op_minus>(&Llvm_function_scanner::insert_op_minus);
      this->template on_leave_if_type<ast::Op_mult>(&Llvm_function_scanner::insert_op_mult);
      this->template on_leave_if_type<ast::Op_div>(&Llvm_function_scanner::insert_op_div);
      this->template on_leave_if_type<ast::Op_mod>(&Llvm_function_scanner::insert_op_mod);
      this->template on_leave_if_type<ast::Op_and>(&Llvm_function_scanner::insert_op_and);
      this->template on_leave_if_type<ast::Op_or>(&Llvm_function_scanner::insert_op_or);
      this->template on_leave_if_type<ast::Op_greater_then>(&Llvm_function_scanner::insert_op_gt);
      this->template on_leave_if_type<ast::Op_lesser_then>(&Llvm_function_scanner::insert_op_lt);
      this->template on_leave_if_type<ast::Op_greater_or_equal_then>(&Llvm_function_scanner::insert_op_ge);
      this->template on_leave_if_type<ast::Op_lesser_or_equal_then>(&Llvm_function_scanner::insert_op_le);
      this->template on_enter_if_type<ast::Op_bidir>(&Llvm_function_scanner::insert_op_bidir);
      this->template on_enter_if_type<ast::Assignment>(&Llvm_function_scanner::enter_assignment);
      this->template on_leave_if_type<ast::Assignment>(&Llvm_function_scanner::leave_assignment);
      this->template on_leave_if_type<ast::Compound>(&Llvm_function_scanner::leave_compound);
      this->template on_enter_if_type<ast::If_statement>(&Llvm_function_scanner::enter_if_statement);
      this->template on_leave_if_type<ast::Function_call>(&Llvm_function_scanner::leave_function_call);
      this->template on_leave_if_type<ast::Function_def>(&Llvm_function_scanner::leave_function_def);
      this->template on_leave_if_type<ast::Process>(&Llvm_function_scanner::leave_process);
      this->template on_leave_if_type<ast::Periodic>(&Llvm_function_scanner::leave_periodic);
      this->template on_leave_if_type<ast::Once>(&Llvm_function_scanner::leave_once);
      this->template on_leave_if_type<ast::Recurrent>(&Llvm_function_scanner::leave_recurrent);
    }


    llvm::FunctionType*
    Llvm_function_scanner::get_function_type(Llvm_function const& function) const {
      using namespace llvm;

      auto lib = ir::find_library(m_ns);
      std::vector<Type*> args;

      if( m_mod ) {
        auto r = PointerType::getUnqual(m_mod->impl.mod_type);
        args.push_back(r);
        args.push_back(r);
        args.push_back(r);
        args.push_back(PointerType::getUnqual(read_mask_type()));
      }

      for(auto p : function.parameters) {
        args.push_back(p->type->impl.type);
      }

      return FunctionType::get(function.return_type->impl.type,
          args,
          false);
    }


    llvm::ArrayType*
    Llvm_function_scanner::read_mask_type() const {
      if( m_mod ) {
        auto lib = ir::find_library(m_ns);
        auto mod_type = m_mod->impl.mod_type;
        return llvm::ArrayType::get(llvm::IntegerType::get(lib->impl.context, 1),
            mod_type->getNumElements());
      } else {
        throw std::runtime_error("read_mask makes only sense for functions within modules, but m_mod == nullptr");
      }

      return nullptr;
    }


    bool
    Llvm_function_scanner::insert_return(ast::Return_statement const& node) {
      auto v = m_values.at(node.objects()[0]);
      m_values[&node] = m_builder.CreateRet(v);
      return true;
    }


    bool
    Llvm_function_scanner::enter_variable_ref(ast::Variable_ref const& node) {
      m_lookups.push_back(Lookup_source::in);
      return true;
    }

    bool
    Llvm_function_scanner::insert_variable_ref(ast::Variable_ref const& node) {
      auto ptr = m_values.at(&(node.expression()));
      auto type = m_types.at(&(node.expression()));

      LOG4CXX_TRACE(m_logger, "dereferencing variable of type '"
          << type->name
          << "'");

      m_values[&node] = m_builder.CreateLoad(ptr, "load");
      m_types[&node] = type;

      m_lookups.pop_back();

      return true;
    }


    bool
    Llvm_function_scanner::leave_op_element(ast::Op_element const& node) {
      auto elem_name = dynamic_cast<ast::Identifier const&>(node.right())
          .identifier();
      auto obj_ptr = m_values.at(&node.left());
      auto type = m_types.at(&node.left());

      LOG4CXX_TRACE(m_logger, "element access on '"
          << type->name
          << "' for element '"
          << elem_name
          << "'");

      auto it = type->elements.find(elem_name);
      if( it == type->elements.end() ) {
        std::stringstream strm;
        strm << node.location()
          << ": failed to find element '"
          << elem_name
          << "' within '"
          << type->name
          << "' ("
          << __func__
          << ")";
        throw std::runtime_error(strm.str());
      }

      auto index = it->second->impl.struct_index;
      auto ptr = m_builder.CreateStructGEP(obj_ptr,
          index,
          std::string("ptr_") + elem_name);

      m_values[&node] = ptr;
      m_types[&node] = it->second->type;

      return true;
    }


    bool
    Llvm_function_scanner::enter_name_lookup(ast::Name_lookup const& node) {
      auto id = node.identifier();
      bool found = false;

      LOG4CXX_TRACE(m_logger, "name lookup for '" << id.identifier() << "'");

      // load value
      auto p = m_named_values.find(id.identifier());

      if( p != m_named_values.end() ) {
        m_values[&node] = p->second;
        m_types[&node] = m_named_types.at(id.identifier());
        found = true;
      } else if( m_mod ) {
        // lookup name in module
        auto p = m_mod->objects.find(id.identifier());
        if( p != m_mod->objects.end() ) {
          if( m_lookups.empty() )
            throw std::runtime_error("expecting source specifier for name lookup");

          llvm::Value* source_ptr;
          if( m_lookups.back() == Lookup_source::in )
            source_ptr = m_named_values.at("this_in");
          else if( m_lookups.back() == Lookup_source::out )
            source_ptr = m_named_values.at("this_out");
          else if( m_lookups.back() == Lookup_source::prev )
            source_ptr = m_named_values.at("this_prev");
          else
            throw std::runtime_error("garbage on lookup source stack");

          auto index = p->second->impl.struct_index;
          llvm::Value* ptr_v;
          std::string twine("elem_ptr_");
          twine += id.identifier();

          if( m_mod->instantiations.find(id.identifier()) != m_mod->instantiations.end() ) {
            // this is a module, so access port of module
            auto ptr_to_ptr_to_mod = m_builder.CreateStructGEP(source_ptr,
                index,
                std::string("mod_ptr_ptr_") + id.identifier());
            auto mod_ptr = m_builder.CreateLoad(ptr_to_ptr_to_mod,
                std::string("mod_ptr_") + id.identifier());
            ptr_v = m_builder.CreateStructGEP(mod_ptr, 0, twine);
          } else {
            ptr_v = m_builder.CreateStructGEP(source_ptr, index, twine);
          }


          m_values[&node] = ptr_v;
          m_types[&node] = p->second->type;
          found = true;

          // log read access in read_mask
          auto read_mask = m_named_values.at("read_mask");
          auto read_mask_elem = m_builder.CreateConstGEP2_32(read_mask,
              0,
              index,
              std::string("read_mask_elem_") + id.identifier());
          m_builder.CreateStore(llvm::ConstantInt::get(llvm::getGlobalContext(),
                llvm::APInt(1, 1, false)), read_mask_elem);
        }
      }

      if( !found ) {
        std::stringstream strm;
        strm << node.location()
          << ": unable to find symbol '"
          << id.identifier()
          << "' (" << __func__ << ")";
        throw std::runtime_error(strm.str());
      }

      return false;
    }


    bool
    Llvm_function_scanner::leave_variable_def(ast::Variable_def const& node) {
      // get name
      auto name = dynamic_cast<ast::Identifier const&>(node.identifier()).identifier();
      std::shared_ptr<Llvm_type> type;

      if( m_named_values.count(name) > 0 )
        throw std::runtime_error(std::string("Variable with name ")
            + name
            + std::string(" already exists in function")
            + m_function.name);

      // get type
      if( !node.without_type() ) {
        if( typeid(node.type()) == typeid(ast::Identifier) ) {
          auto& type_name = dynamic_cast<ast::Identifier const&>(node.type()).identifier();
          type = find_type(m_ns, type_name);
          if( !type ) {
            std::stringstream strm;
            strm << node.type().location();
            strm << ": typename '" << type_name << "' not found.";
            throw std::runtime_error(strm.str());
          }
        } else
          throw std::runtime_error("not supported yet");
        /*else if( typeid(node.type()) == typeid(ast::Array_type) ) {
          auto& ar_type = dynamic_cast<ast::Array_type const&>(node.type());

          auto type = make_array_type(m_mod, ar_type);
          m_mod.types[obj->type->name] = obj->type;
        }*/
      } else if( !node.without_expression() ) {
        type = m_types.at(&(node.expression()));
      } else {
        throw std::runtime_error("Can not infer type for variable definition");
      }

      // initial value
      llvm::Value* v = nullptr;
      if( node.without_expression() ) {
        v = llvm::Constant::getNullValue(type->impl.type);
      } else {
        if( m_types[&(node.expression())] != type ) {
          std::stringstream strm;
          strm << "Type mismatch in variable definition: "
            << "declared type is '"
            << type->name
            << "' but assigned type is '"
            << m_types.at(&(node.expression()))->name
            << "'";
          throw std::runtime_error(strm.str());
        }
        v = m_values[&(node.expression())];
      }

      // allocate
      auto ptr = m_builder.CreateAlloca(type->impl.type, 0, name);
      auto store_v = m_builder.CreateStore(v, ptr);

      m_named_values[name] = ptr;
      m_named_types[name] = type;
      m_values[&node] = v;
      m_types[&node] = type;

      return true;
    }


    bool
    Llvm_function_scanner::insert_literal_int(ast::Literal<int> const& node) {
      using namespace llvm;

      auto v = ConstantInt::get(getGlobalContext(),
          APInt(64, node.value(), true));
      auto ty = ir::Builtins<Llvm_impl>::types.at("int");
      m_values[&node] = v;
      m_types[&node] = ty;

      return true;
    }


    bool
    Llvm_function_scanner::insert_literal_double(ast::Literal<double> const& node) {
      using namespace llvm;

      auto ty = ir::Builtins<Llvm_impl>::types.at("float");
      auto v = ConstantFP::get(ty->impl.type, node.value());
      m_values[&node] = v;
      m_types[&node] = ty;

      return true;
    }


    bool
    Llvm_function_scanner::insert_literal_bool(ast::Literal<bool> const& node) {
      using namespace llvm;

      auto v = ConstantInt::get(getGlobalContext(),
          APInt(1, node.value(), true));
      auto ty = ir::Builtins<Llvm_impl>::types.at("bool");
      m_values[&node] = v;
      m_types[&node] = ty;

      return true;
    }


    bool
    Llvm_function_scanner::insert_literal_string(ast::Literal<std::string> const& node) {
      using namespace llvm;

      auto v = m_builder.CreateGlobalStringPtr(node.value(), "stringconst");
      auto ty = ir::Builtins<Llvm_impl>::types.at("string");
      m_values[&node] = v;
      m_types[&node] = ty;

      return true;
    }


    bool
    Llvm_function_scanner::insert_phys_literal(ast::Phys_literal const& node) {
      using namespace llvm;

      auto unit = dynamic_cast<ast::Identifier const&>(node.unit()).identifier();
      int64_t m;

      if( unit == "ps" ) m = 1;
      else if( unit == "ns" ) m = 1000;
      else if( unit == "us" ) m = 1000*1000;
      else if( unit == "ms" ) m = 1000*1000*1000;
      else if( unit == "s" ) m = 1000ll*1000ll*1000ll*1000ll;
      else
        throw std::runtime_error("Unknown time unit");

      int64_t value = node.value() * m;
      auto v = ConstantInt::get(getGlobalContext(),
          APInt(64, value, true));
      auto ty = ir::Builtins<Llvm_impl>::types.at("int");

      m_values[&node] = v;
      m_types[&node] = ty;

      return false;
    }


    bool
    Llvm_function_scanner::enter_op_at(ast::Op_at const& node) {
      m_lookups.push_back(Lookup_source::in);
      return true;
    }

    bool
    Llvm_function_scanner::insert_op_at(ast::Op_at const& node) {
      auto ty = m_types.at(&(node.operand()));
      auto value = m_values.at(&(node.operand()));

      if( !m_mod )
        throw std::runtime_error("Cannot use operator '@' outside of module");

      // select an operator
      std::shared_ptr<Llvm_operator> op = ir::find_operator(m_ns,
          "@",
          ty,
          ty);


      if( op ) {
        auto ret_ty = op->return_type;
        // get previous value of operand
        auto operand = dynamic_cast<ast::Variable_ref const&>(node.operand());
        auto lookup = dynamic_cast<ast::Name_lookup const&>(operand.expression());
        auto operand_id = dynamic_cast<ast::Identifier const&>(lookup.identifier());
        auto p = m_mod->objects.find(operand_id.identifier());
        if( p != m_mod->objects.end() ) {
          auto this_prev = m_named_values.at("this_prev");
          auto index = p->second->impl.struct_index;
          auto ptr_v = m_builder.CreateStructGEP(this_prev,
              index,
              std::string("elem_ptr_prev_") + operand_id.identifier());
          auto prev_val = m_builder.CreateLoad(ptr_v,
              std::string("mod_load_prev_") + operand_id.identifier());

          auto v = op->impl.insert_func(m_builder, value, prev_val);
          m_values[&node] = v;
          m_types[&node] = ret_ty;
        } else {
          throw std::runtime_error("This is basically impossible!");
        }

      } else {
        std::stringstream strm;
        strm << node.location() << ": failed to find operator '"
          << "@"
          << "' with signature: ["
          << ty->name
          << "]";
        throw std::runtime_error(strm.str());
      }

      m_lookups.pop_back();
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_not(ast::Op_not const& node) {
      auto ty = m_types.at(&(node.operand()));
      auto value = m_values.at(&(node.operand()));

      // select an operator
      std::shared_ptr<Llvm_operator> op = ir::find_operator(m_ns,
          "!",
          ty,
          ty);

      if( op ) {
        auto v = op->impl.insert_func(m_builder, value, value);
        m_values[&node] = v;
        m_types[&node] = op->return_type;
      } else {
        std::stringstream strm;
        strm << node.location() << ": failed to find operator '"
          << "!"
          << "' with signature: ["
          << ty->name
          << "]";
        throw std::runtime_error(strm.str());
      }

      return true;
    }


    bool
    Llvm_function_scanner::insert_op_equal(ast::Op_equal const& node) {
      insert_bin_op(node, "==");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_plus(ast::Op_plus const& node) {
      insert_bin_op(node, "+");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_minus(ast::Op_minus const& node) {
      insert_bin_op(node, "-");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_mult(ast::Op_mult const& node) {
      insert_bin_op(node, "*");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_div(ast::Op_div const& node) {
      insert_bin_op(node, "/");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_mod(ast::Op_mod const& node) {
      insert_bin_op(node, "%");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_and(ast::Op_and const& node) {
      insert_bin_op(node, "&&");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_or(ast::Op_or const& node) {
      insert_bin_op(node, "||");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_gt(ast::Op_greater_then const& node) {
      insert_bin_op(node, ">");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_lt(ast::Op_lesser_then const& node) {
      insert_bin_op(node, "<");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_ge(ast::Op_greater_or_equal_then const& node) {
      insert_bin_op(node, ">=");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_le(ast::Op_lesser_or_equal_then const& node) {
      insert_bin_op(node, "<=");
      return true;
    }


    bool
    Llvm_function_scanner::insert_op_bidir(ast::Op_bidir const& node) {
      m_lookups.push_back(Lookup_source::in);
      node.left().accept(*this);
      node.right().accept(*this);
      m_lookups.pop_back();

      auto ptr_in_left = m_values[&(node.left())];
      auto ptr_in_right= m_values[&(node.right())];

      m_lookups.push_back(Lookup_source::out);
      node.left().accept(*this);
      node.right().accept(*this);
      m_lookups.pop_back();

      auto ptr_out_left = m_values[&(node.left())];
      auto ptr_out_right= m_values[&(node.right())];

      auto ty_left = m_types.at(&(node.left()));
      auto ty_right = m_types.at(&(node.right()));

      auto op_left = ir::find_operator(m_ns,
          "<<",
          ty_left,
          ty_right);
      if( !op_left ) {
        std::stringstream strm;
        strm << node.location() << ": failed to find operator '"
          << "<<"
          << "' with signature: ["
          << ty_left->name
          << "] << ["
          << ty_right->name
          << "] -> ["
          //<< ret_ty->name
          << "]";
        throw std::runtime_error(strm.str());
      }

      auto op_right = ir::find_operator(m_ns,
          ">>",
          ty_left,
          ty_right);
      if( !op_right ) {
        std::stringstream strm;
        strm << node.location() << ": failed to find operator '"
          << ">>"
          << "' with signature: ["
          << ty_left->name
          << "] >> ["
          << ty_right->name
          << "] -> ["
          //<< ret_ty->name
          << "]";
        throw std::runtime_error(strm.str());
      }

      op_left->impl.insert_func(m_builder, ptr_out_left, ptr_in_right);
      op_right->impl.insert_func(m_builder, ptr_in_left, ptr_out_right);

      return false;
    }


    bool
    Llvm_function_scanner::enter_assignment(ast::Assignment const& node) {
      m_lookups.push_back(Lookup_source::out);
      return true;
    }


    bool
    Llvm_function_scanner::leave_assignment(ast::Assignment const& node) {
      auto ptr = m_values.at(&(node.identifier()));
      auto target_type = m_types.at(&(node.identifier()));

      LOG4CXX_TRACE(m_logger, "leaving assignment for type '"
          << target_type->name
          << "'");

      // get right-side value
      auto rval = m_values.at(&(node.expression()));
      auto ty = m_types.at(&(node.expression()));

      // conversion needed?
      if( target_type != ty ) {
        LOG4CXX_TRACE(m_logger, "looking for type conversion operator '"
            << ty->name
            << "' to '"
            << target_type->name
            << "'");

        std::shared_ptr<Llvm_operator> op = ir::find_operator(m_ns,
            "convert",
            target_type,
            ty,
            ty);

        if( op ) {
          rval = op->impl.insert_func(m_builder, rval, rval);
        } else {
          std::stringstream strm;
          strm << "type mismatch in assignment: expected '"
            << target_type
            << "' found '"
            << ty->name
            << "' and no conversion found";
          throw std::runtime_error(strm.str());
        }
      }

      // store value
      m_builder.CreateStore(rval, ptr);

      m_values[&node] = rval;
      m_types[&node] = target_type;

      m_lookups.pop_back();

      return true;
    }


    bool
    Llvm_function_scanner::leave_compound(ast::Compound const& node) {
      if( node.return_last() ) {
        m_values[&node] = m_values.at(node.statements().back());
        m_types[&node] = m_types.at(node.statements().back());
      } else {
        auto ty = ir::Builtins<Llvm_impl>::types.at("unit");
        m_values[&node] = llvm::Constant::getNullValue(ty->impl.type);
        m_types[&node] = ty;
      }

      return true;
    }


    bool
    Llvm_function_scanner::enter_if_statement(ast::If_statement const& node) {
      using namespace llvm;

      m_type_targets.push_back(ir::Builtins<Llvm_impl>::types.at("bool"));
      node.condition().accept(*this);
      m_type_targets.pop_back();


      // get condition result and create basic blocks
      auto cond_val = m_values.at(&(node.condition()));
      auto bb_true = BasicBlock::Create(getGlobalContext(), "if_true", m_function.impl.code);
      auto bb_false = BasicBlock::Create(getGlobalContext(), "if_false", m_function.impl.code);
      auto bb_resume = BasicBlock::Create(getGlobalContext(), "if_resume", m_function.impl.code);

      // create conditional branch instruction
      m_builder.CreateCondBr(cond_val, bb_true, bb_false);

      // generate code for true branch
      m_builder.SetInsertPoint(bb_true);
      node.body().accept(*this);
      m_builder.CreateBr(bb_resume);
      bb_true = m_builder.GetInsertBlock();
      auto val_true = m_values[&(node.body())];

      // check matching result types
      auto res_ty = m_types[&(node.body())];

      // generate code for false branch
      m_builder.SetInsertPoint(bb_false);
      Value* val_false = nullptr;
      if( node.has_else_body() ) {
        node.else_body().accept(*this);
        val_false = m_values[&(node.else_body())];

        if( res_ty != m_types[&(node.else_body())] )
          throw std::runtime_error("if expression does not have matching types in both branches");
      } else {
        val_false = Constant::getNullValue(res_ty->impl.type);
      }
      m_builder.CreateBr(bb_resume);
      bb_false = m_builder.GetInsertBlock();

      m_builder.SetInsertPoint(bb_resume);

      // insert phi instruction
      auto pn = m_builder.CreatePHI(res_ty->impl.type, 2, "iftmp");
      pn->addIncoming(val_true, bb_true);
      pn->addIncoming(val_false, bb_false);

      m_types[&node] = res_ty;
      m_values[&node] = pn;

      return false;
    }


    bool
    Llvm_function_scanner::leave_function_call(ast::Function_call const& node) {
      std::vector<ir::Label> qname;
      std::vector<llvm::Value*> args;

      // find function
      for(auto const& n : node.name()) {
        auto const& p = dynamic_cast<ast::Identifier const&>(*n);
        qname.push_back(p.identifier());
      }
      auto func = ir::find_by_path(m_ns,
          &Llvm_namespace::functions,
          qname);
      if( !func ) {
        std::stringstream strm;
        strm << "Unable to find function '";
        std::copy(qname.begin(),
            qname.end(),
            std::ostream_iterator<std::string>(strm, "::"));
        strm << "' to call ("
          << __func__
          << ")";
        throw std::runtime_error(strm.str());
      }

      // add module arguments
      if( func->within_module ) {
        args.push_back(m_named_values.at("this_out"));
        args.push_back(m_named_values.at("this_in"));
        args.push_back(m_named_values.at("this_prev"));
        args.push_back(m_named_values.at("read_mask"));
      }

      // add parameter values
      for(auto i : node.expressions()) {
        args.push_back(m_values.at(i));
      }

      // insert function call
      auto v = m_builder.CreateCall(func->impl.code, args, "callres");

      m_values[&node] = v;
      m_types[&node] = func->return_type;

      return true;
    }


    bool
    Llvm_function_scanner::leave_function_def(ast::Function_def const& node) {
      auto v = m_values.at(&(node.body()));
      m_values[&node] = m_builder.CreateRet(v);
      m_type_targets.pop_back();

      return true;
    }


    bool
    Llvm_function_scanner::leave_process(ast::Process const& node) {
      auto ty = ir::Builtins<Llvm_impl>::types.at("unit");
      auto v = llvm::Constant::getNullValue(ty->impl.type);
      m_values[&node] = m_builder.CreateRet(v);
      m_type_targets.pop_back();

      return true;
    }


    bool
    Llvm_function_scanner::leave_periodic(ast::Periodic const& node) {
      auto ty = ir::Builtins<Llvm_impl>::types.at("unit");
      auto v = llvm::Constant::getNullValue(ty->impl.type);
      m_values[&node] = m_builder.CreateRet(v);
      m_type_targets.pop_back();

      return true;
    }


    bool
    Llvm_function_scanner::leave_once(ast::Once const& node) {
      auto ty = ir::Builtins<Llvm_impl>::types.at("unit");
      auto v = llvm::Constant::getNullValue(ty->impl.type);
      m_values[&node] = m_builder.CreateRet(v);
      m_type_targets.pop_back();

      return true;
    }


    bool
    Llvm_function_scanner::leave_recurrent(ast::Recurrent const& node) {
      auto v = m_values.at(&node.expression());

      if( m_type_targets.back() != m_types.at(&node.expression()) ) {
        std::stringstream strm;
        strm << "recurrent expects type '"
          << m_type_targets.back()->name
          << "' as return type. Found type '"
          << m_types.at(&node.expression())->name
          << "' (erroneous semicolon?)";
        throw std::runtime_error(strm.str());
      }

      m_values[&node] = m_builder.CreateRet(v);
      m_type_targets.pop_back();

      return true;
    }

}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
