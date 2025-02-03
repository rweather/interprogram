INTERPROGRAM - Australia's Own High-Level Programming Language
==============================================================

INTERPROGRAM was developed by Geoff Hill in 1960 for the
[CSIRAC](https://en.wikipedia.org/wiki/CSIRAC), Australia's first
stored program computer and the only first-generation computer to
survive intact to this day.

INTERPROGRAM is an interpreted high-level programming language, advertised as
"English Language for Automatic Programming".  The syntax is very easy
for beginners to programming to learn.  It predated
[BASIC](https://en.wikipedia.org/wiki/BASIC) by several years.

## What is this project?

There are CSIRAC emulators available on the Internet that can run the
original INTERPROGRAM environment, but the CSIRAC was very limited.  See
[this page](doc/CSIRAC-history.md) for the technical specifications
and history.

I was curious what a modern implementation of INTERPROGRAM would look like,
to explore the language and its possibilities.

What if INTERPROGRAM had continued to be developed by the Australian
computer community?  What if it had become the standard "BASIC" for Australia?
What might it have looked like after several decades of improvement?

## Manual

The original [manual for the INTERPROGRAM language](doc/INTERMAN.pdf)
is in this repository for reference.  The original version of the
manual came in Word format from [this CSIRAC emulator](https://cis.unimelb.edu.au/about/csirac/emulator).

I have made some editorial improvements to the manual, formating program
statements in a monospaced font, adding some cover page images
from the Museums Victoria website, and adding a table of contents.

BTW, the paper tape down the left-hand side of the first cover page reads:

    WAFFLE FOR THE COVER OF THE INTERPROGRAM MANUAL 1/10/60.

## Programming in English

INTERPROGRAM was advertised as "Programming in English".  But what does
that mean?  Clearly natural language processing was beyond the capabilities
of the CSIRAC in 1960.

Let's take a simple computation: `z = x^2 + y` and write out the steps of the
computation in plain English:

1. Take the value of x.
2. Multiply it by itself.
3. Add the value of y.
4. Replace the contents of the variable z with the result.

What does this look like in INTERPROGRAM?

    TAKE X
    MULTIPLY BY THIS
    ADD Y
    REPLACE Z

It looks almost the same as our English description!  This is the key to
INTERPROGRAM's syntactic style - take the straight-forward set of steps in
English and write them out as closely as possible in the program.

Of course, the compiler is a little stricter on syntax and semantics than
plain English, but the style is to make the program read the way you would
explain it to another person on a blackboard.

The "THIS" variable is special in that it refers to the result of the
last computation; essentially an accumulator.  "TAKE" loads a value into
"THIS" and "REPLACE" copies the contents of "THIS" to a destination.

Say we want to multiply several variables A, B, C, and D.  We could do
this:

    TAKE A
    MULTIPLY BY B
    MULTIPLY BY C
    MULTIPLY BY D
    REPLACE Z

But that is verbose.  English to the rescue again with an ampersand:

    TAKE A, MULTIPLY BY B, & C, & D, REPLACE Z

"&" means "repeat the last operation with a new operand".  Commas are
used as statement separators.

Technically, the following would be even better English and still parsable:

    TAKE A, MULTIPLY BY B AND C AND D, REPLACE Z

But the original language went with "&" instead.  So that's what we will use.

## Character set

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
middle.  Blanks are used as padding on the tape between sections of data.

The "erase" symbol is a tape position with all holes punched.  In those
days if you wanted to erase a punched symbol, you would back up the
tape and punch "erase" over the top.

In our implementation of INTERPROGRAM, we will be using ASCII instead
of Flexowriter codes.  However, INTERPROGRAM uses "blank" and "erase" as
part of its syntax to separate sections.  We use "~" and "@" instead for
"blank" and "erase".  Those characters didn't exist on the Flexowriter,
so they won't appear in normal programs except in comments.

INTERPROGRAM also used "stop" to allow the entry of large multi-tape
programs and data.  The programmer would put a "stop" code on the end of
one tape, which paused the system waiting for the operator to load the
next tape onto the tape reader.  We use separate files for multiple tapes;
so the end-of-file indicator can be used for "stop".

## Preliminary statements

TBD

## Variables and types

TBD
