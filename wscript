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
    #conf.check_boost(lib='serialization')
    conf.check_python_headers()

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
      parse_driver.cpp
      ir/namespace.cpp
      ir/builtins.cpp
      ir/analyze.cpp
      ir/streamop.cpp
      ir/namespace_scanner.cpp
      ir/module_scanner.cpp
      ir/scan_ast.cpp
    """

    bindings_src = """
      bindings/ir.swig
      bindings/api.cpp
    """

    bld.objects(
      source = core_src,
      target = 'core',
      includes = '.',
      cxxflags = '-fPIC -std=c++11 -ggdb',
      use = 'BOOST',
    )

    bld.program(
        source = 'frontend.cpp ',
        target = 'frontend',
        includes = '.',
        cxxflags = '-std=c++11 -ggdb',
        use = 'core',
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

