#include "parse_driver.h"

Parse_driver::Parse_driver()
	:	m_trace_parsing(false),
		m_trace_scanning(false),
		m_ast_root(nullptr),
    m_default_namespace("default") {
}

Parse_driver::~Parse_driver() {
}

int
Parse_driver::parse(std::string const& fn) {
	m_filename = fn;
	scan_begin();
	yy::Parser parser(*this);
	parser.set_debug_level(m_trace_parsing);
	int res = parser.parse();
	scan_end();

	return res;
}

void
Parse_driver::error(yy::location const& loc, std::string const& m) {
	std::cerr << loc << " : " << m << std::endl;
}

void
Parse_driver::error(std::string const& m) {
	std::cerr << m << std::endl;
}
