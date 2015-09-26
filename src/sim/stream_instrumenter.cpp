#include "sim/stream_instrumenter.h"


namespace sim {

  void
  Stream_instrumenter::register_module(std::shared_ptr<Module_inspector> insp) {
  }


  void
  Stream_instrumenter::initial(ir::Time const& t) {
  }


  void
  Stream_instrumenter::step(ir::Time const& t) {
  }


  //void
  //Stream_instrumenter::module(ir::Time const& t, Module_inspector& insp) {
    //m_os << "t = " << t << ": {\n";
    //for(auto i=0; i<insp.num_elements(); i++) {
      //m_os << "    "
       //<< insp.get_name(i) << ": "
       //<< insp.get<int64_t>(i) << "\n";
    //}
    //m_os << "}\n";
  //}


  void
  Stream_instrumenter::push_hierarchy() {
  }


  void
  Stream_instrumenter::pop_hierarchy() {
  }

}

