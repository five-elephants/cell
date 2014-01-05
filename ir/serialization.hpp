#pragma once

#include "ir/namespace.h"

#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>

namespace boost {
  namespace serialization {


    template<typename Archive>
    void serialize(Archive& ar, ir::Type& type, unsigned int const version) {
      ar & BOOST_SERIALIZATION_NVP(type.name);
    }


    template<typename Archive>
    void serialize(Archive& ar, ir::Object& obj, unsigned int const version) {
      ar & BOOST_SERIALIZATION_NVP(obj.type);
      ar & BOOST_SERIALIZATION_NVP(obj.name);
    }

    template<typename Archive>
    void serialize(Archive& ar, ir::Function& func, unsigned int const version) {
      ar & BOOST_SERIALIZATION_NVP(func.return_type);
      ar & BOOST_SERIALIZATION_NVP(func.name);
    }

    template<typename Archive>
    void serialize(Archive& ar, ir::Module& mod, unsigned int const version) {
      ar & BOOST_SERIALIZATION_NVP(mod.types);
      ar & BOOST_SERIALIZATION_NVP(mod.objects);
      ar & BOOST_SERIALIZATION_NVP(mod.functions);
    }

    template<typename Archive>
    void serialize(Archive& ar, ir::Namespace& ns, unsigned int const version) {
      ar & BOOST_SERIALIZATION_NVP(ns.name);
      ar & BOOST_SERIALIZATION_NVP(ns.modules);
      ar & BOOST_SERIALIZATION_NVP(ns.namespaces);
    }


  }
}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
