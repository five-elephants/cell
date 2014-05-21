#include "sim/stream_instrumenter.h"


namespace sim {

  void
  Stream_instrumenter::module(ir::Time const& t, Module_inspector& insp) {
    m_os << "t = " << t << ": {\n";
    for(auto i=0; i<insp.num_elements(); i++) {
      m_os << "    " 
       << insp.get_name(i) << ": "
       << insp.get<int64_t>(i) << "\n";
    }
    m_os << "}\n";
  }

}

