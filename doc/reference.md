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

INTERPROGRAM was advertised as "English Language for Automatic Programming".
But what does that mean?  Clearly natural language processing was beyond the
capabilities of the CSIRAC.

Take a simple computation: "set z to the square root of (x<sup>2</sup> + y)".
Let's write out the primitive steps of the computation in plain English:

1. Take the value of x.
2. Multiply it by itself.
3. Add the value of y.
4. Form the square root of the result so for.
5. Replace the contents of the variable z with the final result.

That is how you would perform the computation with pencil and paper.
Here is what it looks like in INTERPROGRAM:

    TAKE X
    MULTIPLY BY THIS
    ADD Y
    FORM SQUARE ROOT
    REPLACE Z

That's almost the same as the pencil and paper version!

Here is another example of replacing X with the absolute value of X:

    TAKE X
    IF THIS IS NEGATIVE, MULTIPLY BY -1
    REPLACE X

This is the key to INTERPROGRAM's syntactic style: let the plain English
description of the algorithm inform how the code is written.  The
code should read the way it is written.  And don't be afraid to use
multiple words if it makes the code easier to read aloud; for example,
"MULTIPLY BY" and "FORM SQUARE ROOT", not "MULTIPLY" and "SQRT".

Of course, the compiler is a little stricter on syntax and semantics than
plain English, but the style is to make the program read the way you would
perform the operations with pencil and paper.

The "THIS" variable is special in that it refers to the result of the
last computation; essentially an accumulator.  "TAKE" loads a value into
"THIS" and "REPLACE" copies the contents of "THIS" to a destination.

Here is an example that calculates the hypotenuse of a right-angled
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
"FORM SQUARE ROOT" as they are more readable.  It also makes the
language easier to parse when trying to determine if an identifier
is a variable or a keyword:

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

"&" means "repeat the last statement with a new operand".  In English,
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

Extended INTERPROGRAM allows simple algebraic expressions involving
numeric addition, subtraction, multiplication, division, negation, and
some mathematical functions.  The previous hypotenuse example can be
rewritten as follows:

    TAKE A*A + B*B
    FORM SQUARE ROOT
    REPLACE H

or even as:

    SET H = SQUARE ROOT OF (A*A + B*B)

Algebraic expressions can be used in other contexts:

    IF A * B IS POSITIVE, SET J = 42

## Hello, World!

Here is the complete source code for the "Hello World" example in the
Classic INTERPROGRAM language:

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

In Extended INTERPROGRAM, "Hello World" is simpler:

    TITLE HELLO WORLD EXAMPLE
    OUTPUT 'Hello, World!'

The interpreter detects Classic vs Extended INTERPROGRAM from the first
non-comment line.  If it begins with <tt>(1) TITLE</tt>, then the
code is assumed to be in the Classic language.  If it begins with just
<tt>TITLE</tt>, then the code is assumed to be in the Extended language.
This can be overridden on the interpreter's command-line using the
<tt>--classic</tt> and <tt>--extended</tt> options.

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

## Reserved words

Reserved words are any that correspond to a single-word keyword:

    ADD         INFINITE*   PAUSE       THIS
    BY*         INPUT       POSITIVE    TIMES
    CALL*       IS          REPLACE     TITLE
    ELSE*       MODULO*     RETURN*     TO*
    EMPTY*      NEGATIVE    SET         ZERO
    FINITE*     NONE        SUBTRACT
    IF          OUTPUT      THEN*

Asterisks indicate reserved words that are part of Extended INTERPROGRAM,
but not the Classic INTERPROGRAM syntax.

Reserved words cannot be used as variable or label names.

## Extended language reference

* [Identifiers and constants](ref-ident-and-const.md)
* [Character set](ref-character-set.md)
* [Preliminary statements](ref-preliminary.md)
* [Variables and types](ref-vars-and-types.md)
* [Mathematical operators](ref-math.md)
* [String operations](ref-strings.md)
* [Control flow statements](ref-control-flow1.md)
* [Input and output](ref-input-output.md)
* [Subroutine arguments and local variables](ref-args-and-locals.md)
* [Structured programming](ref-control-flow2.md)
* [Defining new statements](ref-define-stmt.md)
* [Random numbers](ref-random.md)
* [Console operations](ref-console.md)
* [Other statements](ref-other.md)

## Differences in the classic syntax

The original or "classic" syntax is documented in the
[original manual](INTERMAN.pdf).  There are some slight differences
between my implementation and the original.

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

The original implementation only considered the first 4 letters of a
variable name significant.  `ALPHA` and `ALPHB` were the same variable.
This implementation allows an arbitrary number of characters, all of
which must be alphabetic.  Digits cannot be used in variable names
in either the classic or extended syntax; it is necessary to use
array subscripts like `A(3)`.

The original implementation terminated input data with `END` or `#`.
This implementation uses the end of file (EOF) condition instead.
Existing input data tapes will need to be converted to remove `END`/`#`,
to replace blank symbols with "~", and to remove any "erase" or "stop"
symbols that were originally present.

The original floating-point library on the CSIRAC had some accuracy issues.
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

The original implementation used "dummy symbols" to declare negative
array subscripts:

    (3) MAXIMUM SUBSCRIPTS ZZ(5), A(3)

This declaration means that the indexes for A range from -5 to +3.
In my implementation the range is provided explicitly without the use of
dummy symbols:

    (3) MAXIMUM SUBSCRIPTS A(-5:3)

`OUTPUT` statements formatted right-aligned numeric data in 8 spaces.
This implementation uses 15 as it gives nicer results with the greater
integer and floating-point precision used.  This may change the appearance
of the output for existing programs.

## `END OF INTERPROGRAM` language reference
