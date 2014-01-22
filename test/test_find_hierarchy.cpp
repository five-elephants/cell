#include "ir/find_hierarchy.h"

#include <iostream>
#include <gtest/gtest.h>

TEST(ir, findHierarchy) {
  std::string path = "this.is.a.test";
  auto path_elems = ir::parse_path(path, ".");

  EXPECT_EQ(path_elems.size(), 4);

  std::cout << "PATH: " << path << "\n";
  for(auto i : path_elems) {
    std::cout << "   '" << i << "'\n";
  } 
}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
