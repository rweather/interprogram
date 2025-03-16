# Console operations

Extended INTERPROGRAM includes functionality for dealing with an
interactive text mode console.

<tt>CLEAR SCREEN</tt>

Clears the screen to the current text foreground and background colours.
The default colours are a white foreground over a black background.

<tt>CLEAR TO END OF SCREEN</tt>

Clears from the current cursor position to the end of the screen.

<tt>CLEAR TO END OF LINE</tt>

Clears from the current cursor position to the end of the current line.

<tt>SET W = SCREEN WIDTH</tt>

Sets <tt>W</tt> to the width of the screen in characters.  <tt>SCREEN WIDTH</tt>
can be used in any expression context.

<tt>SET H = SCREEN HEIGHT</tt>

Sets <tt>H</tt> to the height of the screen in lines.  <tt>SCREEN HEIGHT</tt>
can be used in any expression context.

<tt>MOVE CURSOR TO</tt> <i>x</i> : <i>y</i>

Moves the cursor to (<i>x</i>, <i>y</i>).  (0, 0) is the top-left
of the screen.

<tt>SET CX = CURSOR X</tt>

Sets <tt>CX</tt> to the X co-ordinate of the cursor.  <tt>CURSOR X</tt>
can be used in any expression context.

<tt>SET CY = CURSOR Y</tt>

Sets <tt>CY</tt> to the Y co-ordinate of the cursor.  <tt>CURSOR Y</tt>
can be used in any expression context.

<tt>HIDE CURSOR</tt>

Hides the cursor from the screen.

<tt>SHOW CURSOR</tt>

Shows the cursor on the screen.

<tt>NORMAL TEXT</tt>

Selects normal text.

<tt>BOLD TEXT</tt>

Selects bold text.

<tt>TEXT COLOR</tt> <i>foreground</i> : <i>background</i>

Sets the <i>foreground</i> and <i>background</i> text colours.  Use the
values <tt>BLACK</tt>, <tt>RED</tt>, <tt>GREEN</tt>, <tt>YELLOW</tt>,
<tt>BLUE</tt>, <tt>MAGENTA</tt>, <tt>CYAN</tt>, and <tt>WHITE</tt>.
The <i>background</i> colour is optional.

<tt>DELETE CHARACTER</tt>

Deletes the character under the cursor.

<tt>DELETE LINE</tt>

Deletes the line under the cursor.

<tt>INSERT STRING</tt><br/>
<tt>INSERT STRING</tt> <i>value</i>

Inserts the string <i>value</i> at the cursor position.  If <i>value</i> is
not provided, then the value of <tt>THIS</tt> will be inserted instead.

<tt>INSERT LINE</tt>

Inserts a new line at the cursor position.

<tt>BEEP SPEAKER</tt>

Beeps the speaker (terminal bell).

<tt>FLASH SCREEN</tt>

Flashes the screen.

<tt>INPUT KEY</tt><br/>
<tt>INPUT KEY</tt> <i>timeout</i>

Inputs a single keypress from the keyboard and returns it as a string
in <tt>THIS</tt>.  The key is not echo'ed.

The optional <i>timeout</i> is in milliseconds and indicates how long to
wait before returning an empty string to mean "no key".  A negative
<i>timeout</i> indicates to wait forever.

Function and navigation keys are returned as multi-character strings
such as "(F1)", "(F2)", "(UP)", "(RIGHT)", etc.  Single-character strings
correspond to ASCII characters.

<tt>INPUT</tt> <i>var</i>

Reads a line of text from the input and parses it according to the
type of <i>var</i>.

<tt>OUTPUT</tt></br>
<tt>OUTPUT</tt> <i>value</i>

Outputs a <i>value</i> (or <tt>THIS</tt>) to the screen.

[Previous: Random numbers](ref-random.md),
[Next: Other statements](ref-other.md)
