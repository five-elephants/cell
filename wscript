#!/usr/bin/env python
# encoding: utf-8

def options(opt):
    opt.load('compiler_cxx')
    opt.load('bison')
    opt.load('boost')

def configure(conf):
    conf.load('compiler_cxx boost')
    conf.check_tool('bison flex')
    conf.check_boost(lib='program_options')

def build(bld):
    #gen/gen_text.cpp
    #gen/gen_cpp.cpp
    core_src = """
      scanner.l
      parser.yy
      gen/gen_m4.cpp
      ast/node_base.cpp
      ast/tree_base.cpp
      ast/variable_def.cpp
      ast/module_def.cpp
      ast/function_def.cpp
      ast/function_call.cpp
      ast/compound.cpp
      ast/if_statement.cpp
      ast/while_statement.cpp
      ast/bitstring_literal.cpp
      ast/unit.cpp
      parse_driver.cpp
      ir/namespace.cpp
      ir/builtins.cpp
    """

    #bld.program(
        #source = 'test_gen_text.cpp ' + core_src,
        #target = 'test_gen_text',
        #includes = '.',
        #cxxflags = '-std=c++11',
    #)

    bld.program(
        source = 'frontend.cpp ' + core_src,
        target = 'frontend',
        includes = '.',
        cxxflags = '-std=c++11 -ggdb',
        use = 'BOOST',
    )

    #bld.program(
        #source = 'calc++.cpp calc++-driver.cpp calc++-parser.yy calc++-scanner.l',
        #target = 'calc++',
        #includes = '.',
        #cxxflags = '-std=c++11 -ggdb',
    #)
