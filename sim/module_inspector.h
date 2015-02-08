#pragma once

#include "sim/llvm_namespace.h"
#include "ir/find.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <llvm/IR/DataLayout.h>
#include <llvm/ExecutionEngine/JIT.h>


namespace sim {

  class Module_inspector {
    public:
      Module_inspector();
      Module_inspector(std::shared_ptr<Llvm_module> module,
          llvm::StructLayout const* layout,
          unsigned num_elements,
          llvm::ExecutionEngine* exe);

      /** get value of a member variable */
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

        char* this_ptr = m_module->impl.frame.get();
        auto idx = std::distance(m_module->objects.begin(), it);
        auto ofs = m_layout->getElementOffset(idx);

        T rv;
        std::copy_n(this_ptr + ofs, sizeof(rv), reinterpret_cast<char*>(&rv));
        return rv;
      }

      /** get value of member variable by index */
      template<typename T>
      T get(std::size_t idx) {
        char* this_ptr = m_module->impl.frame.get();
        auto ofs = m_layout->getElementOffset(idx);

        T rv;
        std::copy_n(this_ptr + ofs, sizeof(rv), reinterpret_cast<char*>(&rv));
        return rv;
      }


      /** get name from member index */
      ir::Label const& get_name(std::size_t idx) {
        auto it = m_module->objects.begin();
        std::advance(it, idx);
        return it->first;
      }

      /** get number of member variables */
      unsigned num_elements() const { return m_num_elements; }


      /** get a pointer to a function in the module */
      template<typename Func>
      Func* get_function_ptr(ir::Label const& name) {
        auto func = m_module->functions.at(name);
        void* ptr = m_exe->getPointerToFunction(func->impl.code);

        return reinterpret_cast<Func*>(ptr);
      }

      Llvm_impl::Module::Frame& get_frame() {
        return m_module->impl.frame;
      }


      std::shared_ptr<Llvm_module> module() {
        return m_module;
      }


    private:
      std::shared_ptr<Llvm_module> m_module;
      llvm::StructLayout const* m_layout;
      unsigned m_num_elements;
      llvm::ExecutionEngine* m_exe;
  };

}
