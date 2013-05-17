define(`module', `
class $1 {
$2
};
')dnl

define(`variable_def', 
`$1 $2 = $3;
')dnl

define(`variable_decl',
`$1 $2;
')dnl

define(`function_def',
`$2 $1($3) $4')dnl

define(`param_decl', `$1 $2')

define(`compound',
`{
$1}
')dnl


dnl 
dnl operators
dnl
define(`plus', `$1 + $2')
define(`minus', `$1 - $2')
