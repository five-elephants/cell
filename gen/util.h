#include "gen/generator_if.h"
#include <functional>

namespace gen {

  template<typename Iterator>
  void join_nodes(Iterator a, Iterator b,
      void (Generator_if::*separator)(),
      Generator_if& gen,
      bool with_indent = false) {
    if( a == b )
      return;

    //if( with_indent )
      //gen.indent();
    (*a)->visit();

    auto i = a;
    while( ++i != b ) {
      (gen.*separator)(); 
      //if( with_indent )
        //gen.indent();
      (*i)->visit();
    }
  }


  template<typename Iterator>
  void join_nodes_comma(Iterator a, Iterator b, Generator_if& gen) {
    join_nodes(a, b, &Generator_if::comma_sep, gen);
  }


  template<typename Iterator>
  void join_nodes_line(Iterator a, Iterator b, Generator_if& gen) {
    join_nodes(a, b, &Generator_if::line_sep, gen, true);
  }

}
