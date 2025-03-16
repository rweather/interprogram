# Other statements

<tt>END OF INTERPROGRAM</tt>

End the program with an exit status of 0.  In the classic implementation,
this could only appear as the last statement in the program.  This
implementation allows <tt>END OF INTERPROGRAM</tt> to appear anywhere.
If the program is not terminated with <tt>END OF INTERPROGRAM</tt>,
then its presence is implied.

<tt>EXIT INTERPROGRAM</tt>

End the program with an exit status of <tt>THIS</tt> (0-255).
If <tt>THIS</tt> is not in the range 0 to 255, then the exit status will be 1.
This is an extension.

<tt>PAUSE</tt> <i>value</i>

In the classic implementation, <tt>PAUSE</tt> would suspend execution
showing <i>value</i> on the CSIRAC's neons.  Pressing 'Restart' would cause
the program to continue from the next statement.  <tt>PAUSE</tt> was used
as a primitive breakpoint mechanism, with <i>value</i> indicating how
far through the program execution has gotten.  This implementation evaluates
<i>value</i>, outputs it, and then continues execution immediately.

[Previous: Console operations](ref-console.md)
