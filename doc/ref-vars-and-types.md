# Variables and types

Variable names may be any combination of 1 or more letters, as long as
the name does not correspond to a single-word keyword.  Case is
not significant.

Classic INTERPROGRAM only considered the first four characters to be
significant, with the remainder ignored when comparing variable names:
<tt>ALPHA</tt> and <tt>ALPHB</tt> are the same variable.  My implementation
allows arbitrary-length names.

Classsic INTERPROGRAM only had two types: integer and floating-point.
Integer variables must be declared with `SYMBOLS FOR INTEGERS`.
Extended INTERPROGRAM also allows strings.  String values must be
declared with `SYMBOLS FOR STRINGS`.

Array variables may be subscripted with `X(n)` where `n` is an expression.

Strings may also be subscripted with `X(n)` where `n` is the 1-based
index into the string.  The character at `n` is returned as a new
single-character string.

Variables may be assigned with `SET` or `REPLACE`.  The value of a
variable may appear in any expression, or be taken into `THIS` with `TAKE`.

    SET J = 42
    SET K = J - 1

    TAKE K
    MULTIPLY BY 10
    REPLACE J

    SET HYPOT = SQUARE ROOT OF (X*X + Y*Y)

[Previous: Preliminary statements](ref-preliminary.md),
[Next: Mathematical operators](ref-math.md)
