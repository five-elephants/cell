#include "module_inspector.h"

namespace sim {

  //Module_inspector::Module_inspector()
    //: m_layout(nullptr),
      //m_num_elements(0),
      //m_exe(nullptr) {
  //}


  Module_inspector::Module_inspector(std::shared_ptr<Llvm_module> mod,
      llvm::StructLayout const* layout,
      unsigned num_elements,
      llvm::ExecutionEngine* exe,
      Memory const& memory,
      Runset::Module& runset)
    : m_module(mod),
      m_layout(layout),
      m_num_elements(num_elements),
      m_exe(exe),
      m_memory(memory),
      m_runset(runset) {
  }

}
