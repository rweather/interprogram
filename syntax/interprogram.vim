" Syntax highlighting for INTERPROGRAM

" Quit when a syntax file was already loaded.
if exists("b:current_syntax")
  finish
endif

" INTERPROGRAM is case-insensitive.
syn case ignore

" Match syntactic elements.
syn match   ipComment           /TITLE.*$/
syn match   ipComment           /#.*$/

syn match   ipBoundary          "\<COMPILE THE FOLLOWING INTERPROGRAM\>"
syn match   ipBoundary          "\<END OF INTERPROGRAM\>"
syn match   ipBoundary          "\<EXIT INTERPROGRAM\>"

syn match   ipType              "\<SYMBOLS FOR INTEGERS\>"
syn match   ipType              "\<SYMBOLS FOR STRINGS\>"
syn match   ipType              "\<MAXIMUM SUBSCRIPTS\>"
syn match   ipType              "\<NONE\>"

syn match   ipStatement         "&"
syn match   ipStatement         "\<TAKE\>"
syn match   ipStatement         "\<IF\>"
syn match   ipStatement         "\<REPLACE\>"
syn match   ipStatement         "\<INPUT\>"
syn match   ipStatement         "\<OUTPUT\>"
syn match   ipStatement         "\<SET\>"
syn match   ipStatement         "\<GO TO\>"
syn match   ipStatement         "\<EXECUTE PROCESS\>"
syn match   ipStatement         "\<END OF PROCESS DEFINITION\>"
syn match   ipStatement         "\<REPEAT FROM\>"
syn match   ipStatement         "\<TIMES\>"
syn match   ipStatement         "\<PAUSE\>"
syn match   ipStatement         "\<COPY TAPE\>"
syn match   ipStatement         "\<IGNORE TAPE\>"
syn match   ipStatement         "\<CALL\>"
syn match   ipStatement         "\<RETURN\>"

syn match   ipOperator          "\<ADD\>"
syn match   ipOperator          "\<SUBTRACT\>"
syn match   ipOperator          "\<MULTIPLY BY\>"
syn match   ipOperator          "\<DIVIDE BY\>"
syn match   ipOperator          "\<MODULO\>"
syn match   ipOperator          "\<FORM SQUARE ROOT\>"
syn match   ipOperator          "\<FORM SINE\>"
syn match   ipOperator          "\<FORM COSINE\>"
syn match   ipOperator          "\<FORM TANGENT\>"
syn match   ipOperator          "\<FORM ARCTAN\>"
syn match   ipOperator          "\<FORM SINE RADIANS\>"
syn match   ipOperator          "\<FORM COSINE RADIANS\>"
syn match   ipOperator          "\<FORM TANGENT RADIANS\>"
syn match   ipOperator          "\<FORM ARCTAN RADIANS\>"
syn match   ipOperator          "\<FORM SINE DEGREES\>"
syn match   ipOperator          "\<FORM COSINE DEGREES\>"
syn match   ipOperator          "\<FORM TANGENT DEGREES\>"
syn match   ipOperator          "\<FORM ARCTAN DEGREES\>"
syn match   ipOperator          "\<FORM NATURAL LOG\>"
syn match   ipOperator          "\<FORM EXPONENTIAL\>"
syn match   ipOperator          "\<FORM ABSOLUTE\>"
syn match   ipOperator          "\<RAISE TO THE POWER OF\>"
syn match   ipOperator          "\<BITWISE AND WITH\>"
syn match   ipOperator          "\<BITWISE AND WITH NOT\>"
syn match   ipOperator          "\<BITWISE OR WITH\>"
syn match   ipOperator          "\<BITWISE XOR WITH\>"
syn match   ipOperator          "\<BITWISE NOT\>"
syn match   ipOperator          "\<SHIFT LEFT BY\>"
syn match   ipOperator          "\<SHIFT RIGHT BY\>"
syn match   ipOperator          "\<LENGTH OF\>"
syn match   ipOperator          "\<SUBSTRING FROM\>"
syn match   ipOperator          "\<TO\>"

syn match   ipCondition         "\<IS\>"
syn match   ipCondition         "\<IS NOT\>"
syn match   ipCondition         "\<GREATER THAN\>"
syn match   ipCondition         "\<MUCH GREATER THAN\>"
syn match   ipCondition         "\<SMALLER THAN\>"
syn match   ipCondition         "\<MUCH SMALLER THAN\>"
syn match   ipCondition         "\<ZERO\>"
syn match   ipCondition         "\<POSITIVE\>"
syn match   ipCondition         "\<NEGATIVE\>"
syn match   ipCondition         "\<EQUAL TO\>"
syn match   ipCondition         "\<GREATER THAN OR EQUAL TO\>"
syn match   ipCondition         "\<SMALLER THAN OR EQUAL TO\>"
syn match   ipCondition         "\<FINITE\>"
syn match   ipCondition         "\<INFINITE\>"
syn match   ipCondition         "\<A NUMBER\>"
syn match   ipCondition         "\<EMPTY\>"

syn match   ipConstant          "\<THIS\>"
syn match   ipConstant          /[0-9]*\.[0-9]*/
syn match   ipConstant          /[0-9]*\.[0-9]*[eE][0-9][0-9]*/
syn match   ipConstant          /[0-9]*\.[0-9]*[eE][-+][0-9][0-9]*/
syn match   ipConstant          /[0-9]*\.[0-9]*([0-9][0-9]*)/
syn match   ipConstant          /[0-9]*\.[0-9]*([-+][0-9][0-9]*)/
syn match   ipConstant          /[0-9][0-9]*/
syn match   ipConstant          /[0-9][0-9]*[eE][0-9][0-9]*/
syn match   ipConstant          /[0-9][0-9]*[eE][-+][0-9][0-9]*/
syn region  ipConstant          start=/PUNCH THE FOLLOWING CHARACTERS/ end=/\~\~\~\~\~*/

syn match   ipString            /'[^']*'/
syn match   ipString            /"[^"]*"/

" Set the highlighting categories.
hi def link ipComment           Comment
hi def link ipBoundary          Type
hi def link ipType              Type
hi def link ipStatement         Statement
hi def link ipOperator          Operator
hi def link ipCondition         Conditional
hi def link ipConstant          Number
hi def link ipString            String

" Activate the language.
let b:current_syntax = "interprogram"
