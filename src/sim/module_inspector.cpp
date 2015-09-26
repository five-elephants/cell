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
      Runset& runset)
    : m_module(mod),
      m_layout(layout),
      m_num_elements(num_elements),
      m_exe(exe),
      m_runset(runset) {
    auto it = std::find_if(std::begin(m_runset.modules),
        std::end(m_runset.modules),
        [&mod](Runset::Module m) -> bool { return m.mod == mod; });
    if( it != std::end(m_runset.modules) ) {
      this_in = it->this_in;
      this_out = it->this_out;
      read_mask = it->read_mask;
    } else {
      throw std::runtime_error("unable to find matching module in runset");
    }
  }

}
