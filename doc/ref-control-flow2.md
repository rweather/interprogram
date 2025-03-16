# Structured programming

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

In English, this can be read as "REPEAT the loop body WHILE the
condition is true".

If you want an infinite loop that repeats forever, you can do one of
the following (the second is preferable):

    REPEAT WHILE X IS EQUAL TO X
        ...
    END REPEAT

    REPEAT FOREVER
        ...
    END REPEAT

For loops are also possible:

    REPEAT FOR X = 0 TO 100 BY 5
        ...
    END REPEAT

The step is given with the optional <tt>BY</tt> clause.  By default the
step is 1.  The loop ends when the loop variable exceeds the end value
(or goes below the end value if the step is negative).  There will be
zero iterations if the starting value exceeds the ending value.

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

[Previous: Subroutine arguments and local variables](ref-args-and-locals.md),
[Next: Definining new statements](ref-define-stmt.md)
