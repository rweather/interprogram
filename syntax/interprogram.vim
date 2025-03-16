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
syn match   ipType              "\<SYMBOLS FOR ROUTINES\>"
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
syn match   ipStatement         "\<CALL\>"
syn match   ipStatement         "\<RETURN\>"
syn match   ipStatement         "\<THEN\>"
syn match   ipStatement         "\<ELSE\>"
syn match   ipStatement         "\<ELSE IF\>"
syn match   ipStatement         "\<END IF\>"
syn match   ipStatement         "\<REPEAT WHILE\>"
syn match   ipStatement         "\<REPEAT FOR\>"
syn match   ipStatement         "\<REPEAT FOREVER\>"
syn match   ipStatement         "\<END REPEAT\>"
syn match   ipStatement         "\<TO\>"
syn match   ipStatement         "\<BY\>"
syn match   ipStatement         "\<AT END OF INPUT\>"
syn match   ipStatement         "\<ADD\>"
syn match   ipStatement         "\<SUBTRACT\>"
syn match   ipStatement         "\<MULTIPLY BY\>"
syn match   ipStatement         "\<DIVIDE BY\>"
syn match   ipStatement         "\<MODULO\>"
syn match   ipStatement         "\<COPY TAPE\>"
syn match   ipStatement         "\<IGNORE TAPE\>"

syn match   ipLibrary           "\<FORM SQUARE ROOT\>"
syn match   ipLibrary           "\<FORM SINE\>"
syn match   ipLibrary           "\<FORM COSINE\>"
syn match   ipLibrary           "\<FORM TANGENT\>"
syn match   ipLibrary           "\<FORM ARCTAN\>"
syn match   ipLibrary           "\<FORM SINE RADIANS\>"
syn match   ipLibrary           "\<FORM COSINE RADIANS\>"
syn match   ipLibrary           "\<FORM TANGENT RADIANS\>"
syn match   ipLibrary           "\<FORM ARCTAN RADIANS\>"
syn match   ipLibrary           "\<FORM SINE DEGREES\>"
syn match   ipLibrary           "\<FORM COSINE DEGREES\>"
syn match   ipLibrary           "\<FORM TANGENT DEGREES\>"
syn match   ipLibrary           "\<FORM ARCTAN DEGREES\>"
syn match   ipLibrary           "\<FORM NATURAL LOG\>"
syn match   ipLibrary           "\<FORM EXPONENTIAL\>"
syn match   ipLibrary           "\<FORM ABSOLUTE VALUE\>"
syn match   ipLibrary           "\<RAISE TO THE POWER OF\>"
syn match   ipLibrary           "\<BITWISE AND WITH\>"
syn match   ipLibrary           "\<BITWISE AND WITH NOT\>"
syn match   ipLibrary           "\<BITWISE OR WITH\>"
syn match   ipLibrary           "\<BITWISE XOR WITH\>"
syn match   ipLibrary           "\<BITWISE NOT\>"
syn match   ipLibrary           "\<SHIFT LEFT BY\>"
syn match   ipLibrary           "\<SHIFT RIGHT BY\>"
syn match   ipLibrary           "\<ROUND NEAREST\>"
syn match   ipLibrary           "\<ROUND UP\>"
syn match   ipLibrary           "\<ROUND DOWN\>"
syn match   ipLibrary           "\<ROUND TO A MULTIPLE OF\>"
syn match   ipLibrary           "\<RANDOM NUMBER\>"
syn match   ipLibrary           "\<SEED RANDOM\>"

syn match   ipLibrary           "\<TRIM STRING\>"
syn match   ipLibrary           "\<PAD STRING ON LEFT\>"
syn match   ipLibrary           "\<PAD STRING ON RIGHT\>"
syn match   ipLibrary           "\<NUMBER TO STRING\>"
syn match   ipLibrary           "\<STRING TO NUMBER\>"
syn match   ipLibrary           "\<STRING TO INTEGER\>"
syn match   ipLibrary           "\<STRING TO UPPERCASE\>"
syn match   ipLibrary           "\<STRING TO LOWERCASE\>"
syn match   ipLibrary           "\<STRING TO CHARACTER CODE\>"
syn match   ipLibrary           "\<CHARACTER CODE TO STRING\>"
syn match   ipLibrary           "\<SUBSTRING FROM\>"

syn match   ipLibrary           "\<CLEAR SCREEN\>"
syn match   ipLibrary           "\<CLEAR TO END OF SCREEN\>"
syn match   ipLibrary           "\<CLEAR TO END OF LINE\>"
syn match   ipLibrary           "\<MOVE CURSOR TO\>"
syn match   ipLibrary           "\<CURSOR X\>"
syn match   ipLibrary           "\<CURSOR Y\>"
syn match   ipLibrary           "\<SCREEN WIDTH\>"
syn match   ipLibrary           "\<SCREEN HEIGHT\>"
syn match   ipLibrary           "\<NORMAL TEXT\>"
syn match   ipLibrary           "\<BOLD TEXT\>"
syn match   ipLibrary           "\<TEXT COLOR\>"
syn match   ipLibrary           "\<BEEP SPEAKER\>"
syn match   ipLibrary           "\<FLASH SCREEN\>"
syn match   ipLibrary           "\<INPUT KEY\>"
syn match   ipLibrary           "\<DELETE CHARACTER\>"
syn match   ipLibrary           "\<DELETE LINE\>"
syn match   ipLibrary           "\<INSERT STRING\>"
syn match   ipLibrary           "\<INSERT LINE\>"
syn match   ipLibrary           "\<HIDE CURSOR\>"
syn match   ipLibrary           "\<SHOW CURSOR\>"

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
syn match   ipCondition         "\<IN\>"

syn match   ipFunction          "\<LENGTH OF\>"
syn match   ipFunction          "\<SINE OF\>"
syn match   ipFunction          "\<COSINE OF\>"
syn match   ipFunction          "\<TANGENT OF\>"
syn match   ipFunction          "\<ARCTAN OF\>"
syn match   ipFunction          "\<SQUARE ROOT OF\>"
syn match   ipFunction          "\<NATURAL LOG OF\>"
syn match   ipFunction          "\<EXPONENTIAL OF\>"
syn match   ipFunction          "\<ABSOLUTE VALUE OF\>"

syn match   ipConstant          /[0-9]*\.[0-9]*/
syn match   ipConstant          /[0-9]*\.[0-9]*[eE][0-9][0-9]*/
syn match   ipConstant          /[0-9]*\.[0-9]*[eE][-+][0-9][0-9]*/
syn match   ipConstant          /[0-9]*\.[0-9]*([0-9][0-9]*)/
syn match   ipConstant          /[0-9]*\.[0-9]*([-+][0-9][0-9]*)/
syn match   ipConstant          /[0-9][0-9]*/
syn match   ipConstant          /[0-9][0-9]*[eE][0-9][0-9]*/
syn match   ipConstant          /[0-9][0-9]*[eE][-+][0-9][0-9]*/
syn region  ipConstant          start=/PUNCH THE FOLLOWING CHARACTERS/ end=/\~\~\~\~\~*/

syn match   ipVariable          /@[1-9]/
syn match   ipVariable          "\<THIS\>"
syn match   ipVariable          "\<ARGV\>"
syn match   ipVariable          "\<BLACK\>"
syn match   ipVariable          "\<RED\>"
syn match   ipVariable          "\<GREEN\>"
syn match   ipVariable          "\<YELLOW\>"
syn match   ipVariable          "\<BLUE\>"
syn match   ipVariable          "\<MAGENTA\>"
syn match   ipVariable          "\<CYAN\>"
syn match   ipVariable          "\<WHITE\>"

syn match   ipString            /'[^']*'/
syn match   ipString            /"[^"]*"/

" Set the highlighting categories.
hi def link ipComment           Comment
hi def link ipBoundary          Type
hi def link ipType              Type
hi def link ipStatement         Statement
hi def link ipLibrary           Define
hi def link ipCondition         Type
hi def link ipFunction          Type
hi def link ipConstant          Number
hi def link ipString            String
hi def link ipVariable          Number

" Activate the language.
let b:current_syntax = "interprogram"
