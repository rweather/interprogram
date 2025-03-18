# Control flow statements

The most basic kind of control flow is `IF ... GO TO`:

        IF X IS POSITIVE, GO TO *7
        SET X = -X
    *7

The next section lists the allowable conditions for `IF`.

Classic INTERPROGRAM supported numeric labels between 1 and 150.
Extended INTERPROGRAM supports numeric labels between 1 and 9999,
and also supports any number of named labels:

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

See [Defining new statements](ref-define-stmt.md) for another method to call
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

See [Structured programming](ref-control-flow2.md) for more control
flow statements.

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

[Previous: String operations](ref-strings.md),
[Next: Input and output](ref-input-output.md)
