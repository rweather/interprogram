INTERPROGRAM Language Reference
===============================

INTERPROGRAM was developed by Geoff Hill in 1960 for the
[CSIRAC](https://en.wikipedia.org/wiki/CSIRAC), Australia's first
electronic stored program computer and the only first-generation
computer to survive intact to this day.

This page describes the "Extended INTERPROGRAM" syntax that is implemented
by the code in this repository.  The original language will be referred
to as "Classic INTERPROGRAM" below.

The original or "classic" syntax is documented in the
[original manual](INTERMAN.pdf).  For a history of CSIRAC, please see
[this page](CSIRAC-history.md).

Extending the language is intentional.  I wanted to explore what the
language may have become had it survived and continued to evole over time.
All programming languages that are in active use evolve to incorporate
new features and to fix "warts" in the original language.

## Introduction to "Programming in English"

INTERPROGRAM was advertised as "Programming in English".  But what does
that mean?  Clearly natural language processing was beyond the capabilities
of the CSIRAC.

Let's take a simple computation, z = x<sup>2</sup> + y, and write out the
primitive steps of the computation in plain English:

1. Take the value of x.
2. Multiply it by itself.
3. Add the value of y.
4. Replace the contents of the variable z with the result.

This is how you would perform the computation with pencil and paper.
What does this look like in INTERPROGRAM?

    TAKE X
    MULTIPLY BY THIS
    ADD Y
    REPLACE Z

It looks almost the same as the English description!  This is the key to
INTERPROGRAM's syntactic style - take the straight-forward set of steps in
English and write them out as closely as possible in the program.

Of course, the compiler is a little stricter on syntax and semantics than
plain English, but the style is to make the program read the way you would
perform the operations with pencil and paper.

The "THIS" variable is special in that it refers to the result of the
last computation; essentially an accumulator.  "TAKE" loads a value into
"THIS" and "REPLACE" copies the contents of "THIS" to a destination.

Here is a longer example that calculates the hypotenuse of a right-angled
triangle, h = sqrt(a<sup>2</sup> + b<sup>2</sup>):

    TAKE A
    MULTIPLY BY THIS
    REPLACE SUM
    TAKE B
    MULTIPLY BY THIS
    ADD SUM
    FORM SQUARE ROOT
    REPLACE H

A temporary variable <tt>SUM</tt> is needed for this computation.

INTERPROGRAM uses multi-word keywords like "MULTIPLY BY" and
"FORM SQUARE ROOT" as they are more readable than "MUL" or "SQRT".
It also makes the language easier to parse when trying to determine
if an identifier is a variable or a keyword:

    TAKE FORM           # 'FORM' is an ordinary variable
    FORM SQUARE ROOT    # Statement keyword

Say we want to multiply X by several variables A, B, C, and D.  We could do
this:

    TAKE X
    MULTIPLY BY A
    MULTIPLY BY B
    MULTIPLY BY C
    MULTIPLY BY D

But that is long-winded.  English to the rescue with the ampersand:

    TAKE X, MULTIPLY BY A, & B, & C, & D

"&" means "repeat the last operation with a new operand".  In English,
the above code reads natually as "take X, multiply by A and B and C and D".
Also note the use of comma as a statement separator, which is more
natural in English than a semi-colon.

"&" can only be used on the same line as the statement that is being
repeated.  A new line starts fresh with a new statement.

## Controlling the verbosity

The previous examples are verbose, although very clear as to what
operations are happening.  Can we do better?

The classic language has a variable assignment feature using `SET`.
Floating-point variables could be assigned a constant:

    SET X = 23
    SET X = -499.999
    SET A(3) = 27.5

Integer variables could be assigned constants or the result of some
simple algebraic forms:

    SET J = 23          SET K = -499
    SET J = J + 5       SET K = K - 99
    SET J = K           SET J = -K

That's it!  This is probably due to the limitations of the CSIRAC.
There wasn't enough memory to implement a full algebraic expression
parser.  That forced Geoff Hill to come up with other approaches.

My implementation of INTERPROGRAM allows simple algebraic expressions
involving numeric addition, subtraction, multiplication, division,
and negation.  The previous hypotenuse example can be rewritten
as follows:

    TAKE A*A + B*B
    FORM SQUARE ROOT
    REPLACE H

Algebraic expressions can be used in other contexts:

    SET SUMOFSQUARES = A*A + B*B
    IF A * B IS POSITIVE, SET J = 42

## Identifiers and constants

Identifiers are a sequence of one or more alphabetic letters between A and Z.
Upper and lower case may be used, but the interpreter will force everything
to upper case.

It is not possible to use digits in a variable name; e.g. "A3".  Instead,
put the value in an array and use a subscripted term like "A(3)".

Integer constants are one or more decimal digits, optionally preceded by a
minus sign:

    0
    123
    -89

Floating-point constants have decimal points or exponents, such as:

    0.123
    1.5e27
    -60
    4.567e-10

In the classic language, the "e" notation didn't exist.  Instead parentheses
were used for the exponent:

    1.5(27)
    4.567(-10)

This implementation accepts either the "e" notation or the parenthesised
notation, although the "e" notation is preferred.

Strings didn't exist in the classic language; I have added them to
my implementation.  Strings may appear in either single or double quotes:

    'abc'
    "Hello, World!\n"
    'abc''def'          # Final string: abc'def

The single quoted form is preferable as double quotes did not exist in the
CSIRAC's original character set.

Some limited C-style escape sequences are available:

<table border="1">
<tr><td><b>Escape</b></td><td><b>ASCII Character</b></td><td><b>Decimal</b></td><td><b>Hexadecimal</b></td></tr>
<tr><td>\a</td><td>Bell</td><td>7</td><td>07</td></tr>
<tr><td>\b</td><td>Backspace</td><td>8</td><td>08</td></tr>
<tr><td>\t</td><td>Tab</td><td>9</td><td>09</td></tr>
<tr><td>\n</td><td>Newline</td><td>10</td><td>0A</td></tr>
<tr><td>\v</td><td>Vertical Tab</td><td>11</td><td>0B</td></tr>
<tr><td>\f</td><td>Form Feed</td><td>12</td><td>0C</td></tr>
<tr><td>\r</td><td>Carriage Return</td><td>13</td><td>0D</td></tr>
<tr><td>\e</td><td>Escape</td><td>27</td><td>1B</td></tr>
</table>

All other \\c escapes are the same as the character c.  Octal and
hexadecimal character escapes are not supported.

## Character set

In 1960, CSIRAC was using a Flexowriter teletype for character-based
input and output.  The Flexowriter used a 6-bit code, encoded into 5 bits
using "shift" symbols to switch between two sets of symbols.

The first set of symbols consisted of letters and control characters:

    blank A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
    space carriage-return figure-shift letter-shift erase

The second set of symbols consisted of figures, punctuation, and control
characters:

    blank 0 1 2 3 4 5 6 7 8 9 + - * / ( ) # = & , . ' x £ stop tab
    space carriage-return figure-shift letter-shift erase

The "blank" symbol is different from "space".  Blanks are positions on the
punch tape with no holes punched other than the sprocket hole down the
middle.  In ASCII we would call this a "NUL".  Blanks are used as padding
on the tape between sections of data.

The "erase" symbol is a tape position with all holes punched.  In those
days if you wanted to erase a punched symbol, you would back up the
tape and punch "erase" over the top.

My implementation of INTERPROGRAM uses ASCII instead of Flexowriter codes.
We do need a mapping for "blank" however, as INTERPROGRAM uses it as
part of its syntax to separate sections.  We use "~" as that character
didn't exist on the Flexowriter.  It won't appear in normal programs
except in comments or strings.

ASCII doesn't have an equivalent to "erase".  Easiest is to remove the
erased characters entirely when converting a Flexowriter tape to ASCII.

Classic INTERPROGRAM used "stop" to allow the entry of large multi-tape
programs and data.  The programmer would put a "stop" code on the end of
one tape, which paused the system waiting for the operator to load the
next tape onto the tape reader.  We use the end-of-file indicator
for "stop" instead.

## Hello, World!

Here is the complete source code for the traditional Hello World example:

    (1) TITLE HELLO WORLD EXAMPLE
    (2) SYMBOLS FOR INTEGERS NONE
    (4) COMPILE THE FOLLOWING INTERPROGRAM
    PUNCH THE FOLLOWING CHARACTERS
    HELLO, WORLD
    ~~~~~
    END OF INTERPROGRAM

As stated above, blank symbols are replaced with "~" in ASCII.  At least
five blanks are needed to indicate the end of the text after
`PUNCH THE FOLLOWING CHARACTERS`.

Here is a complete program from the original manual, including the
input data:

     (1)  TITLE  ALPHA = BETA - 27.394 + A(3).(X/Y)
     (2)  SYMBOLS FOR INTEGERS   J
     (3)  MAXIMUM SUBSCRIPTS   A(3)
     (4)  COMPILE THE FOLLOWING INTERPROGRAM

     *1   INPUT J
     *2   IF THIS IS ZERO, GO TO *7     # END WHEN J = 0
     *3   INPUT BETA, & A(3), & X, & Y
     *4   TAKE X, DIVIDE BY Y, MULTIPLY BY A(3)
          SUBTRACT 27.394, ADD BETA, REPLACE ALPHA
          TAKE J, OUTPUT, OUTPUT ALPHA
     *6   GO TO *1
     *7   END OF INTERPROGRAM
     ~~~~~
     1   25.382    2.754      7     0.92
     2   31.542   -1.832      9     1.24
     3   22.379   -0.023     11     2.57
     4   23.456   +0.001     23     0.17
     5   27.356   +1.113      3   -95.76
     0

This program produces the following output:

     1  18.9423
     2  -9.14877
     3  -5.11344
     4  -3.80271
     5  -0.0728684

## Preliminary statements

The program begins with several numbered "preliminary statements" to declare
global information about the program.

`(1) TITLE` gives a title to the program, which was traditionally written
to the program's printout so that the machine operators knew who to give
the output to.  The title extends to the end of the line.

`(2) SYMBOLS FOR INTEGERS` declares all of the identifiers that will be
used for integer values in the program.  In the classic language,
all other symbols were implicitly declared as floating-point.
`NONE` indicates that there are no integer variables.  The symbol
names may be separated by spaces or commas:

    (2) SYMBOLS FOR INTEGERS J K BETA, B, C

In the classic language, up to 8 integer variables could be declared
on a single line.  My implementation allows an arbitrary number, with
multiple `(2) SYMBOLS FOR INTEGERS` lines if necessary.  My implementation
also allows string variables to be declared:

    (2) SYMBOLS FOR STRINGS NAME, ADDRESS

`(3) MAXIMUM SUBSCRIPTS` is used to declare the size of any arrays that
will be used by the program:

    (3) MAXIMUM SUBSCRIPTS A(3), X(37), J(-5)

Array A has indexes 0, 1, 2, and 3; X has indexes 0 to 37; and J has
indexes 0, -1, -2, -3, -4, and -5.  The original manual implied that
subscript ranges were possible; e.g. -5 to +3; using "dummy symbols".
My implementation lists the range explicitly, with the end-points
separated by a colon:

    (3) MAXIMUM SUBSCRIPTS A(-5:+3)

In the classic language, integer array elements had to be declared
individually:

    (2) SYMBOLS FOR INTEGERS K(0) K(1) K(2) K(3)

My implementation takes a two-step approach.  The symbol is declared
as an integer first, and then promoted to be an array of integers:

    (2) SYMBOLS FOR INTEGERS K
    (3) MAXIMUM SUBSCRIPTS K(3)

The last preliminary statement must be `(4) COMPILE THE FOLLOWING INTERPROGRAM`.

## Reserved words

Reserved words are any that correspond to a single-word keyword:

    ADD         INPUT       POSITIVE    TIMES
    CALL        IS          REPLACE     TITLE
    ELSE        MODULO      RETURN      TO
    EMPTY       NEGATIVE    SET         ZERO
    FINITE      NONE        SUBTRACT
    IF          OUTPUT      THEN
    INFINITE    PAUSE       THIS

Reserved words cannot be used as variable or label names.

## Variables and types

Variable names may be any combination of 1 or more letters, as long as
the name does not correspond to a single-word keyword.  Case is
not significant.

Classic INTERPROGRAM only considered the first four characters to be
significant, with the remainder ignored when comparing variable names.
My implementation allows arbitrary-length names.

Classsic INTERPROGRAM only had two types: integer and floating-point.
Integer variables must be declared with `(2) SYMBOLS FOR INTEGERS`.
Extended INTERPROGRAM also allows strings.  String values must be
declared with `(2) SYMBOLS FOR STRINGS`.

Array variables may be subscripted with `X(n)` where `n` is an expression.

Strings may also be subscripted with `X(n)` where `n` is the 1-based
index into the string.  The character at `n` is returned as a new
single-character string.

Variables may be assigned with `SET` or `REPLACE`.  The value of a
variable may appear in any expression, or be taken into `THIS` with `TAKE`.

## Mathematical operators

In the statements below, <i>value</i> is a variable, constant,
or a reference to an array element.  Some examples:

    ADD X
    SUBTRACT 1.5
    MULTIPLY BY 2
    DIVIDE BY A(6)
    FORM SINE

Angles in the classic implementation for `FORM SINE`, `FORM COSINE`,
`FORM TANGENT`, and `FORM ARCTAN` were in units of π.  For example,
0.5 is π/2 radians or 90 degrees.  The extended implementation
provides alternative statements for angles in radians and degrees.

<table border="1">
<tr><td><b>Statement</b></td><td><b>Description</b></td><td><b>Extension?</b></td></tr>
<tr><td><tt>ADD</tt> <i>value</i></td><td>Adds <i>value</i> to <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; performs string concatenation if used on string arguments.</td><td>String concatenation is an extension</td></tr>
<tr><td><tt>SUBTRACT</tt> <i>value</i></td><td>Subtracts <i>value</i> from <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>MULTIPLY BY</tt> <i>value</i></td><td>Multiplies the <i>value</i> with <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>DIVIDE BY</tt> <i>value</i></td><td>Divides <tt>THIS</tt> by <i>value</i>, leaving the quotient in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>MODULO</tt> <i>value</i></td><td>Divides <tt>THIS</tt> by <i>value</i>, leaving the remainder in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>FORM SQUARE ROOT</tt></td><td>Forms the square root of <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>FORM SINE</tt></td><td>Forms the sine of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in units of π.</td><td> </td></tr>
<tr><td><tt>FORM COSINE</tt></td><td>Forms the cosine of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in units of π.</td><td> </td></tr>
<tr><td><tt>FORM TANGENT</tt></td><td>Forms the tangent of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in units of π.</td><td> </td></tr>
<tr><td><tt>FORM ARCTAN</tt></td><td>Forms the arctangent of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in units of π.</td><td> </td></tr>
<tr><td><tt>FORM SINE RADIANS</tt></td><td>Forms the sine of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in radians.</td><td>Yes</td></tr>
<tr><td><tt>FORM COSINE RADIANS</tt></td><td>Forms the cosine of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in radians.</td><td>Yes</td></tr>
<tr><td><tt>FORM TANGENT RADIANS</tt></td><td>Forms the tangent of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in radians.</td><td>Yes</td></tr>
<tr><td><tt>FORM ARCTAN RADIANS</tt></td><td>Forms the arctangent of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in radians.</td><td>Yes</td></tr>
<tr><td><tt>FORM SINE DEGREES</tt></td><td>Forms the sine of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in degrees.</td><td>Yes</td></tr>
<tr><td><tt>FORM COSINE DEGREES</tt></td><td>Forms the cosine of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in degrees.</td><td>Yes</td></tr>
<tr><td><tt>FORM TANGENT DEGREES</tt></td><td>Forms the tangent of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in degrees.</td><td>Yes</td></tr>
<tr><td><tt>FORM ARCTAN DEGREES</tt></td><td>Forms the arctangent of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in degrees.</td><td>Yes</td></tr>
<tr><td><tt>FORM NATURAL LOG</tt></td><td>Forms the natural logorithm of <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>FORM EXPONENTIAL</tt></td><td>Raises <tt>THIS</tt> to the power of <i>e</i>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>FORM ABSOLUTE</tt></td><td>Forms the absolute value of <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td>Yes</td></tr>
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
</table>

Algebraic expressions in the extension syntax use the following operators
with precedence from highest to lowest:

<table border="1">
<tr><td><b>Operator</b></td><td><b>Description</b></td></tr>
<tr><td>(<i>value</i>)</td><td>Parenthesised expression</td></tr>
<tr><td><tt>LENGTH OF</tt> <i>value</i></td><td>Length of a string or array</td></tr>
<tr><td>-<i>value</i>, +<i>value</i></td><td>Unary negation or unary plus</td></tr>
<tr><td><i>value1</i> \* <i>value2</i>, <i>value1</i> / <i>value2</i>, <i>value1</i> <tt>MODULO</tt> <i>value2</i></td><td>Multiplication, division, or remainder</td></tr>
<tr><td><i>value1</i> + <i>value2</i>, <i>value1</i> - <i>value2</i></td><td>Addition or subtraction</td></tr>
</table>

## String operations

<tt>ADD</tt> and "+" can be used to perform string concatenation in
Extended INTERPROGRAM.  The following extension statements are also available:

<table border="1">
<tr><td><b>Statement</b></td><td><b>Description</b></td><td><b>Extension?</b></td></tr>
<tr><td><tt>LENGTH OF</tt></td><td>Length of the string in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>NUMBER TO STRING</tt></td><td>Converts the numeric value in <tt>THIS into a string</tt></td><td>Yes</td></tr>
<tr><td><tt>PAD STRING ON LEFT</tt> <i>size</i></td><td>Pads the string in <tt>THIS</tt> to <i>size</i> characters by padding the value on the left with spaces.</td><td>Yes</td></tr>
<tr><td><tt>PAD STRING ON RIGHT</tt> <i>size</i></td><td>Pads the string in <tt>THIS</tt> to <i>size</i> characters by padding the value on the right with spaces.</td><td>Yes</td></tr>
<tr><td><tt>STRING TO INTEGER</tt> [<i>base</i>]</td><td>Converts the string in <tt>THIS into an integer.  The optional <i>base</i> indicates the base for teh conversion between 2 and 26.  The default base is 10.  The special <i>base</i> of 0 indicates to recognise C-style decimal, hexadecimal, and octal values.</tt></td><td>Yes</td></tr>
<tr><td><tt>STRING TO NUMBER</tt></td><td>Converts the string in <tt>THIS into a floating-point value</tt></td><td>Yes</td></tr>
<tr><td><tt>SUBSTRING FROM</tt> <i>index1</i> [<tt>TO</tt> <i>index2</i>]</td><td>Extracts a substring from <tt>THIS</tt> starting at <i>index1</i> and ending at <i>index2</i>.  If <i>index2</i> is not present, then the rest of the string starting at <i>index1</i> is the result.  String indexes are 1-based.</td><td>Yes</td></tr>
<tr><td><tt>TRIM STRING</tt></td><td>Trims whitespace from the start and end of <tt>THIS</tt></td><td>Yes</td></tr>
</table>

## Control flow statements

The most basic kind of control flow is `IF ... GO TO`:

        IF X IS POSITIVE, GO TO *7
        SET X = -X
    *7

The next section lists the allowable conditions for `IF`.

Classic INTERPROGRAM supported numeric labels between 1 and 150.
This implementation supports numeric labels between 1 and 9999, and also
supports any number of named labels:

        IF X IS POSITIVE, GO TO NAME
        SET X = -X
    *NAME

When using a named label, the "\*" is omitted in the `GO TO` statement,
to distinguish it from a computed `GO TO`:

        SET J = 50
        GO TO *J+5
    *50 *52 *54 *56
        OUTPUT 'EVEN', GO TO DONE
    *51 *53 *55 *57
        OUTPUT 'ODD'
    *DONE

As can be seen, multiple labels may appear on a line.

Subroutines in INTERPROGRAM are called "routine processes".  The following
example is from the original manual and defines the "ARC-COSINE" function
on `THIS`:

    *100 # PROCESS DEFINED FOR ARC-COSINE(THIS)
         REPLACE ARGUMENT, MULTIPLY BY THIS, & -1 , ADD 1
         FORM SQUARE ROOT, DIVIDE BY ARGUMENT, FORM ARCTAN
         END OF PROCESS DEFINITION

    TAKE 0.5
    EXECUTE PROCESS *100
    REPLACE ANSWER

The keywords `EXECUTE PROCESS` and `END OF PROCESS DEFINITION` are a
little obnoxious and make it uncomfortable to use subroutines.
The extended syntax supports the more natural `CALL` and `RETURN` as
aliases for subroutine operations:

    *ARCCOS
         REPLACE ARGUMENT, MULTIPLY BY THIS, & -1 , ADD 1
         FORM SQUARE ROOT, DIVIDE BY ARGUMENT, FORM ARCTAN
         RETURN

    TAKE 0.5
    CALL ARCCOS
    REPLACE ANSWER

See <b>Defining new statements</b> below for another method to call
subroutines without needing the `CALL` keyword.

Classic INTERPROGRAM had a single looping construct called `REPEAT FROM`:

    (2) SYMBOLS FOR INTEGERS J
    (3) MAXIMUM SUBSCRIPTS A(19)

        SET TOTAL = 0
        SET J = 19
    *22 TAKE A(J), ADD TOTAL, REPLACE TOTAL
        REPEAT FROM *22 J TIMES
        TAKE TOTAL, DIVIDE BY 20, REPLACE AVERAGE

Note that although J is set to 19 initially, the loop body is actually
executed 20 times.  The `REPEAT FROM` statement is interpreted as:

1. If J is zero, then exit the loop.
2. Otherwise, decrement J and jump to the top of the loop.

If J is zero initially, then the loop body is executed once and then stops.
If J is 1 initially, then the loop body is executed twice and then stops.
It would have been more correct to write the statement this way:

    REPEAT FROM *22 J MORE TIMES

Loop control variables can also start off negative and increment
towards zero:

        SET J = -19
    *22 TAKE A(19 + J), ADD TOTAL, REPLACE TOTAL
        REPEAT FROM *22 J TIMES

In the previous example, the array was traversed from the last element to
the first.  In this example, the array is traversed from the first element to
the last.

## Conditions

The table below lists the positive form of the condition using `IS`, such as:

    X IS GREATER THAN Y

Each condition also has a negative form using the extension `IS NOT`:

    X IS NOT GREATER THAN Y

<table border="1">
<tr><td><b>Expression</b></td><td><b>Description</b></td><td><b>Extension?</b></td></tr>
<tr><td><tt><i>value</i> IS ZERO</tt></td><td>Condition is true if <i>value</i> is equal to zero<sup>(see note 1)</sup></i></td><td> </td></tr>
<tr><td><tt><i>value</i> IS POSITIVE</tt></td><td>Condition is true if <i>value</i> is positive or zero</td><td> </td></tr>
<tr><td><tt><i>value</i> IS NEGATIVE</tt></td><td>Condition is true if <i>value</i> is negative</td><td> </td></tr>
<tr><td><tt><i>value1</i> IS EQUAL TO <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is equal to <i>value2</i></td><td>Yes</td></tr>
<tr><td><tt><i>value1</i> IS GREATER THAN <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is greater than <i>value2</i></td><td> </td></tr>
<tr><td><tt><i>value1</i> IS MUCH GREATER THAN <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is much greater than <i>value2</i><sup>(see note 2)</sup></td><td> </td></tr>
<tr><td><tt><i>value1</i> IS GREATER THAN OR EQUAL TO <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is greater than or equal to <i>value2</i></td><td>Yes</td></tr>
<tr><td><tt><i>value1</i> IS SMALLER THAN <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is smaller than <i>value2</i></td><td> </td></tr>
<tr><td><tt><i>value1</i> IS MUCH SMALLER THAN <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is much smaller than <i>value2</i><sup>(see note 2)</sup></td><td> </td></tr>
<tr><td><tt><i>value1</i> IS SMALLER THAN OR EQUAL TO <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is smaller than or equal to <i>value2</i></td><td>Yes</td></tr>
<tr><td><tt><i>value</i> IS FINITE</tt></td><td>Condition is true if <i>value</i> is a finite value<sup>(see note 3)</sup></td><td>Yes</td></tr>
<tr><td><tt><i>value</i> IS INFINITE</tt></td><td>Condition is true if <i>value</i> is an infinite value<sup>(see note 3)</td><td>Yes</td></tr>
<tr><td><tt><i>value</i> IS A NUMBER</tt></td><td>Condition is true if <i>value</i> is not the special "not a number" value<sup>(see note 3)</td><td>Yes</td></tr>
<tr><td><tt><i>value</i> IS EMPTY</tt></td><td>Condition is true if <i>value</i> is an empty string</td><td>Yes</td></tr>
</table>

Notes:

1. When floating-point values are compared with zero, they are actually
checked for being "very close to zero", below 1x10<sup>-20</sup>.
This makes comparisons against zero safer when using floating-point.
If you want exact comparison with zero, then use `IF X IS EQUAL TO 0`.
2. Classic INTERPROGRAM interpreted "much greater than" and "much smaller than"
as roughly "1 million times greater" or "1 million times smaller", as that
was the precision of the CSIRAC's floating-point representation.
My implementation uses a scale of 1 million also, despite modern
floating-point representations having better precision.  Probably best
to avoid `IS MUCH GREATER THAN` and `IS MUCH SMALLER THAN` in new code.
3. My implementation assumes that the host machine's floating-point
representation is compatible with IEEE 754, with that standard's notions
of infinities and not-a-numbers.  Integer values are always finite.

## Input and Output

<tt>INPUT</tt> <i>variable</i>

Reads a value from the program's input stream into <i>variable</i> and
<tt>THIS</tt>.  The type of value read will depend upon the type of
<i>variable</i>: integer, floating-point, or string.  If <i>variable</i> is
omitted, then a floating-point value will be read into <tt>THIS</tt> only.

If end of file is encountered, then the next statement on the current
line will be skipped.  In the following example, control will go to
label 1 if a value is read, or to label 2 if end of file is encountered:

    INPUT X, GO TO *1, GO TO *2

Reading integers or floating-point values will first skip whitespace,
and then read the digits of the number.  If the number is followed by a
newline, then the newline will be skipped.

Reading string values will read all characters until the next newline.
The newline character will not be included in the returned string.

Input data can be embedded at the end of the program after "~~~~~":

    ...
    END OF INTERPROGRAM
    ~~~~~
    1 1 2 3 5 8 13

The interpreter will read the embedded input data until end of file,
and then switch over to using regular input.  Either standard input or a
file specified by <tt>--input</tt> on the command-line.  Two end of
files will be reported; the first for the embedded input data and the
second for the regular input.

<tt>OUTPUT</tt> <i>value</i>

Outputs <i>value</i> to the program's output stream.  If <i>value</i>
is not followed by a comma, then a newline will also be output.
This can be useful for formatting the output:

    OUTPUT 'The answer is: ', OUTPUT ANSWER

If <tt>value</tt> is omitted, then <tt>THIS</tt> will be output
with special formatting to right-align the value in a fixed-width field.
Use <tt>OUTPUT THIS</tt> if you don't want the special formatting.

<tt>COPY TAPE</tt>

Copies characters from the input stream until "~~~~~" is seen.

<tt>IGNORE TAPE</tt>

Ignores characters from the input stream until "~~~~~" is seen.

## Subroutine arguments and local variables

Subroutines in the classic syntax can take a single argument in the
<tt>THIS</tt> variable, but everything else must be passed in
global variables.

The extension syntax supports up to 9 subroutine arguments with the special
variable names <tt>@1</tt>, <tt>@2</tt>, ..., <tt>@9</tt>.

The following example defines a function that computes the reciprocal
of its argument and returns the answer in <tt>THIS</tt>:

    CALL RECIP 3

    *RECIP
        TAKE 1.0
        DIVIDE BY @1
        RETURN

If the subroutine takes more than one argument, separate them with colons:

    CALL MYFUNC X : Y + 2 : Z * 6 + W

Unused arguments are set to "uninitialised" and will give an error
when read.

Argument variables, even unused ones, can be assigned.  This gives the
subroutine a method to store local variables.  The original manual
contained this example for computing the arc-cosine of the value in
<tt>THIS</tt>:

    *100
        REPLACE ARGUMENT, MULTIPLY BY THIS, & -1, ADD 1
        FORM SQUARE ROOT, DIVIDE BY ARGUMENT, FORM ARCTAN
        END OF PROCESS DEFINITION

This required the use of a temporary global variable called <tt>ARGUMENT</tt>.
In the extended syntax, we can do this instead:

    *ACOS
        REPLACE @9, MULTIPLY BY THIS, & -1, ADD 1
        FORM SQUARE ROOT, DIVIDE BY @9, FORM ARCTAN
        RETURN

This allows the extended language to more easily support recursion.
There is however a limit of 9 local variables.

## Structured programming

Using <tt>IF</tt> and <tt>GO TO</tt> for control flow is difficult in
more complicated applications.  Extended INTERPROGRAM provides a true
<tt>IF ... THEN ... ELSE</tt> construct:

    IF X IS NEGATIVE THEN
        SET X = -X
    END IF

    IF X IS NEGATIVE THEN, SET X = -X, END IF

    IF X IS GREATER THAN Y THEN
        SET MAX = X
    ELSE
        SET MAX = Y
    END IF

This can be extended to multiple <tt>ELSE IF</tt> conditions if needed:

    IF condition1 THEN
        ...
    ELSE IF condition2 THEN
        ...
    ELSE IF condition3 THEN
        ...
    ELSE
        ...
    END IF

Note that the <tt>ELSE</tt> and <tt>IF</tt> must be on the same line
to make it part of the original <tt>IF</tt> statement.  If they are on
different lines, then the <tt>IF</tt> starts a nested <tt>IF</tt> statement:

    IF condition1 THEN
        ...
    ELSE
        IF condition2 THEN
            ...
        END IF
    END IF

While loops are accomplished with the <tt>REPEAT WHILE</tt> construct:

    REPEAT WHILE X IS LESS THAN 10
        ...
        SET X = X + 1
    END REPEAT

Currently there is no equivalent of <tt>continue</tt> or <tt>break</tt>
from C.  However, it is possible to get the same effect with <tt>GO TO</tt>:

    REPEAT WHILE X IS LESS THAN 10
        IF X IS EQUAL TO 5 THEN
            SET X = X + 2
            GO TO CONTINUE
        ELSE IF STOP IS NOT ZERO THEN
            GO TO BREAK
        END IF
        ...
        SET X = X + 1
    *CONTINUE
    END REPEAT
    *BREAK

## Defining new statements

The subroutine syntax allows new functionality to be provided under a
convenient name.  But it is a little annoying to have to use the
<tt>EXECUTE PROCESS</tt> or <tt>CALL</tt> keywords.

It would be nice if we could add new kinds of statements to the language.
The extended syntax allows this: first declare that the name is a routine
with <tt>SYMBOLS FOR ROUTINES</tt>:

    (2) SYMBOLS FOR ROUTINES ARCCOS

Then the name of the subroutine can be used directly as though it was a
statement:

    TAKE 0.5
    ARCCOS      # <-----
    REPLACE ANSWER

    *ARCCOS
         REPLACE @9, MULTIPLY BY THIS, & -1 , ADD 1
         FORM SQUARE ROOT, DIVIDE BY @9, FORM ARCTAN
         RETURN

The <tt>CALL</tt> keyword is added implicitly by the language parser.

But even this isn't perfect.  It would be better to use <tt>FORM ARCCOS</tt>
to match the built-in <tt>FORM ARCTAN</tt> statement.  This is possible
using a string when the routine name is declared:

    (2) SYMBOLS FOR ROUTINES 'FORM ARCCOS'

    TAKE 0.5
    FORM ARCCOS     # <-----
    REPLACE ANSWER

    *FORM ARCCOS
         REPLACE @9, MULTIPLY BY THIS, & -1 , ADD 1
         FORM SQUARE ROOT, DIVIDE BY @9, FORM ARCTAN
         RETURN

Note that routine names defined in this way must not conflict with
built-in keywords.

This is how we can add new statements to the language in the program.

## Random numbers

Extended INTERPROGRAM has two statements for working with random numbers.

<tt>RANDOM NUMBER</tt>

Sets <tt>THIS</tt> to a random floating-point value between 0 (inclusive)
and 1 (exclusive).  For example, to generate a random integer between
1 and 10:

    (2) SYMBOLS FOR INTEGERS J

    RANDOM NUMBER
    MULTIPLY BY 10
    ADD 1
    REPLACE J

<tt>SEED RANDOM</tt> <i>value</i>

Seeds the random number generator using the integer <i>value</i>.
This can be used for repeatable random sequences.  Normally the
random number generator is seeded from the system time at startup.

This implementation uses the standard C rand() and srand() functions to
generate random numbers.  The quality of the random numbers will depend
upon the underlying system's C library.  The application can expect to
see no more than 2^31 distinct random values.

## Other statements

<tt>END OF INTERPROGRAM</tt>

End the program with an exit status of 0.  In the classic implementation,
this could only appear as the last statement in the program.  This
implementation allows <tt>END OF INTERPROGRAM</tt> to appear anywhere.
If the program is not terminated with <tt>END OF INTERPROGRAM</tt>,
then its presence is implied.

<tt>EXIT INTERPROGRAM</tt>

End the program with an exit status of <tt>THIS</tt> (0-255).
If <tt>THIS</tt> is not in the range 0 to 255, then the exit status will be 1.
This is an extension.

<tt>PAUSE</tt> <i>value</i>

In the classic implementation, <tt>PAUSE</tt> would suspend execution
showing <i>value</i> on the CSIRAC's neons.  Pressing 'Restart' would cause
the program to continue from the next statement.  <tt>PAUSE</tt> was used
as a primitive breakpoint mechanism, with <i>value</i> indicating how
far through the program execution has gotten.  This implementation evaluates
<i>value</i>, outputs it, and then continues execution immediately.

## Classic syntax

The original or "classic" syntax is documented in the
[original manual](doc/INTERMAN.pdf).

If you provide the `--classic` or `-c` option to the interpreter,
it will attempt to implement only the classic subset.  There are some
slight differences with the actual classic language due to my implementation
choices.

The "blank" symbol from the original 5-hole punch tape is replaced by the
ASCII tilde "~".

`TITLE` is a single-line statement in this version, terminated by a newline.
In the original version, the title had to be terminated with at least
five blank characters, to allow for multi-line titles.  This was
error-prone so I simplified it.

The data that was input to the program was typically listed after the
program itself on the punch tape.  A "stop" symbol could be put on the
tape to prompt the loading of another tape for large amounts of input.

In this implementation, you can put data at the end of the program by
separating it from the program code with `~~~~~`.  Or the input can be
provided in a text file using the `--input` or `-i` option to the interpreter.
If neither are provided, the input comes from standard input.

The original implementation allowed simple expressions like `J + 5` in some
contexts like the right-hand side of `SET` statements and in a computed
`GO TO`.  It is unclear whether such expressions could be used in other
contexts like `IF` conditions or the index expression for array access.
This implementation allows such expressions in any "r-value" context.

The original implementation allowed only 8 integer variables but an
arbitrary number of floating-point variables (up to the memory
limitations of the CSIRAC).  This implementation allows an arbitrary
number of each type.

The original implementation only considered the first 4 characters of a
variable name significant.  `ALPHA` and `ALPHB` were the same variable.
This implementation allows an arbitrary number of characters, all of
which must be alphabetic.  Digits cannot be used in variable names
in either the classic or extended syntax; it is necessary to use
array subscripts like `A(3)`.

When using `PUNCH THE FOLLOWING CHARACTERS` or `COPY TAPE`, the
terminating `~~~~~` will be printed to the output when using the
classic syntax.  This replicates the original behaviour on punch tape,
which allowed the output of one program to be easily fed into another program.
The terminator can be suppressed by putting a comma after the statement:

    PUNCH THE FOLLOWING CHARACTERS,
    THIS IS A MESSAGE
    ~~~~~

    COPY TAPE,

The blanks are always omitted when using the extended syntax.  No need
for a comma.

The original implementation terminated input data with `END` or `#`.
This implementation uses the end of file (EOF) condition instead.
Existing input data tapes will need to be converted to remove `END`/`#`,
to replace blank symbols with "~", and to remove any "erase" or "stop"
symbols that were originally present.

The original floating-point library on the CSIRAC was not very good.
If you compare the output of the examples in the original manual with
what my interpreter produces, you will sometimes see different answers.
Doing the calculations by hand reveals that the original is accurate to
about 3 decimal places and then diverges.  I have not attempted to
replicate the original floating-point behaviour.  The native IEEE 754
"double" type on the host computer is used instead.

The original integer representation was signed 10-bit.  This implementation
uses signed 64-bit integers instead.

In the original implementation, integer array elements had to be declared
individually:

    (2) SYMBOLS FOR INTEGERS K(0) K(1) K(2) K(3)

My implementation takes a two-step approach.  The symbol is declared
as an integer first, and then promoted to be an array of integers:

    (2) SYMBOLS FOR INTEGERS K
    (3) MAXIMUM SUBSCRIPTS K(3)

`OUTPUT` statements formatted right-aligned numeric data in 8 spaces.
This implementation uses 15 as it gives nicer results with the greater
integer and floating-point precision used.  This may change the appearance
of the output for existing programs.

## `END OF INTERPROGRAM` language reference
