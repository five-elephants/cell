#!/usr/bin/env python
# encoding: utf-8

def options(opt):
    opt.load('compiler_cxx compiler_c')
    opt.load('flex')
    opt.load('bison')
    opt.load('boost')
    opt.load('swig')
    #opt.load('python')

def configure(conf):
    conf.load('compiler_cxx compiler_c boost bison flex swig')
    #conf.load('python')
    #conf.check_tool('bison flex')
    conf.check_boost(lib='program_options serialization')
    #conf.check_python_headers()
    conf.check(lib='pthread', uselib_store='PTHREAD')
    #conf.check(lib='gtest', uselib_store='GTEST')
    #conf.check(lib='gtest_main', uselib_store='GTEST_MAIN')
    #conf.check(header_name='gtest/gtest.h')
    conf.check(lib='log4cxx', uselib_store='LOG4CXX')
    conf.check(header_name='log4cxx/log4cxx.h', uselib_store='LOG4CXX')
    for llvm_config in [ 'llvm-config', 'llvm-config-3.4' ]:
      res = conf.check_cfg(
        path=llvm_config,
        args='--cppflags --includedir --ldflags --libs core jit native',
        package='',
        uselib_store='LLVM',
        mandatory=False
      )
      if res != None:
        break

    if res == None:
      conf.fatal('Can not find llvm-config program')
    #print conf.env.LIB_LLVM
    #print conf.env.INCLUDES_LLVM

def build(bld):
    #gen/gen_text.cpp
    #gen/gen_cpp.cpp
    core_src = """
      scanner.l
      parser.yc
      gen/gen_m4.cpp
      ast/node_base.cpp
      ast/tree_base.cpp
      ast/identifier.cpp
      ast/variable_def.cpp
      ast/module_def.cpp
      ast/namespace_def.cpp
      ast/function_def.cpp
      ast/function_call.cpp
      ast/function_param.cpp
      ast/compound.cpp
      ast/if_statement.cpp
      ast/while_statement.cpp
      ast/bitstring_literal.cpp
      ast/unit.cpp
      ast/socket_item.cpp
      ast/socket_def.cpp
      ast/connection_item.cpp
      ast/module_instantiation.cpp
      ast/process.cpp
      ast/assignment.cpp
      ast/variable_ref.cpp
      ast/array_type.cpp
      parse_driver.cpp
    """

    sim_src = """
      sim/llvm_namespace_scanner.cpp
      sim/llvm_module_scanner.cpp
      sim/llvm_function_scanner.cpp
      sim/llvm_namespace.cpp
      sim/llvm_builtins.cpp
      sim/runset.cpp
      sim/module_inspector.cpp
      sim/stream_instrumenter.cpp
      sim/vcd_instrumenter.cpp
      sim/simulation_engine.cpp
      sim/compile.cpp
      sim/runtime.cpp
    """

    #sim_src = """
      #sim/llvm_namespace_scanner.cpp
      #sim/llvm_function_scanner.cpp
      #sim/llvm_codegen.cpp
      #sim/llvm_codeblock.cpp
      #sim/codegen_visitor.cpp
      #sim/module_codegen_visitor.cpp
      #sim/scan_ast.cpp
      #sim/compile.cpp
      #sim/runtime.cpp
      #sim/simulation_engine.cpp
      #sim/module_inspector.cpp
      #sim/stream_instrumenter.cpp
    #"""

    bindings_src = """
      bindings/ir.swig
      bindings/api.cpp
    """

    gtest_src = """
      gtest/gtest-1.7.0/src/gtest-all.cc
    """

    test_src = """
      gtest/gtest-1.7.0/src/gtest_main.cc
      test/test_find_hierarchy.cpp
      test/test_scanner_base.cpp
      test/test_simple_sim.cpp
      test/test_codegen.cpp
      test/test_demos.cpp
      test/test_module_inspector.cpp
    """
    #test/test_simple_sim.cpp

    bld.objects(
      source = core_src,
      target = 'core',
      includes = '.',
      cxxflags = '-fPIC -std=c++11 -ggdb',
      use = 'BOOST',
    )

    bld.objects(
      source = sim_src,
      target = 'sim',
      includes = '.',
      cxxflags = '-fPIC -std=c++11 -ggdb',
      use = 'BOOST LLVM LOG4CXX'
    )

    #bld.program(
        #source = 'frontend.cpp ',
        #target = 'frontend',
        #includes = '.',
        #cxxflags = '-std=c++11 -ggdb',
        #use = 'core',
    #)

    bld.program(
      source = 'sim/compiler.cpp',
      target = 'compiler',
      includes = '.',
      cxxflags = '-std=c++11 -ggdb',
      use = 'core sim LLVM'
    )

    #bld.program(
      #source = 'sim/simulator.cpp',
      #target = 'simulator',
      #includes = '.',
      #cxxflags = '-std=c++11 -ggdb -fPIC',
      #use = 'core sim LLVM'
    #)

    if False:
      bindings_lib = bld(
        source = bindings_src,
        target = 'ir',
        swig_flags = '-lua -c++',
        includes = '.',
        cxxflags = '-std=c++11',
        features = 'swig cxx cxxshlib',
        use = 'core'
      )
      bindings_lib.env.cxxshlib_PATTERN = '%s.so'

      bindings_lib = bld(
        source = bindings_src,
        target = '_ir',
        swig_flags = '-python -c++',
        includes = '.',
        cxxflags = '-std=c++11',
        features = 'swig pyext cxx cxxshlib',
        use = 'core'
      )
      #bindings_lib.env.cxxshlib_PATTERN = '%s.so'

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
      source = test_src,
      target = 'test-main',
      cxxflags = '-std=c++11 -ggdb',
      includes = [
        'gtest/gtest-1.7.0/include',
        '.',
      ],
      use = 'core sim gtest LLVM',
    )


