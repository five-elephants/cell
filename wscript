#!/usr/bin/env python
# encoding: utf-8

def options(opt):
    opt.load('compiler_cxx')
    opt.load('bison')

def configure(conf):
    conf.load('compiler_cxx')
    conf.check_tool('bison flex')

def build(bld):
    core_src = """
      gen/gen_text.cpp
      ast/node_base.cpp
      ast/variable_def.cpp
      ast/module_def.cpp
      ast/function_def.cpp
    """

    bld.program(
        source = 'test_gen_text.cpp ' + core_src,
        target = 'test_gen_text',
        includes = '.',
        cxxflags = '-std=c++11',
    )

    #bld.program(
        #source = 'calc++.cpp calc++-driver.cpp calc++-parser.yy calc++-scanner.l',
        #target = 'calc++',
        #includes = '.',
        #cxxflags = '-std=c++11 -ggdb',
    #)
