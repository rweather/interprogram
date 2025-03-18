# Preliminary statements

The program begins with several "preliminary statements" to declare
global information about the program.  In the classic language, each
preliminary statement is prefixed with a number in parentheses:

     (1)  TITLE  ALPHA = BETA - 27.394 + A(3).(X/Y)
     (2)  SYMBOLS FOR INTEGERS   J
     (3)  MAXIMUM SUBSCRIPTS   A(3)
     (4)  COMPILE THE FOLLOWING INTERPROGRAM

Extended INTERPROGRAM drops the numbers:

     TITLE  ALPHA = BETA - 27.394 + A(3).(X/Y)
     SYMBOLS FOR INTEGERS   J
     MAXIMUM SUBSCRIPTS   A(3)
     COMPILE THE FOLLOWING INTERPROGRAM

When <tt>TITLE</tt> appears on its own, the interpreter will automatically
switch into the Extended INTERPROGRAM syntax.  If the <tt>(1)</tt> is
present, then the Classic INTERPROGRAM syntax will be used.  The syntax
choice can be forced using the <tt>--classic</tt> and <tt>--extended</tt>
options on the command-line.

`TITLE` gives a title to the program, which was traditionally written
to the program's printout so that the machine operators knew who to give
the output to.  The title extends to the end of the line.

`SYMBOLS FOR INTEGERS` declares all of the identifiers that will be
used for integer values in the program.  In the classic language,
all other symbols were implicitly declared as floating-point.
`NONE` indicates that there are no integer variables.  The symbol
names may be separated by spaces or commas:

    SYMBOLS FOR INTEGERS J K BETA, B, C

In the original implementation, up to 8 integer variables could be declared
on a single line.  My implementation allows an arbitrary number, with
multiple `SYMBOLS FOR INTEGERS` lines if necessary.  My implementation
also allows string variables to be declared:

    SYMBOLS FOR STRINGS NAME, ADDRESS

`MAXIMUM SUBSCRIPTS` is used to declare the size of any arrays that
will be used by the program:

    MAXIMUM SUBSCRIPTS A(3), X(37), J(-5)

Array A has indexes 0, 1, 2, and 3; X has indexes 0 to 37; and J has
indexes 0, -1, -2, -3, -4, and -5.  The original manual indicates that
subscript ranges were possible; e.g. -5 to +3; using "dummy symbols":

    (3) MAXIMUM SUBSCRIPTS ZZ(5), A(3)

My implementation lists the range explicitly, with the end-points
separated by a colon:

    MAXIMUM SUBSCRIPTS A(-5:+3)

In the classic language, integer array elements had to be declared
individually:

    (2) SYMBOLS FOR INTEGERS K(0) K(1) K(2) K(3)

My implementation takes a two-step approach.  The symbol is declared
as an integer or string first, and then promoted to be an array:

    SYMBOLS FOR INTEGERS J, K
    SYMBOLS FOR STRINGS S
    MAXIMUM SUBSCRIPTS K(3), S(9)

The last preliminary statement must be `COMPILE THE FOLLOWING INTERPROGRAM`.

Classic INTERPROGRAM requires (1), (2), and (4) to be present and allows
multiple (3) preliminary statements.  Extended INTERPROGRAM requires
<tt>TITLE</tt> but the other preliminary statements are optional.

[Previous: Character set](ref-character-set.md),
[Next: Variables and types](ref-vars-and-types.md)
