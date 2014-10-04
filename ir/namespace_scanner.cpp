
namespace ir {

  template<typename Impl>
  bool
  Namespace_scanner<Impl>::insert_module(ast::Module_def const& mod) {
    auto label = dynamic_cast<ast::Identifier const&>(mod.identifier()).identifier();
    auto m = std::make_shared<Module<Impl>>(label);
    if( m_ns.modules.count(m->name) > 0 )
      throw std::runtime_error(std::string("Module with name ")+ m->name +std::string(" already exists"));

    m->enclosing_ns = &m_ns;
    Module_scanner<Impl> scanner(*m);
    mod.accept(scanner);
    m_ns.modules[m->name] = m;

    return false;
  }

  template<typename Impl>
  bool
  Namespace_scanner<Impl>::insert_namespace(ast::Namespace_def const& ns) {
    auto label = dynamic_cast<ast::Identifier const*>(&(ns.identifier()))->identifier();
    auto n = std::make_shared<Namespace<Impl>>(label);
    //auto n = std::shared_ptr<Namespace<Impl>>(new Namespace(label));
    if( m_ns.namespaces.count(n->name) > 0 )
      throw std::runtime_error(std::string("Namespace with name ")+ n->name +std::string(" already exists"));

    n->enclosing_ns = &m_ns;

    Namespace_scanner<Impl> scanner(*n);
    ns.accept(scanner);

    m_ns.namespaces[n->name] = n;

    return false;
  }


  template<typename Impl>
  bool
  Namespace_scanner<Impl>::insert_socket(ast::Socket_def const& sock) {
    auto label = dynamic_cast<ast::Identifier const*>(&(sock.identifier()))->identifier();
    auto s = std::make_shared<Socket<Impl>>(label);
    //auto s = std::shared_ptr<Socket>(new Socket(label));
    if( m_ns.sockets.count(s->name) > 0 )
      throw std::runtime_error(std::string("Socket with name ") + s->name +std::string(" already exists"));
    if( m_ns.types.count(s->name) > 0 )
      throw std::runtime_error(std::string("Type with name ") + s->name +std::string(" already exists"));

    s->enclosing_ns = &m_ns;
    scan_ast(*s, sock);
    m_ns.sockets[s->name] = s;
    m_ns.types[s->name] = s;

    return false;
  }


  template<typename Impl>
  bool
  Namespace_scanner<Impl>::insert_function(ast::Function_def const& node) {
    auto func = create_function(node);
    m_ns.functions[func->name] = func;

    return false;
  }


  template<typename Impl>
  std::shared_ptr<ir::Function<Impl>>
  Namespace_scanner<Impl>::create_function(ast::Function_def const& node) {
    std::shared_ptr<Function<Impl>> func(new Function<Impl>());

    // get name
    func->name = dynamic_cast<ast::Identifier const*>(&(node.identifier()))->identifier();
    if( m_ns.functions.count(func->name) > 0 )
      throw std::runtime_error(std::string("Function with name ")+ func->name +std::string(" already exists"));

    // get return type
    auto type_name = dynamic_cast<ast::Identifier const*>(&(node.return_type()))->identifier();
    func->return_type = find_type(m_ns, type_name);
    if( !func->return_type ) {
      std::stringstream strm;
      strm << node.return_type().location();
      strm << ": return type '" << type_name << "' not found.";
      throw std::runtime_error(strm.str());
    }

    // get parameters
    auto params = node.parameters();
    for(auto p_node : params) {
      if( typeid(*p_node) != typeid(ast::Function_param) )
        throw std::runtime_error("function parameter is not of type Function_param ("
            + std::string(typeid(p_node).name())
            + std::string(" instead)"));

      auto p = dynamic_cast<ast::Function_param*>(p_node);
      auto p_ir = std::make_shared<Object<Impl>>();
      p_ir->name = dynamic_cast<ast::Identifier const&>(p->identifier()).identifier();
      //if( func->parameters.count(p_ir->name) > 0 )
      if( std::any_of(func->parameters.begin(),
            func->parameters.end(),
            [p_ir](std::shared_ptr<Object<Impl>> const& i) { return i->name == p_ir->name; }) ) {
        throw std::runtime_error(std::string("Parameter with name ")
            + p_ir->name
            + std::string(" already defined"));
      }

      auto type_name = dynamic_cast<ast::Identifier const&>(p->type()).identifier();
      p_ir->type = find_type(m_ns, type_name);
      if( !p_ir->type ) {
        std::stringstream strm;
        strm << p->type().location();
        strm << ": typename '" << type_name << "' not found.";
        throw std::runtime_error(strm.str());
      }

      //func->parameters[p_ir->name] = p_ir;
      func->parameters.push_back(p_ir);
    }

    m_ns.functions[func->name] = func;

    return func;
  }

}
