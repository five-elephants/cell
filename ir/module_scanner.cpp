
  namespace ir {
    template<typename Impl>
    bool
    Module_scanner<Impl>::insert_function(ast::Function_def const& node) {
      std::cout << "Module_scanner::insert_function" << std::endl;
      auto func = this->create_function(node);

      // TODO generate code for function body

      this->m_ns.functions[func->name] = func;

      return false;
    }


    template<typename Impl>
    bool
    Module_scanner<Impl>::insert_socket(ast::Socket_def const& node) {
      return Namespace_scanner<Impl>::insert_socket(node);
    }


    template<typename Impl>
    bool
    Module_scanner<Impl>::insert_object(ast::Variable_def const& node) {
      auto obj = create_object(node);
      m_mod.objects[obj->name] = obj;
      return false;
    }


    template<typename Impl>
    bool
    Module_scanner<Impl>::insert_instantiation(ast::Module_instantiation const& node) {
      create_instantiation(node);
      return false;
    }


    template<typename Impl>
    bool
    Module_scanner<Impl>::insert_process(ast::Process const& node) {
      auto proc = create_process(node);
      m_mod.processes.push_back(proc);

      return false;
    }


    template<typename Impl>
    bool
    Module_scanner<Impl>::insert_periodic(ast::Periodic const& node) {
      auto rv = std::make_shared<Periodic<Impl>>();

      rv->period = Time(2, Time::ns);
      m_mod.periodicals.push_back(rv);

      return false;
    }


    template<typename Impl>
    bool
    Module_scanner<Impl>::insert_once(ast::Once const& node) {
      auto rv = std::make_shared<Once<Impl>>();

      rv->time = Time(2, Time::ns);
      m_mod.onces.push_back(rv);

      return false;
    }


    template<typename Impl>
    bool
    Module_scanner<Impl>::insert_recurrent(ast::Recurrent const& node) {
      auto rv = std::make_shared<Recurrent<Impl>>();
      rv->time_id = dynamic_cast<ast::Identifier const&>(node.time_id()).identifier();
      m_mod.recurrents.push_back(rv);

      return false;
    }


    template<typename Impl>
    std::shared_ptr<Object<Impl>>
    Module_scanner<Impl>::create_object(ast::Variable_def const& node) {
      std::shared_ptr<Object<Impl>> obj(new Object<Impl>());

      // get name
      obj->name = dynamic_cast<ast::Identifier const&>(node.identifier()).identifier();
      if( m_mod.objects.count(obj->name) > 0 )
        throw std::runtime_error(std::string("Variable with name ")
            + obj->name
            + std::string(" already exists"));

      // get type
      if( typeid(node.type()) == typeid(ast::Identifier) ) {
        auto& type_name = dynamic_cast<ast::Identifier const&>(node.type()).identifier();
        obj->type = find_type(m_mod, type_name);
        if( !obj->type ) {
          std::stringstream strm;
          strm << node.type().location();
          strm << ": typename '" << type_name << "' not found.";
          throw std::runtime_error(strm.str());
        }
      } else if( typeid(node.type()) == typeid(ast::Array_type) ) {
        auto& ar_type = dynamic_cast<ast::Array_type const&>(node.type());

        obj->type = make_array_type(m_mod, ar_type);
        m_mod.types[obj->type->name] = obj->type;
      }

      return obj;
    }


    template<typename Impl>
    std::shared_ptr<Instantiation<Impl>>
    Module_scanner<Impl>::create_instantiation(ast::Module_instantiation const& node) {
      std::shared_ptr<Instantiation<Impl>> inst(new Instantiation<Impl>);
      std::string module_name;

      inst->name = dynamic_cast<ast::Identifier const&>(node.instance_name()).identifier();
      if( m_mod.instantiations.count(inst->name) > 0 )
        throw std::runtime_error(std::string("Instantiation with name ")
            + inst->name
            + std::string(" already exists"));

      if( node.is_template_instantiation() ) {
        LOG4CXX_TRACE(Namespace_scanner<Impl>::m_logger, "instantiating module template");
        // extract name
        auto tmpl_id = node.template_identifier();
        std::vector<Label> qname = tmpl_id.name();
        std::string qname_join = boost::algorithm::join(qname, "::");

        // find template
        std::shared_ptr<Module_template<Impl>> tmpl;
        if( qname.size() > 1 ) {
          tmpl = find_by_path(m_mod,
              &Module<Impl>::module_templates,
              qname);
        } else {
          tmpl = find_module_template(m_mod, qname[0]);
        }

        if( !tmpl ) {
          std::stringstream strm;
          strm << "Can not find module template with name '"
            << qname_join
            << "'";
          throw std::runtime_error(strm.str());
        }

        // determine module name and type overrides
        module_name = qname_join + '<';
        auto type_names = tmpl_id.arg_type_names();
        std::map<Label,std::shared_ptr<Type<Impl>>> types;
        auto it_placeholder = tmpl->type_names.begin();
        for(auto it=type_names.begin();
            (it != type_names.end()) && (it_placeholder != tmpl->type_names.end()); ) {
          // find type
          auto ty = find_type(m_mod, *it);
          if( !ty ) {
            std::stringstream strm;
            strm << node.location() << ": "
              << "failed to find type '"
              << *it
              << "' in instantiation of template '"
              << qname_join
              << "'";
            throw std::runtime_error(strm.str());
          }
          types[*it_placeholder] = ty;

          // append name
          module_name += *it;
          if( ++it != type_names.end() )
            module_name += ',';
          ++it_placeholder;
        }
        module_name += '>';

        // run Module_scanner
        inst->module = this->instantiate_module_template(module_name,
            tmpl->module_node,
          types);
    } else {
      module_name = dynamic_cast<ast::Identifier const&>(node.module_name()).identifier();
      inst->module = find_module(m_mod, module_name);
    }

    if( !inst->module ) {
      std::stringstream strm;
      strm << node.location();
      strm << ": module '" << module_name << "' not found.";
      throw std::runtime_error(strm.str());
    }

    std::set<Label> matched_ports;
    for(auto& i : node.connection_items()) {
      if( typeid(*i) == typeid(ast::Connection_item) ) {
        auto& con_item = dynamic_cast<ast::Connection_item const&>(*i);
        auto port_name = con_item.port_name().identifier();
        //auto signal_name = con_item.signal_name().identifier();

        if( matched_ports.count(port_name) > 0 ) {
          std::stringstream strm;
          strm << con_item.port_name().location();
          strm << ": Port already connected";
          throw std::runtime_error(strm.str());
        }

        std::shared_ptr<Port_assignment<Impl>> port_assign(new Port_assignment<Impl>());
        port_assign->port = find_port(*(inst->module), port_name);
        if( !port_assign->port ) {
          std::stringstream strm;
          strm << con_item.port_name().location();
          strm << ": port '" << port_name << "' not found.";
          throw std::runtime_error(strm.str());
        }

        //port_assign->object = find_object(m_mod, signal_name);
        //if( !port_assign->object ) {
          //std::stringstream strm;
          //strm << con_item.signal_name().location();
          //strm << ": assigned object '" << signal_name << "' not found.";
          //throw std::runtime_error(strm.str());
        //}

        //if( !type_compatible(*(port_assign->port->type), *(port_assign->object->type)) ) {
          //std::stringstream strm;
          //strm << con_item.location();
          //strm << ": incompatible types in port assignment: expected type '"
            //<< *(port_assign->port->type)
            //<< "' got '"
            //<< *(port_assign->object->type) << "'";
          //throw std::runtime_error(strm.str());
        //}

        inst->connection.push_back(port_assign);
        matched_ports.insert(port_name);
      } else if( typeid(*i) == typeid(ast::Identifier) ) {
        auto& obj_name = dynamic_cast<ast::Identifier const&>(*i).identifier();
        auto assignee = find_object(m_mod, obj_name);

        if( !assignee ) {
          std::stringstream strm;
          strm << i->location()
            << ": object '" << obj_name << "' not found";
          throw std::runtime_error(strm.str());
        }

        auto assignee_socket = find_socket(m_mod, assignee->type->name);
        if( !assignee_socket ) {
          std::stringstream strm;
          strm << i->location()
            << ": object '" << obj_name << "' of type '" << assignee->type->name
            << "' is not a socket";
          throw std::runtime_error(strm.str());
        }

        for(auto assignee_port_pair : assignee_socket->elements) {
          auto port_name = assignee_port_pair.first;
          auto assignee_port = assignee_port_pair.second;
          auto searchit = assignee->type->elements.find(port_name);
          if( searchit != assignee->type->elements.end() ) {
            auto it = searchit->second;
            if( !type_compatible(*(it->type), *(assignee_port->type)) ) {
              std::stringstream strm;
              strm << i->location()
                << ": name match for port '" << port_name << "'"
                << " but no type match (expected: "
                << *(it->type)
                << ", got: "
                << *(assignee_port->type)
                << ")";
              throw std::runtime_error(strm.str());
            }

            if( matched_ports.count(assignee_port->name) > 0 ) {
              std::stringstream strm;
              strm << i->location()
                << ": port '" << assignee_port->name << "' already matched";
              throw std::runtime_error(strm.str());
            }

            std::shared_ptr<Port_assignment<Impl>> port_assign(new Port_assignment<Impl>);
            port_assign->port = it;
            // XXX assign object to element of composite type socket
            inst->connection.push_back(port_assign);
            matched_ports.insert(assignee_port->name);
          }
        }
      } else
        throw std::runtime_error("connection items should be either a list or a single identifier");
    }
    m_mod.instantiations[inst->name] = inst;

    auto obj = std::make_shared<Object<Impl>>();
    obj->name = inst->name;
    obj->type = inst->module->socket;

    m_mod.objects[obj->name] = obj;

    return inst;
  }


  template<typename Impl>
  std::shared_ptr<ir::Function<Impl>>
  Module_scanner<Impl>::create_function(ast::Function_def const& node) {
    auto rv = Namespace_scanner<Impl>::create_function(node);
    rv->within_module = true;
    return rv;
  }


  template<typename Impl>
  std::shared_ptr<ir::Process<Impl>>
  Module_scanner<Impl>::create_process(ast::Process const& node) {
    auto rv = std::make_shared<Process<Impl>>();
    return rv;
  }


  template<typename Impl>
  std::shared_ptr<Constant<Impl>>
  Module_scanner<Impl>::create_constant(ast::Constant_def const& node) {
    auto c = Namespace_scanner<Impl>::create_constant(node);
    if( m_mod.objects.count(c->name) > 0 ) {
      std::stringstream strm;
      strm << node.location() << ": "
        << "Constant definition has conflicting name with previous "
              "variable definition ('"
        << c->name << "')";
      throw std::runtime_error(strm.str());
    }
    return c;
  }


  template<typename Impl>
  std::shared_ptr<Module<Impl>>
  Module_scanner<Impl>::instantiate_module_template(Label inst_name,
      ast::Module_def const* node,
      std::map<Label,std::shared_ptr<Type<Impl>>> const& args) {
    LOG4CXX_TRACE(this->m_logger, "creating module '"
        << inst_name << "' from template");

    auto existing = m_mod.modules.find(inst_name);
    if( existing != m_mod.modules.end() )
      return existing->second;

    auto m = std::make_shared<Module<Impl>>(inst_name);
    m->enclosing_ns = &m_mod;
    m->enclosing_library = m_mod.enclosing_library;

    // inject type arguments
    for(auto ty : args) {
      LOG4CXX_TRACE(this->m_logger, "overriding type parameters '"
          << ty.first << "' with type '"
          << ty.second->name << "'");
      m->types[ty.first] = ty.second;
    }

    Module_scanner<Impl> scanner(*m);
    node->accept(scanner);
    m_mod.modules[m->name] = m;

    return m;
  }

}

/* vim: set et fenc=utf-8 ff=unix sts=2 sw=2 ts=2 : */
