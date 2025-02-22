INTERPROGRAM - Australia's First High-Level Programming Language
================================================================

INTERPROGRAM was developed by Geoff Hill in 1960 for the
[CSIRAC](https://en.wikipedia.org/wiki/CSIRAC), Australia's first
electronic stored program computer and the only first-generation
computer to survive intact to this day.

INTERPROGRAM is an interpreted high-level programming language, advertised as
"English Language for Automatic Programming".  The syntax is very easy
for beginners to programming to learn.  It predated
[BASIC](https://en.wikipedia.org/wiki/BASIC) by three years.

## What is this project?

There are CSIRAC emulators available on the Internet that can run the
original INTERPROGRAM environment, but the CSIRAC was very limited.  See
[this page](doc/CSIRAC-history.md) for the technical specifications
and history.

I was curious what a modern implementation of INTERPROGRAM would look like,
to explore the language and its possibilities.

What if INTERPROGRAM had continued to be developed by the Australian
computer community?  What if it had become the standard "BASIC" for Australia?
What might it have looked like after several decades of improvement?

## Manual

The original [manual for the INTERPROGRAM language](doc/INTERMAN.pdf)
is in this repository for reference.  The original version of the
manual came in Word format from [this CSIRAC emulator](https://cis.unimelb.edu.au/about/csirac/emulator).

I have made some editorial improvements to the manual, formating program
statements in a monospaced font, adding some cover page images
from the Museums Victoria website, and adding a table of contents.

By the way, the paper tape down the left-hand side of the first cover
page reads:

    WAFFLE FOR THE COVER OF THE INTERPROGRAM MANUAL 1/10/60.

## Programming in English

INTERPROGRAM was advertised as "Programming in English".  But what does
that mean?  Clearly natural language processing was beyond the capabilities
of the CSIRAC.

Let's take a simple computation, z = x<sup>2</sup> + y, and write out the
primitive steps of the computation in plain English:

1. Take the value of x.
2. Multiply it by itself.
3. Add the value of y.
4. Replace the contents of the variable z with the result.

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

    TAKE FORM           # Ordinary variable
    FORM SQUARE ROOT    # Statement keyword

Say we want to multiply X by several variables A, B, C, and D.  We could do
this:

    TAKE X
    MULTIPLY BY A
    MULTIPLY BY B
    MULTIPLY BY C
    MULTIPLY BY D

But that is long-winded.  English to the rescue with an ampersand:

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

In the original language there was a variable assignment feature
using `SET`.  Floating-point variables could be assigned a constant:

    SET X = 23
    SET X = -499.999

Integer variables could be assigned constants or the result of some
simple algebraic forms:

    SET J = 23          SET K = -499
    SET J = J + 5       SET K = K - 99
    SET J = K           SET J = -K

That's it!  This is probably due to the limitations of the CSIRAC.
There wasn't enough memory to implement a full algebraic expression
parser.  That forced Geoff Hill to come up with other approaches.

My implementation of INTERPROGRAM allows algebraic expressions
involving numeric addition, subtraction, multiplication, division,
and negation.  The previous hypotenuse example can be rewritten
as follows:

    TAKE A*A + B*B
    FORM SQUARE ROOT
    REPLACE H

Algebraic expressions can be used in other contexts:

    SET SUMOFSQUARES = A*A + B*B
    IF A * B IS POSITIVE, SET J = 42

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
middle.  Blanks are used as padding on the tape between sections of data.

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

INTERPROGRAM used "stop" to allow the entry of large multi-tape
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

## Preliminary statements

The program begins with several numbered "preliminary statements" to declare
global information about the program.

`(1) TITLE` gives a title to the program, which was traditionally written
to the program's printout so that the machine operators knew who to give
the output to.

`(2) SYMBOLS FOR INTEGERS` declares all of the identifiers that will be
used for integer values in the program.  In the original language,
all other symbols were implicitly declared as floating-point.
`NONE` indicates that there are no integer variables.  The symbol
names may be separated by spaces or commas:

    (2) SYMBOLS FOR INTEGERS J K BETA, B, C

In the original language, up to 8 integer variables could be declared
on a single line.  My implementation allows an arbitrary number, with
multiple `(2) SYMBOLS FOR INTEGERS` lines if necessary.

`(3) MAXIMUM SUBSCRIPTS` is used to declare the size of any arrays that
will be used by the program:

    (3) MAXIMUM SUBSCRIPTS A(3), X(37), J(-5)

Array A has indexes 0, 1, 2, and 3; X has indexes 0 to 37; and J has
indexes 0, -1, -2, -3, -4, and -5.  The original manual implied that
subscript ranges were possible; e.g. -5 to +3; using "dummy symbols".
My implementation lists the range explicitly, with the end-points
separated by a colon:

    (3) MAXIMUM SUBSCRIPTS A(-5:+3)

In the original language, integer array elements had to be declared
individually:

    (2) SYMBOLS FOR INTEGERS K(0) K(1) K(2) K(3)

My implementation takes a two-step approach.  The symbol is declared
as an integer first, and then promoted to be an array of integers:

    (2) SYMBOLS FOR INTEGERS K
    (3) MAXIMUM SUBSCRIPTS K(3)

The last preliminary statement must be `(4) COMPILE THE FOLLOWING INTERPROGRAM`.

## Reserved words

Reserved words are any that correspond to a single-word keyword:

    ADD         MODULO      REPLACE     THIS
    CALL        NONE        RETURN      TIMES
    IF          OUTPUT      SET         TITLE
    INPUT       PAUSE       SUBTRACT    TO

Reserved words cannot be used as variable or label names.

## Variables and types

Variable names may be any combination of 1 or more letters, as long as
the name does not correspond to a single-word keyword.  Case is
not significant.

Classic INTERPROGRAM only considered the first four characters to be
significant, with the remainder ignored when comparing variable names.
My implementation allows arbitrary-length names.

INTERPROGRAM only has two types: integer and floating-point.
Integer variables must be declared with `(2) SYMBOLS FOR INTEGERS`.

Array variables may be subscripted with `X(n)` where `n` is an expression.

Variables may be assigned with `SET` or `REPLACE`.  The value of a
variable may appear in any expression, or be taken into `THIS` with `TAKE`.

## Mathematical operators

In the statements below, <i>value</i> is a variable, constant,
or a reference to an array element.  Some examples:

    ADD X
    SUBTRACT 1.5
    MULTIPLY BY 2
    DIVIDE BY A(6)

<table border="1">
<tr><td><b>Statement</b></td><td><b>Description</b></td><td><b>Extension?</b></td></tr>
<tr><td><tt>ADD</tt> <i>value</i></td><td>Adds <i>value</i> to <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; performs string concatenation if used on string arguments.</td><td>String concatenation is an extension</td></tr>
<tr><td><tt>SUBTRACT</tt> <i>value</i></td><td>Subtracts <i>value</i> from <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>MULTIPLY BY</tt> <i>value</i></td><td>Multiplies the <i>value</i> with <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>DIVIDE BY</tt> <i>value</i></td><td>Divides <tt>THIS</tt> by <i>value</i>, leaving the quotient in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>MODULO</tt> <i>value</i></td><td>Divides <tt>THIS</tt> by <i>value</i>, leaving the remainder in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>FORM SQUARE ROOT</tt></td><td>Forms the square root of <tt>THIS</tt>, leaving the result in <tt>THIS</tt></td><td> </td></tr>
<tr><td><tt>FORM SINE</tt></td><td>Forms the sine of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in units of pi; e.g. 0.5 is pi/2.</td><td> </td></tr>
<tr><td><tt>FORM COSINE</tt></td><td>Forms the cosine of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in units of pi.</td><td> </td></tr>
<tr><td><tt>FORM TANGENT</tt></td><td>Forms the tangent of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in units of pi.</td><td> </td></tr>
<tr><td><tt>FORM ARCTAN</tt></td><td>Forms the arctangent of <tt>THIS</tt>, leaving the result in <tt>THIS</tt>; angles are in units of pi.</td><td> </td></tr>
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
<tr><td><tt>LENGTH OF</tt></td><td>Length of the string in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>SUBSTRING FROM</tt> <i>index1</i> [<tt>TO</tt> <i>index2</i>]</td><td>Extracts a substring from <tt>THIS</tt> starting at <i>index1</i> and ending at <i>index2</i>.  If <i>index2</i> is not present, then the rest of the string starting at <i>index1</i> is the result.</td><td>Yes</td></tr>
</table>

Algebraic expressions in the extension syntax use the following operators
with precedence from highest to lowest:

<table border="1">
<tr><td><b>Operator</b></td><td><b>Description</b></td></tr>
<tr><td>(<i>value</i>)</td><td>Parenthesised expression</td></tr>
<tr><td><tt>LENGTH OF</tt> <i>value</i></td><td>Length of a string</td></tr>
<tr><td>-<i>value</i>, +<i>value</i></td><td>Unary negation or unary plus</td></tr>
<tr><td><i>value1</i> \* <i>value2</i>, <i>value1</i> / <i>value2</i>, <i>value1</i> % <i>value2</i></td><td>Multiplication, division, or remainder</td></tr>
<tr><td><i>value1</i> + <i>value2</i>, <i>value1</i> - <i>value2</i></td><td>Addition or subtraction</td></tr>
</table>

## Control flow statements

IF

GO TO

EXECUTE PROCESS and CALL

REPEAT FROM

## Conditions

<table border="1">
<tr><td><b>Expression</b></td><td><b>Description</b></td><td><b>Extension?</b></td></tr>
<tr><td><tt><i>value</i> IS ZERO</tt></td><td>Condition is true if <i>value</i> is equal to zero<sup>(see note 1)</sup></i></td><td> </td></tr>
<tr><td><tt><i>value</i> IS NOT ZERO</tt></td><td>Condition is true if <i>value</i> is not equal to zero<sup>(see note 1)</sup></i></td><td>Yes</td></tr>
<tr><td><tt><i>value</i> IS POSITIVE</tt></td><td>Condition is true if <i>value</i> is positive or zero</td><td> </td></tr>
<tr><td><tt><i>value</i> IS NEGATIVE</tt></td><td>Condition is true if <i>value</i> is negative</td><td> </td></tr>
<tr><td><tt><i>value1</i> IS EQUAL TO <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is equal to <i>value2</i></td><td>Yes</td></tr>
<tr><td><tt><i>value1</i> IS NOT EQUAL TO <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is not equal to <i>value2</i></td><td>Yes</td></tr>
<tr><td><tt><i>value1</i> IS GREATER THAN <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is greater than <i>value2</i></td><td> </td></tr>
<tr><td><tt><i>value1</i> IS MUCH GREATER THAN <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is much greater than <i>value2</i><sup>(see note 2)</sup></td><td> </td></tr>
<tr><td><tt><i>value1</i> IS GREATER THAN OR EQUAL TO <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is greater than or equal to <i>value2</i></td><td>Yes</td></tr>
<tr><td><tt><i>value1</i> IS SMALLER THAN <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is smaller than <i>value2</i></td><td> </td></tr>
<tr><td><tt><i>value1</i> IS MUCH SMALLER THAN <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is much smaller than <i>value2</i><sup>(see note 2)</sup></td><td> </td></tr>
<tr><td><tt><i>value1</i> IS SMALLER THAN OR EQUAL TO <i>value2</i></tt></td><td>Condition is true if <i>value1</i> is smaller than or equal to <i>value2</i></td><td>Yes</td></tr>
<tr><td><tt><i>value</i> IS FINITE</tt></td><td>Condition is true if <i>value</i> is a finite value<sup>(see note 3)</sup></td><td>Yes</td></tr>
<tr><td><tt><i>value</i> IS INFINITE</tt></td><td>Condition is true if <i>value</i> is an infinite value<sup>(see note 3)</td><td>Yes</td></tr>
<tr><td><tt><i>value</i> IS NOT A NUMBER</tt></td><td>Condition is true if <i>value</i> is not a number<sup>(see note 3)</td><td>Yes</td></tr>
<tr><td><tt><i>value</i> IS EMPTY</tt></td><td>Condition is true if <i>value</i> is an empty string</td><td>Yes</td></tr>
<tr><td><tt><i>value</i> IS NOT EMPTY</tt></td><td>Condition is true if <i>value</i> is not an empty string</td><td>Yes</td></tr>
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

TBD

## Other statements

<table border="1">
<tr><td><b>Statement</b></td><td><b>Description</b></td><td><b>Extension?</b></td></tr>
<tr><td><tt>END OF INTERPROGRAM</tt></td><td>End the program with an exit status of 0.</td><td> </td></tr>
<tr><td><tt>EXIT INTERPROGRAM</tt></td><td>End the program with an exit status of <tt>THIS</tt> (0-255).  If <tt>THIS</tt> is not an integer in the range 0 to 255, then the exit status will be 1.</td><td>Yes</td></tr>
</table>

## License

MIT License.

## Contact

For more information on this project, to report bugs, or to suggest
improvements, please contact the author Rhys Weatherley via
[email](mailto:rhys.weatherley@gmail.com).
