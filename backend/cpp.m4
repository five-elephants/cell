divert(-1)
define(`module', `
class $1 {
$2
};
')

define(`variable_def', `$1 $2 = $3;')
define(`variable_decl', `$1 $2;')

define(`function_def', `$2 $1(`$3') { $4 }')

define(`param_list', `$@')
define(`param_decl', `$1 $2')

define(`compound',
`{
$@
} 
')dnl

define(`type', `$1')

define(`if_else_statement', `if( `$1' ) `$2'else `$3'')
define(`if_statement', `if( `$1' ) `$2'')
define(`while_statement', `while( `$1' ) `$2'')
define(`return_statement', `return $1')

dnl 
dnl operators
dnl
define(`plus', `$1 + $2')
define(`minus', `$1 - $2')
define(`mult', `$1 * $2')
define(`div', `$1 / $2')
define(`equal', `$1 == $2')
define(`not_equal', `$1 != $2')
define(`greater_then', `$1 > $2')
define(`lesser_then', `$1 < $2')
define(`greater_or_equal_then', `$1 >= $2')
define(`lesser_or_equal_then', `$1 <= $2')

divert(0)
