#!/usr/bin/env python
# encoding: utf-8

from waflib.Tools import waf_unit_test

def options(opt):
    opt.load('compiler_cxx compiler_c')
    #opt.load('clang clangxx')
    opt.load('flex')
    opt.load('bison')
    opt.load('boost')
    opt.load('doxygen')
    opt.load('waf_unit_test')

def configure(conf):
    conf.load('compiler_cxx compiler_c boost bison flex')
    #conf.load('clang clangxx boost bison flex')
    conf.check_boost(lib='program_options serialization system filesystem')
    conf.check(lib='pthread', uselib_store='PTHREAD')
    conf.check(lib='log4cxx', uselib_store='LOG4CXX')
    conf.check(header_name='log4cxx/log4cxx.h', uselib_store='LOG4CXX')
    for llvm_config in [ 'llvm-config', 'llvm-config-3.4' ]:
      res = conf.check_cfg(
        path=llvm_config,
        args='--cppflags --includedir --ldflags --system-libs --libs core jit native',
        package='',
        uselib_store='LLVM',
        mandatory=False
      )
      if res != None:
        break

    if res == None:
      conf.fatal('Can not find llvm-config program')

    conf.load('doxygen')
    conf.load('waf_unit_test')

def build(bld):
    core_src = """
      src/parsing/scanner.l
      src/parsing/parser.yc
      src/ast/node_base.cpp
      src/ast/tree_base.cpp
      src/ast/identifier.cpp
      src/ast/variable_def.cpp
      src/ast/module_def.cpp
      src/ast/module_template.cpp
      src/ast/namespace_def.cpp
      src/ast/function_def.cpp
      src/ast/function_call.cpp
      src/ast/function_param.cpp
      src/ast/compound.cpp
      src/ast/if_statement.cpp
      src/ast/while_expression.cpp
      src/ast/for_expression.cpp
      src/ast/bitstring_literal.cpp
      src/ast/unit.cpp
      src/ast/socket_item.cpp
      src/ast/socket_def.cpp
      src/ast/connection_item.cpp
      src/ast/module_instantiation.cpp
      src/ast/process.cpp
      src/ast/assignment.cpp
      src/ast/variable_ref.cpp
      src/ast/array_type.cpp
      src/parsing/parse_driver.cpp
    """

    sim_src = """
      src/sim/llvm_namespace_scanner.cpp
      src/sim/llvm_module_scanner.cpp
      src/sim/llvm_function_scanner.cpp
      src/sim/llvm_constexpr_scanner.cpp
      src/sim/llvm_namespace.cpp
      src/sim/llvm_builtins.cpp
      src/sim/runset.cpp
      src/sim/module_inspector.cpp
      src/sim/stream_instrumenter.cpp
      src/sim/vcd_instrumenter.cpp
      src/sim/simulation_engine.cpp
      src/sim/compile.cpp
      src/sim/runtime.cpp
    """

    gtest_src = """
      gtest/gtest-1.7.0/src/gtest-all.cc
    """

    test_src = """
      gtest/gtest-1.7.0/src/gtest_main.cc
      src/test/test_find_hierarchy.cpp
      src/test/test_scanner_base.cpp
      src/test/test_simple_sim.cpp
      src/test/test_codegen.cpp
      src/test/test_demos.cpp
      src/test/test_module_inspector.cpp
    """

    flags = {
      'cxxflags': '-fPIC -std=c++11 -O0 -ggdb -DBOOST_FILESYSTEM_NO_DEPRECATED',
      'includes': [ 'src', 'src/parsing' ],
    }

    bld.objects(
      source = core_src,
      target = 'core',
      use = 'BOOST',
      **flags
    )

    bld.objects(
      source = sim_src,
      target = 'sim',
      use = 'BOOST LLVM LOG4CXX',
      **flags
    )

    bld.program(
      source = 'src/sim/cellsim.cpp',
      target = 'cellsim',
      use = 'core sim LLVM',
      **flags
    )

    bld.objects(
      source = gtest_src,
      target = "gtest",
      includes = [
        "gtest/gtest-1.7.0/include",
        "gtest/gtest-1.7.0/",
      ],
      use = 'PTHREAD'
    )

    bld.program(
      features = 'test',
      source = test_src,
      target = 'test-main',
      includes = [
        'gtest/gtest-1.7.0/include',
      ] + flags['includes'],
      use = 'core sim gtest LLVM',
      install_path = None,
      cxxflags = flags['cxxflags']
    )
    bld.add_post_fun(waf_unit_test.summary)

    bld(
        features = 'doxygen',
        doxyfile = 'doc/doxygen/Doxyfile',
    )


