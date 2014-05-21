#pragma once

#include "ir/namespace.h"
#include "ir/find.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <llvm/IR/DataLayout.h>


namespace sim {

  class Module_inspector {
    public:
      Module_inspector();
      Module_inspector(std::shared_ptr<ir::Module> module,
          void* this_ptr,
          llvm::StructLayout const* layout,
          unsigned num_elements);

      template<typename T>
      T get(ir::Label const& var_name) {
        auto it = m_module->objects.find(var_name);
        if( it == m_module->objects.end() ) {
          std::stringstream strm;
          strm << "object '" << var_name << "' requested for introspection"
            " not found in module '"
            << m_module->name << "'";
          throw std::runtime_error(strm.str());
        }

        auto idx = std::distance(m_module->objects.begin(), it);
        auto ofs = m_layout->getElementOffset(idx);

        T rv;
        std::copy_n(m_this_ptr + ofs, sizeof(rv), reinterpret_cast<char*>(&rv));
        return rv;
      }

      template<typename T>
      T get(std::size_t idx) {
        auto ofs = m_layout->getElementOffset(idx);

        T rv;
        std::copy_n(m_this_ptr + ofs, sizeof(rv), reinterpret_cast<char*>(&rv));
        return rv;
      }

      
      ir::Label const& get_name(std::size_t idx) {
        auto it = m_module->objects.begin();
        std::advance(it, idx);
        return it->first;
      }

      unsigned num_elements() const { return m_num_elements; }

    private:
      std::shared_ptr<ir::Module> m_module;
      char* m_this_ptr;
      llvm::StructLayout const* m_layout;
      unsigned m_num_elements;
  };

}
