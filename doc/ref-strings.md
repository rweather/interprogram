# String operations

<tt>ADD</tt> and "+" can be used to perform string concatenation in
Extended INTERPROGRAM.  The following extension statements are also available:

<table border="1">
<tr><td><b>Statement</b></td><td><b>Description</b></td><td><b>Extension?</b></td></tr>
<tr><td><tt>CHARACTER CODE TO STRING</tt></td><td>Converts the integer character code in <tt>THIS</tt> into a single-character string (0 will result in the empty string).</td><td>Yes</td></tr>
<tr><td><tt>LENGTH OF</tt></td><td>Length of the string in <tt>THIS</tt></td><td>Yes</td></tr>
<tr><td><tt>NUMBER TO STRING</tt></td><td>Converts the numeric value in <tt>THIS into a string</tt></td><td>Yes</td></tr>
<tr><td><tt>PAD STRING ON LEFT</tt> <i>size</i></td><td>Pads the string in <tt>THIS</tt> to <i>size</i> characters by padding the value on the left with spaces.</td><td>Yes</td></tr>
<tr><td><tt>PAD STRING ON RIGHT</tt> <i>size</i></td><td>Pads the string in <tt>THIS</tt> to <i>size</i> characters by padding the value on the right with spaces.</td><td>Yes</td></tr>
<tr><td><tt>STRING TO CHARACTER CODE</tt></td><td>Converts the first character in the string <tt>THIS</tt> into an integer character code (0 if the string is empty).</td><td>Yes</td></tr>
<tr><td><tt>STRING TO INTEGER</tt> [<i>base</i>]</td><td>Converts the string in <tt>THIS into an integer.  The optional <i>base</i> indicates the base for teh conversion between 2 and 26.  The default base is 10.  The special <i>base</i> of 0 indicates to recognise C-style decimal, hexadecimal, and octal values.</tt></td><td>Yes</td></tr>
<tr><td><tt>STRING TO LOWERCASE</tt></td><td>Converts the string in <tt>THIS</tt> into lowercase</td><td>Yes</td></tr>
<tr><td><tt>STRING TO NUMBER</tt></td><td>Converts the string in <tt>THIS</tt> into a floating-point value</td><td>Yes</td></tr>
<tr><td><tt>STRING TO UPPERCASE</tt></td><td>Converts the string in <tt>THIS</tt> into uppercase</td><td>Yes</td></tr>
<tr><td><tt>SUBSTRING FROM</tt> <i>index1</i> [<tt>TO</tt> <i>index2</i>]</td><td>Extracts a substring from <tt>THIS</tt> starting at <i>index1</i> and ending at <i>index2</i>.  If <i>index2</i> is not present, then the rest of the string starting at <i>index1</i> is the result.  String indexes are 1-based.</td><td>Yes</td></tr>
<tr><td><tt>TRIM STRING</tt></td><td>Trims whitespace from the start and end of <tt>THIS</tt></td><td>Yes</td></tr>
</table>

[Previous: Mathematical operators](ref-math.md),
[Next: Control flow statements](ref-control-flow1.md)
