#include "ast/ast.h"
#include "gen/text/gen_text.h"

#include <iostream>

using namespace std;

/*
 * This approach does not work, because it requires knowledge of the tree-structure at
 * compile time.
 * */

int main() {
	auto a = ast::Literal<int>(5);
	auto b = ast::Literal<int>(2);
	auto c = ast::Literal<int>(7);
	//auto ast = ast::Op_plus<ast::Literal<int>,ast::Literal<int>>(a,b);
	auto ast = ast::make_op_plus(a, ast::make_op_minus(b, c));

	{
		using namespace gen_text;
		cout << "AST:\n" << ast << endl;
	}

	return 0;
}
