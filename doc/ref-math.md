# Mathematical operators

In the statements below, <i>value</i> is a variable, constant,
or a reference to an array element.  Some examples:

    ADD X
    SUBTRACT 1.5
    MULTIPLY BY 2
    DIVIDE BY A(6)
    FORM SINE

Angles in the classic implementation for `FORM SINE`, `FORM COSINE`,
`FORM TANGENT`, and `FORM ARCTAN` are in units of π.  For example,
0.5 is π/2 radians or 90 degrees.  Extended INTERPROGRAM uses radians.

At startup, Extended INTERPROGRAM initialises the variable <tt>PI</tt> to π.
The value can be overwritten by the program if it needs the variable
<tt>PI</tt> for some other purpose.

<table border="1">
<tr><td><b>Statement</b></td><td><b>Description</b></td><td><b>Extension?</b></td></tr>
<tr><td><tt>ADD</tt> <i>value</i></td><td>Adds <i>value</i> to <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; performs string concatenation if used on string arguments.</td><td>String concatenation is an extension</td></tr>
<tr><td><tt>SUBTRACT</tt> <i>value</i></td><td>Subtracts <i>value</i> from <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>MULTIPLY BY</tt> <i>value</i></td><td>Multiplies the <i>value</i> with <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>DIVIDE BY</tt> <i>value</i></td><td>Divides <tt>THIS</tt> by <i>value</i>, leaving the quotient in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>MODULO</tt> <i>value</i></td><td>Divides <tt>THIS</tt> by <i>value</i>, leaving the remainder in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>FORM SQUARE ROOT</tt></td><td>Forms the square root of <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>FORM CUBE ROOT</tt></td><td>Forms the cube root of <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>FORM SINE</tt></td><td>Forms the sine of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in radians.</td><td> </td></tr>
<tr><td><tt>FORM COSINE</tt></td><td>Forms the cosine of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in radians.</td><td> </td></tr>
<tr><td><tt>FORM TANGENT</tt></td><td>Forms the tangent of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in radians.</td><td> </td></tr>
<tr><td><tt>FORM ARCTAN</tt></td><td>Forms the arctangent of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in radians.</td><td> </td></tr>
<tr><td><tt>FORM NATURAL LOG</tt></td><td>Forms the natural logorithm of <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>FORM LOGARITHM IN BASE</tt> <i>value</i></td><td>Forms the logorithm of <tt>THIS</tt> in base <i>value</i>, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>FORM EXPONENTIAL</tt></td><td>Raises <tt>THIS</tt> to the power of <i>e</i>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>FORM ABSOLUTE VALUE</tt></td><td>Forms the absolute value of <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>RAISE TO THE POWER OF</tt> <i>value</i></td><td>Raises <tt>THIS</tt> to the power of <i>value</i>, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>BITWISE AND WITH</tt> <i>value</i></td><td>Performs a bitwise AND of integers <tt>THIS</tt> and <i>value</i>, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>BITWISE AND WITH NOT</tt> <i>value</i></td><td>Performs a bitwise AND of integers <tt>THIS</tt> and NOT <i>value</i>, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>BITWISE OR WITH</tt> <i>value</i></td><td>Performs a bitwise OR of integers <tt>THIS</tt> and <i>value</i>, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>BITWISE XOR WITH</tt> <i>value</i></td><td>Performs a bitwise exclusive-OR of integers <tt>THIS</tt> and <i>value</i>, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>BITWISE NOT</tt></td><td>Performs a bitwise NOT of integer <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>SHIFT LEFT BY</tt> <i>value</i></td><td>Performs an arithmetic left shift of the integer <tt>THIS</tt> by <i>value</i> bits, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>SHIFT RIGHT BY</tt> <i>value</i></td><td>Performs an arithmetic right shift of the integer <tt>THIS</tt> by <i>value</i> bits, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>ROUND NEAREST</tt></td><td>Rounds <tt>THIS</tt> to the nearest integer, rounding halfway cases away from zero.  Same as the round() function in C.</td><td>Yes</td></tr>
<tr><td><tt>ROUND UP</tt></td><td>Rounds <tt>THIS</tt> to an integer, rounding up.  Same as the ceil() function in C.</td><td>Yes</td></tr>
<tr><td><tt>ROUND DOWN</tt></td><td>Rounds <tt>THIS</tt> to an integer, rounding down.  Same as the floor() function in C.</td><td>Yes</td></tr>
<tr><td><tt>ROUND TO A MULTIPLE OF</tt> <i>value</i></td><td>Rounds <tt>THIS</tt> to a mutiple of <i>value</i>, rounding halfway cases away from zero.  A division by zero error will occur if <i>value</i> is zero.</td><td>Yes</td></tr>
</table>

Algebraic expressions in the extension syntax can use the following operators:

<table border="1">
<tr><td><b>Priority</b></td><td><b>Operator</b></td><td><b>Description</b></td></tr>
<tr><td>1</td><td>(<i>value</i>)</td><td>Parenthesised expression</td></tr>
<tr><td>2</td><td>-<i>value</i>, +<i>value</i></td><td>Unary negation or unary plus</td></tr>
<tr><td>2</td><td><tt>LENGTH OF</tt> <i>value</i></td><td>Length of a string or array</td></tr>
<tr><td>2</td><td><tt>SINE OF</tt> <i>value</i></td><td>Sine of the angle <i>value</i></td></tr>
<tr><td>2</td><td><tt>COSINE OF</tt> <i>value</i></td><td>Cosine of the angle <i>value</i></td></tr>
<tr><td>2</td><td><tt>TANGENT OF</tt> <i>value</i></td><td>Tangent of the angle <i>value</i></td></tr>
<tr><td>2</td><td><tt>ARCTAN OF</tt> <i>value</i></td><td>Arctangent of <i>value</i></td></tr>
<tr><td>2</td><td><tt>SQUARE ROOT OF</tt> <i>value</i></td><td>Square root of <i>value</i></td></tr>
<tr><td>2</td><td><tt>CUBE ROOT OF</tt> <i>value</i></td><td>Cube root of <i>value</i></td></tr>
<tr><td>2</td><td><tt>NATURAL LOG OF</tt> <i>value</i></td><td>Natural logarithm of <i>value</i></td></tr>
<tr><td>2</td><td><tt>EXPONENTIAL OF</tt> <i>value</i></td><td>Raises <i>e</i> to the power of <i>value</i></td></tr>
<tr><td>2</td><td><tt>ABSOLUTE VALUE OF</tt> <i>value</i></td><td>Absolute value of <i>value</i></td></tr>
<tr><td>3</td><td><i>value1</i> * <i>value2</i>, <i>value1</i> / <i>value2</i>, <i>value1</i> <tt>MODULO</tt> <i>value2</i></td><td>Multiplication, division, or remainder</td></tr>
<tr><td>4</td><td><i>value1</i> + <i>value2</i>, <i>value1</i> - <i>value2</i></td><td>Addition or subtraction</td></tr>
</table>

In the extended language, the following classic computation:

    TAKE X
    MULTIPLY BY THIS
    ADD Y
    FORM SQUARE ROOT
    REPLACE Z

Can be written as:

    SET Z = SQUARE ROOT OF (X * X + Y)

[Previous: Variables and types](ref-vars-and-types.md),
[Next: String operations](ref-strings.md)
