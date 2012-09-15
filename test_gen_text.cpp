#include "ast/ast.h"
#include "gen/gen_text.h"

#include <iostream>

using namespace std;

int main() {
	auto a = ast::Literal<int>(1);
	auto b = ast::Literal<int>(2);
	auto c = ast::Literal<int>(3);
	auto im = ast::Op_minus(b, c);
	auto ast = ast::Op_plus(a, im);

	ast.visit();
	cout << endl;

	auto mod_if = ast::Identifier("mod_name");
	auto module = ast::Module_def(mod_if);
	auto var_id = ast::Identifier("var_name");
	auto type_id = ast::Identifier("int");
	auto var_exp = ast::Op_plus(a, c);
	auto var_assign = ast::Variable_def(var_id, type_id, var_exp);

	auto func_id = ast::Identifier("func_name");
	auto param_0 = ast::Variable_def(var_id, type_id, var_exp);
	auto param_1 = ast::Variable_def(var_id, type_id, var_exp);
	auto func_body_0 = ast::Variable_def(var_id, type_id, var_exp);
	auto func_def = ast::Function_def(func_id, type_id);

	func_def.append_parameter(param_0);
	func_def.append_parameter(param_1);
	func_def.append_body(func_body_0);

	module.append(func_def);
	module.append(var_assign);

	module.visit();
	cout << endl;

	return 0;
}
