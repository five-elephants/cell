#include "sim/runset.h"


namespace sim {

  Runset::Module
  Runset::create_module(Memory& mem,
      llvm::ExecutionEngine* exe,
      std::shared_ptr<Llvm_module> mod) {
    Module rv;
    rv.mod = mod;
    rv.this_in = mem.allocate_module_frame(mod);
    std::fill_n(rv.this_in.get(), mem.module_frame_size(mod), 0);
    rv.this_out = mem.allocate_module_frame(mod);
    std::fill_n(rv.this_out.get(), mem.module_frame_size(mod), 0);
    rv.read_mask = mem.allocate_read_mask(mod);
    rv.read_mask_sz = mem.read_mask_size(mod);
    rv.sensitivity.resize(mod->impl.mod_type->getNumElements());
    rv.layout = mem.layout()->getStructLayout(mod->impl.mod_type);

    for(auto proc : mod->processes) {
      Process p;
      p.function = proc->function->impl.code;
      p.exe_ptr = exe->getPointerToFunction(p.function);

      rv.processes.push_back(p);
      rv.run_list.insert(p);
    }

    //for(auto proc : m_top_mod->periodicals) {
      //Process p;
      //p.function = m_code->get_process(proc);
      //p.exe_ptr = m_exe->getPointerToFunction(p.function);
      //p.sensitive = false;

      //mod.run_list.insert(p);

      //mod.periodicals.insert(std::make_pair(proc->period, p));
      //mod.schedule.insert(std::make_pair(proc->period, std::make_tuple(proc->period, p)));
    //}

    return rv;
  }

}

/* vim: set et fenc= ff=unix sts=2 sw=2 ts=2 : */

