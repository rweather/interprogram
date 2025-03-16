# Random numbers

Extended INTERPROGRAM has two statements for working with random numbers.

<tt>RANDOM NUMBER</tt>

Sets <tt>THIS</tt> to a random floating-point value between 0 (inclusive)
and 1 (exclusive).  For example, to generate a random integer between
1 and 10:

    SYMBOLS FOR INTEGERS J

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

[Previous: Defining new statements](ref-define-stmt.md),
[Next: Console operations](ref-console.md)
