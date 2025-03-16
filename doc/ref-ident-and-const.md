# Identifiers and constants

Identifiers are a sequence of one or more alphabetic letters between A and Z.
Upper and lower case may be used, but the interpreter will force everything
to upper case.

It is not possible to use digits in a variable name; e.g. "A3".  Instead,
put the value in an array and use a subscripted term like "A(3)".

Integer constants are one or more decimal digits, optionally preceded by a
minus sign:

    0
    123
    -89

Floating-point constants have decimal points or exponents, such as:

    0.123
    1.5e27
    -60
    4.567e-10

In the classic language, the "e" notation didn't exist.  Instead parentheses
were used for the exponent:

    1.5(27)
    4.567(-10)

This implementation accepts either the "e" notation or the parenthesised
notation, although the "e" notation is preferred.

Strings didn't exist in the classic language; I have added them to
my implementation.  Strings may appear in either single or double quotes:

    'abc'
    "Hello, World!\n"
    'abc''def'          # Final string: abc'def

The single quoted form is preferable as double quotes did not exist in the
CSIRAC's original character set.

Some limited C-style escape sequences are available:

<table border="1">
<tr><td><b>Escape</b></td><td><b>ASCII Character</b></td><td><b>Decimal</b></td><td><b>Hexadecimal</b></td></tr>
<tr><td>\a</td><td>Bell</td><td>7</td><td>07</td></tr>
<tr><td>\b</td><td>Backspace</td><td>8</td><td>08</td></tr>
<tr><td>\t</td><td>Tab</td><td>9</td><td>09</td></tr>
<tr><td>\n</td><td>Newline</td><td>10</td><td>0A</td></tr>
<tr><td>\v</td><td>Vertical Tab</td><td>11</td><td>0B</td></tr>
<tr><td>\f</td><td>Form Feed</td><td>12</td><td>0C</td></tr>
<tr><td>\r</td><td>Carriage Return</td><td>13</td><td>0D</td></tr>
<tr><td>\e</td><td>Escape</td><td>27</td><td>1B</td></tr>
</table>

All other \\c escapes are the same as the character c.  Octal and
hexadecimal character escapes are not supported.

[Next: Character set](ref-character-set.md)
