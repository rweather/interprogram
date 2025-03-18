# Random numbers

Extended INTERPROGRAM has two constructs for working with random numbers.

<tt>SET N = RANDOM NUMBER</tt>

Sets <tt>N</tt> to a random floating-point value between 0 (inclusive)
and 1 (exclusive).  For example, to generate a random integer between
1 and 10:

    SYMBOLS FOR INTEGERS J

    TAKE RANDOM NUMBER
    MULTIPLY BY 10
    ADD 1
    REPLACE J

or alternatively:

    SYMBOLS FOR INTEGERS J
    SET J = RANDOM NUMBER * 10 + 1

<tt>SEED RANDOM</tt> <i>value</i>

Seeds the random number generator using the integer <i>value</i>.
This can be used for repeatable random sequences.  Normally the
random number generator is seeded from the system time at startup.

This implementation uses the standard C rand() and srand() functions to
generate random numbers.  The quality of the random numbers will depend
upon the underlying system's C library.

[Previous: Defining new statements](ref-define-stmt.md),
[Next: Console operations](ref-console.md)
