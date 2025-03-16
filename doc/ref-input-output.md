# Input and Output

<tt>INPUT</tt> <i>variable</i>

Reads a value from the program's input stream into <i>variable</i> and
<tt>THIS</tt>.  The type of value read will depend upon the type of
<i>variable</i>: integer, floating-point, or string.  If <i>variable</i> is
omitted, then a floating-point value will be read into <tt>THIS</tt> only.

If end of file is encountered, then the next statement on the current
line will be skipped.  In the following example, control will go to
label 1 if a value is read, or to label 2 if end of file is encountered:

    INPUT X, GO TO *1, GO TO *2

Extended INTERPROGRAM provides a better method for end of file handling,
using the <tt>AT END OF INPUT</tt> statement:

    AT END OF INPUT, GO TO *3
    ...
    INPUT X, & Y, & Z
    ...

<tt>AT END OF INPUT</tt> arranges to execute the rest of the current
line when the input system detects end of file.  The <tt>INPUT</tt>
statement will not skip the next statement, as in the classic language.

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

[Previous: Control flow statements](ref-control-flow1.md),
[Next: Subroutine arguments and local variables](ref-args-and-locals.md)
