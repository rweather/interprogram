# Defining new statements

The subroutine syntax allows new functionality to be provided under a
convenient name.  But it is a little annoying to have to use the
<tt>EXECUTE PROCESS</tt> or <tt>CALL</tt> keywords.

It would be nice if we could add new kinds of statements to the language.
The extended syntax allows this: first declare that the name is a routine
with <tt>SYMBOLS FOR ROUTINES</tt>:

    SYMBOLS FOR ROUTINES ARCCOS

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

    SYMBOLS FOR ROUTINES 'FORM ARCCOS'

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

[Previous: Structured programming](ref-control-flow2.md),
[Next: Random numbers](ref-random.md)
