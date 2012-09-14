#include "ast/ast.h"
#include "gen/gen_text.h"

#include <iostream>

using namespace std;

int main() {
	gen::Text_generator g(cout);

	auto a = ast::Literal<int>(1, g);
	auto b = ast::Literal<int>(2, g);
	auto c = ast::Literal<int>(3, g);
	auto im = ast::Op_minus(b, c, g);
	auto ast = ast::Op_plus(a, im, g);

	ast.visit();
	cout << endl;

	auto mod_if = ast::Identifier("mod_name", g);
	auto module = ast::Module_def(mod_if, g);
	auto var_id = ast::Identifier("var_name", g);
	auto type_id = ast::Identifier("int", g);
	auto var_exp = ast::Op_plus(a, c, g);
	auto var_assign = ast::Var_assign(var_id, type_id, var_exp, g);

	auto func_id = ast::Identifier("func_name", g);
	auto param_0 = ast::Var_assign(var_id, type_id, var_exp, g);
	auto param_1 = ast::Var_assign(var_id, type_id, var_exp, g);
	auto func_body_0 = ast::Var_assign(var_id, type_id, var_exp, g);
	auto func_def = ast::Function_def(func_id, g);

	func_def.append_parameter(param_0);
	func_def.append_parameter(param_1);
	func_def.append_body(func_body_0);

	module.append(func_def);
	module.append(var_assign);

	module.visit();
	cout << endl;

	return 0;
}
