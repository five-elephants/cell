#pragma once

#include "gen/generator_if.h"
#include "gen/macros.h"
#include <ostream>
#include <sstream>

namespace gen {

#define OUTFILE_HEADER "include(backend/cpp.m4)dnl\n"

	DECL_GENERATOR_CLASS(M4_generator) {
		GENERATOR_CLASS_BODY(M4_generator)

		bool within_function_params = false;

		template<typename T>
		std::string m4_escape(T const& x) {
			std::stringstream strm;
			strm << x;
			return '`' + strm.str() + '\'';
		}
	};

#undef OUTFILE_HEADER

}
