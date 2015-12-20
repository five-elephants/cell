#include "sim/simulation_engine.h"
#include "logging/logger.h"

#include <gtest/gtest.h>


class Test_module_inspector : public ::testing::Test {
  protected:
    virtual void SetUp() {
      init_logging();

      m_logger = log4cxx::Logger::getLogger("cell.test");
    }

    log4cxx::LoggerPtr m_logger;
};



TEST_F(Test_module_inspector, get_members) {
  sim::Simulation_engine engine("../lib/test/basic_fsm.cell",
      "test");

  engine.setup();
  auto intro = engine.inspect_module("");

  EXPECT_EQ(1, intro.get<int64_t>("state"));
  EXPECT_EQ(0, intro.get<int64_t>("ctr"));
  EXPECT_EQ(true, intro.get<bool>("reset"));
  EXPECT_EQ(false, intro.get<bool>("clk"));


  for(std::size_t i=0; i<intro.num_elements(); ++i) {
    auto obj = intro.get_object(i);
    auto name = intro.get_name(i);
    auto bits_by_i = intro.get_bits(i);
    auto bits_by_name = intro.get_bits(name);

    EXPECT_EQ(obj->name, name);
    EXPECT_EQ(bits_by_i, bits_by_name);
  }

  engine.teardown();
}


TEST_F(Test_module_inspector, get_bits) {
  sim::Simulation_engine engine("../lib/test/basic_fsm.cell",
      "test");

  engine.setup();
  auto intro = engine.inspect_module("");

  auto state_bits = intro.get_bits("state");
  ir::Bitset exp_bits(std::string("0000000000000000000000000000000000000000000000000000000000000001"));
  EXPECT_EQ(state_bits, exp_bits);

  exp_bits = ir::Bitset(std::string("0000000000000000000000000000000000000000000000000000000000000000"));
  auto ctr_bits = intro.get_bits("ctr");
  EXPECT_EQ(exp_bits, ctr_bits);

  exp_bits = ir::Bitset(std::string("1"));
  auto reset_bits = intro.get_bits("reset");
  EXPECT_EQ(exp_bits, reset_bits);

  exp_bits = ir::Bitset(std::string("0"));
  auto clk_bits = intro.get_bits("clk");
  EXPECT_EQ(exp_bits, clk_bits);

  engine.teardown();
}


TEST_F(Test_module_inspector, get_element_bits) {
  sim::Simulation_engine engine("../lib/test/demo_fsm.cell",
      "demo::Fsm");

  engine.setup();
  auto intro = engine.inspect_module("");

  //auto ctrl_bits = intro.get_bits("ctrl");
  //ir::Bitset exp_bits(std::string(
        //"0100000000000000000000000000000000000000000000000000000000000000000"
      //));

  //EXPECT_EQ(exp_bits, ctrl_bits);


  auto obj = intro.module()->objects.at("ctrl");
  auto ctrl_elem_bits = intro.get_element_bits("ctrl");
  ir::Bitset exp_bits(std::string("0"));
  auto idx = obj->type->elements.at("clk")->impl.struct_index;
  EXPECT_EQ(exp_bits, ctrl_elem_bits[idx]);

  exp_bits = ir::Bitset(std::string("1"));
  idx = obj->type->elements.at("reset")->impl.struct_index;
  EXPECT_EQ(exp_bits, ctrl_elem_bits[idx]);

  exp_bits = ir::Bitset(std::string(
        "0000000000000000000000000000000000000000000000000000000000000000"
      ));
  idx = obj->type->elements.at("op")->impl.struct_index;
  EXPECT_EQ(exp_bits, ctrl_elem_bits[idx]);

  exp_bits = ir::Bitset(std::string("0"));
  idx = obj->type->elements.at("en")->impl.struct_index;
  EXPECT_EQ(exp_bits, ctrl_elem_bits[idx]);

  auto named_elems = intro.get_named_element_bits("ctrl");
  for(auto const& it : named_elems) {
    LOG4CXX_INFO(m_logger, "" << it.first << ": " << it.second);
  }
  ASSERT_EQ(4, named_elems.size());

  exp_bits = ir::Bitset(std::string("0"));
  EXPECT_EQ(exp_bits, named_elems["clk"]);
  exp_bits = ir::Bitset(std::string("1"));
  EXPECT_EQ(exp_bits, named_elems["reset"]);
  exp_bits = ir::Bitset(std::string("0"));
  EXPECT_EQ(exp_bits, named_elems["en"]);
  exp_bits = ir::Bitset(std::string(
      "0000000000000000000000000000000000000000000000000000000000000000"
    ));
  EXPECT_EQ(exp_bits, named_elems["op"]);

  engine.teardown();
}


/* vim: set et ff=unix sts=2 sw=2 ts=2 : */
