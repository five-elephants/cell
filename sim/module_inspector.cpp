#include "module_inspector.h"

namespace sim {

  Module_inspector::Module_inspector()
    : m_this_ptr(nullptr),
      m_layout(nullptr),
      m_num_elements(0),
      m_exe(nullptr) {
  }


  Module_inspector::Module_inspector(std::shared_ptr<Llvm_module> mod,
      void* this_ptr,
      llvm::StructLayout const* layout,
      unsigned num_elements,
      llvm::ExecutionEngine* exe)
    : m_module(mod),
      m_this_ptr(static_cast<char*>(this_ptr)),
      m_layout(layout),
      m_num_elements(num_elements),
      m_exe(exe) {
  }

}
