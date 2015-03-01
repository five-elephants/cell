#pragma once

#include "sim/llvm_namespace.h"
#include "sim/runset.h"
#include "ir/find.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <llvm/IR/DataLayout.h>
#include <llvm/ExecutionEngine/JIT.h>


namespace sim {

  class Module_inspector {
    public:
      //Module_inspector();
      Module_inspector(std::shared_ptr<Llvm_module> module,
          llvm::StructLayout const* layout,
          unsigned num_elements,
          llvm::ExecutionEngine* exe,
          Runset& runset);

      /** get value of a member variable */
      template<typename T>
      T get(ir::Label const& var_name) {
        auto it = m_module->objects.find(var_name);
        if( it == m_module->objects.end() ) {
          std::stringstream strm;
          strm << "object '" << var_name << "' requested for introspection"
            " not found in module '"
            << m_module->name << "'";
          throw std::runtime_error(strm.str());
        }

        char* this_ptr = this_in->data();
        auto idx = it->second->impl.struct_index;
        auto ofs = m_layout->getElementOffset(idx);

        T rv;
        std::copy_n(this_ptr + ofs, sizeof(rv), reinterpret_cast<char*>(&rv));
        return rv;
      }

      /** get value of member variable by index */
      template<typename T>
      T get(std::size_t idx) {
        char* this_ptr = this_in->data();
        auto ofs = m_layout->getElementOffset(idx);

        T rv;
        std::copy_n(this_ptr + ofs, sizeof(rv), reinterpret_cast<char*>(&rv));
        return rv;
      }


      /** return the IR object struct */
      std::shared_ptr<Llvm_object> get_object(std::size_t idx) {
        auto it = std::find_if(std::begin(m_module->objects),
            std::end(m_module->objects),
            [&idx](std::pair<ir::Label,std::shared_ptr<Llvm_object>> x) {
              return x.second->impl.struct_index == idx;
            });

        if( it == std::end(m_module->objects) ) {
          std::stringstream strm;
          strm << "no element with index " << idx << " found in module";
          throw std::runtime_error(strm.str());
        }

        return it->second;
      }


      /** get the bit representation */
      ir::Bitset get_bits(std::size_t idx) {
        llvm::Type* ty = get_object(idx)->type->impl.type;
        auto lay = m_exe->getDataLayout();
        auto bit_sz = lay->getTypeSizeInBits(ty);
        auto alloc_bit_sz = lay->getTypeAllocSizeInBits(ty);
        auto byte_sz = bit_sz / 8;
        auto ofs = m_layout->getElementOffset(idx);

        ir::Bitset rv(alloc_bit_sz);
        char* ptr = this_in->data() + ofs;
        for(std::size_t i=0; i<alloc_bit_sz; i++) {
          auto bit_i = i % 8;
          rv[i] = (*ptr >> bit_i) & 1;

          if( bit_i == 7 )
            ++ptr;
        }

        rv.resize(bit_sz);
        return rv;
      }


      /** get the bit representation of the full module */
      ir::Bitset get_bits() {
        auto a = reinterpret_cast<ir::Bitset::block_type*>(
            this_in->data()
          );
        auto b = reinterpret_cast<ir::Bitset::block_type*>(
            this_in->data() + this_in->size() - 1
          );

        ir::Bitset rv(8 * this_in->size());
        boost::from_block_range(a, b, rv);

        return rv;
      }


      ir::Bitset get_bits(ir::Label const& var_name) {
        auto it = m_module->objects.find(var_name);
        if( it == m_module->objects.end() ) {
          std::stringstream strm;
          strm << "object '" << var_name << "' requested for introspection"
            " not found in module '"
            << m_module->name << "'";
          throw std::runtime_error(strm.str());
        }

        auto idx = it->second->impl.struct_index;
        return get_bits(idx);
      }


      /** get name from member index */
      ir::Label const& get_name(std::size_t idx) {
        return get_object(idx)->name;
      }


      /** get type name from member index */
      ir::Label const& get_type_name(std::size_t idx) {
        return get_object(idx)->type->name;
      }


      /** get number of member variables */
      unsigned num_elements() const { return m_num_elements; }


      /**
       * Directyl call a function in the module
       *
       * This version return a return value by reference as the first argument.
       * */
      template<typename Ret, typename ... Args>
      void call(Ret& ret, ir::Label const& name, Args ... args) {
        auto raw_func = get_function_ptr<Ret(char*,char*,char*,char*,Args...)>(name);

        std::copy(this_in->begin(), this_in->end(), this_out->begin());

        ret = raw_func(this_out->data(),
            this_in->data(),
            this_in->data(),
            read_mask->data(),
            args...);

        std::copy(this_out->begin(), this_out->end(), this_in->begin());
      }


      /**
       * Directly call a function in the module
       *
       * This version does not return anything.
       * */
      template<typename ... Args>
      void call(ir::Label const& name, Args ... args) {
        auto raw_func = get_function_ptr<void(char*,char*,char*,char*,Args...)>(name);
        auto this_out = m_runset.allocate_module_frame(m_module);
        auto read_mask = m_runset.allocate_read_mask(m_module);

        std::copy(this_in->begin(), this_in->end(), this_out->begin());

        raw_func(this_out->data(),
            this_in->data(),
            this_in->data(),
            read_mask->data(),
            args...);

        std::copy(this_out->begin(), this_out->end(), this_in->begin());
      }


      /** get a pointer to a function in the module */
      template<typename Func>
      Func* get_function_ptr(ir::Label const& name) {
        auto func = m_module->functions.at(name);
        void* ptr = m_exe->getPointerToFunction(func->impl.code);

        return reinterpret_cast<Func*>(ptr);
      }


      std::shared_ptr<Llvm_module> module() {
        return m_module;
      }


    private:
      std::shared_ptr<Llvm_module> m_module;
      llvm::StructLayout const* m_layout;
      unsigned m_num_elements;
      llvm::ExecutionEngine* m_exe;
      Runset& m_runset;
      Runset::Module_frame this_in, this_out;
      Runset::Read_mask read_mask;
  };

}
