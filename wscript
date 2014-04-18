#!/usr/bin/env python
# encoding: utf-8

def options(opt):
    opt.load('compiler_cxx compiler_c')
    opt.load('flex')
    opt.load('bison')
    opt.load('boost')
    opt.load('swig')
    opt.load('python')

def configure(conf):
    conf.load('compiler_cxx compiler_c boost bison flex swig')
    conf.load('python')
    #conf.check_tool('bison flex')
    conf.check_boost(lib='program_options serialization')
    conf.check_python_headers()
    conf.check(lib='gtest', uselib_store='GTEST')
    conf.check(lib='gtest_main', uselib_store='GTEST_MAIN')
    conf.check(header_name='gtest/gtest.h')
    conf.check_cfg(
      path='llvm-config',
      args='--cppflags --includedir --ldflags --libs',
      package='',
      uselib_store='LLVM'
    )
    print conf.env.LIB_LLVM
    print conf.env.INCLUDES_LLVM

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
      parse_driver.cpp
      ir/namespace.cpp
      ir/builtins.cpp
      ir/analyze.cpp
      ir/streamop.cpp
      ir/namespace_scanner.cpp
      ir/module_scanner.cpp
      ir/scan_ast.cpp
      ir/codegen.cpp
      ir/codeblock.cpp
    """

    sim_src = """
      sim/llvm_codegen.cpp
      sim/llvm_codeblock.cpp
      sim/compile.cpp
    """

    bindings_src = """
      bindings/ir.swig
      bindings/api.cpp
    """

    test_src = """
      test/test_find_hierarchy.cpp
    """

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
      use = 'BOOST LLVM'
    )

    bld.program(
        source = 'frontend.cpp ',
        target = 'frontend',
        includes = '.',
        cxxflags = '-std=c++11 -ggdb',
        use = 'core',
    )

    bld.program(
      source = 'sim/compiler.cpp',
      target = 'compiler',
      includes = '.',
      cxxflags = '-std=c++11 -ggdb',
      use = 'core sim LLVM'
    )

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

    bld.program(
      source = test_src,
      target = 'test-main',
      cxxflags = '-std=c++11',
      includes = '.',
      use = 'core GTEST GTEST_MAIN',
    )


