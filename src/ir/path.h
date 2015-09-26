#pragma once

#include "namespace.h"

#include <boost/filesystem.hpp>
#include "logging/logger.h"


namespace ir {

  template<typename Impl>
  std::string path_lookup(std::shared_ptr<Library<Impl>> lib,
      Label const& name) {
    namespace bf = boost::filesystem;

    static std::string const ext(".cell");

    log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("cell.path");
    std::string const needle(name + ext);

    for(auto const& p : lib->lookup_path) {
      auto path = bf::path(p) /= needle;
      LOG4CXX_DEBUG(logger, "searching for " << path);
      if( bf::exists(path) && bf::is_regular_file(path) )
        return path.string();
    }

    return std::string("");
  }

}


/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */
