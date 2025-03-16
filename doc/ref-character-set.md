# Character set

In 1960, CSIRAC was using a Flexowriter teletype for character-based
input and output.  The Flexowriter used a 6-bit code, encoded into 5 bits
using "shift" symbols to switch between two sets of symbols.

The first set of symbols consisted of letters and control characters:

    blank A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
    space carriage-return figure-shift letter-shift erase

The second set of symbols consisted of figures, punctuation, and control
characters:

    blank 0 1 2 3 4 5 6 7 8 9 + - * / ( ) # = & , . ' x £ stop tab
    space carriage-return figure-shift letter-shift erase

The "blank" symbol is different from "space".  Blanks are positions on the
punch tape with no holes punched other than the sprocket hole down the
middle.  In ASCII we would call this a "NUL".  Blanks are used as padding
on the tape between sections of data.

The "erase" symbol is a tape position with all holes punched.  In those
days if you wanted to erase a punched symbol, you would back up the
tape and punch "erase" over the top.

My implementation of INTERPROGRAM uses ASCII instead of Flexowriter codes.
We do need a mapping for "blank" however, as INTERPROGRAM uses it as
part of its syntax to separate sections.  We use "~" as that character
didn't exist on the Flexowriter.  It won't appear in normal programs
except in comments or strings.

ASCII doesn't have an equivalent to "erase".  Easiest is to remove the
erased characters entirely when converting a Flexowriter tape to ASCII.

Classic INTERPROGRAM used "stop" to allow the entry of large multi-tape
programs and data.  The programmer would put a "stop" code on the end of
one tape, which paused the system waiting for the operator to load the
next tape onto the tape reader.  We use the end-of-file indicator
for "stop" instead.

[Previous: Identifiers and constants](ref-ident-and-const.md),
[Next: Preliminary statements](ref-preliminary.md)
