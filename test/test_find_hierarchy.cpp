#include "ir/find_hierarchy.h"

#include <iostream>
#include <gtest/gtest.h>

static std::string parseCode(std::regex_constants::error_type etype) {
  using namespace std;

  switch (etype) {
    case regex_constants::error_collate:
        return "error_collate: invalid collating element request";
    case regex_constants::error_ctype:
        return "error_ctype: invalid character class";
    case regex_constants::error_escape:
        return "error_escape: invalid escape character or trailing escape";
    case regex_constants::error_backref:
        return "error_backref: invalid back reference";
    case regex_constants::error_brack:
        return "error_brack: mismatched bracket([ or ])";
    case regex_constants::error_paren:
        return "error_paren: mismatched parentheses(( or ))";
    case regex_constants::error_brace:
        return "error_brace: mismatched brace({ or })";
    case regex_constants::error_badbrace:
        return "error_badbrace: invalid range inside a { }";
    case regex_constants::error_range:
        return "erro_range: invalid character range(e.g., [z-a])";
    case regex_constants::error_space:
        return "error_space: insufficient memory to handle this regular expression";
    case regex_constants::error_badrepeat:
        return "error_badrepeat: a repetition character (*, ?, +, or {) was not preceded by a valid regular expression";
    case regex_constants::error_complexity:
        return "error_complexity: the requested match is too complex";
    case regex_constants::error_stack:
        return "error_stack: insufficient memory to evaluate a match";
    default:
        return "";
  }
}

TEST(ir, findHierarchy) {
  using namespace std;
  using namespace ir;

  struct Test_vector {
    string path;
    vector<string> broken;
    string separator;
  };
 
  array<Test_vector, 6> vectors {{
    { "this.is.a.test", { "this", "is", "a", "test" }, "." },
    { "one", { "one" }, "." },
    { "", { }, "." },
    { ".", { "", "" }, "." },
    { "..", { "", "", "" }, "." },
    { "1.2.3", { "1", "2", "3" }, "." }
  }};

  array<Test_vector, 4> vec_throw {{
    { " ", { " " }, "." },
    { ". ", { }, "." },
    { ".\n", {}, "." },
    { "abc.\t.asd", {}, "." }
  }};

  array<vector<string>, 5> vec_constr {{
    { "a", "nother", "test" },
    { "this", "is", "stupid" },
    { "one" },
    { "two", "twob" },
    { }
  }};

  array<string, 5> seps {{
    ".", "::", ":", "--", "<-#harfdarf#->"
  }};

  for(size_t i=0; i<vectors.size(); i++) {
    auto path_elems = ir::parse_path(vectors[i].path, vectors[i].separator);

    cout << "'" << vectors[i].path << "' -> ";
    for(auto elem : path_elems)
      cout << "'" << elem << "' ";
    cout << '\n';

    EXPECT_EQ(vectors[i].broken.size(), path_elems.size());

    for(size_t j=0; j<min(path_elems.size(), vectors[i].broken.size()); j++)
      EXPECT_EQ(path_elems[j], vectors[i].broken[j]);
  }

  for(auto v : vec_constr) {
    for(auto sep : seps) {
      string s;

      if( !v.empty() ) {
        s += v[0];
        for(auto i=begin(v)+1; i!=end(v); ++i) {
          s += sep;
          s += *i;
        }
      }

      cout << s << '\n';
      auto path_elems = ir::parse_path(s, sep);

      EXPECT_EQ(v.size(), path_elems.size());
    }
  }

  for(auto v : vec_throw) {
    std::vector<string> path_elems;
    EXPECT_THROW(path_elems = ir::parse_path(v.path, v.separator),
        std::runtime_error);
  }
}

TEST(general, regex) {
  using namespace std;

  /* neither clang-3.3 nor gcc-4.8 seem to have usable regex support on Fedora 19 */
  EXPECT_THROW(regex integer("(\\+|-)?[[:digit:]]+"), regex_error);
}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
