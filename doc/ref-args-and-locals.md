# Subroutine arguments and local variables

Subroutines in Classic INTERPROGRAM can take a single argument in the
<tt>THIS</tt> variable, but everything else must be passed in
global variables.

Extended INTERPROGRAM supports up to 9 subroutine arguments with the special
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

[Previous: Input and output](ref-input-output.md),
[Next: Structured programming](ref-control-flow2.md)
