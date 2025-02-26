INTERPROGRAM - Australia's First High-Level Programming Language
================================================================

INTERPROGRAM was developed by Geoff Hill in 1960 for the
[CSIRAC](https://en.wikipedia.org/wiki/CSIRAC), Australia's first
electronic stored program computer and the only first-generation
computer to survive intact to this day.

INTERPROGRAM is an interpreted high-level programming language, advertised as
"English Language for Automatic Programming".  The syntax is very easy
for beginners to programming to learn.  It predated
[BASIC](https://en.wikipedia.org/wiki/BASIC) by three years.

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

By the way, the paper tape down the left-hand side of the first cover
page reads:

    WAFFLE FOR THE COVER OF THE INTERPROGRAM MANUAL 1/10/60.

The syntax for my implementation of "Extended INTERPROGRAM" is described
in the [Language Reference](doc/reference.md).

## Building

The source code is designed to be built using cmake.  To build and run the
automated tests:

    mkdir build
    cd build
    cmake ..
    make
    make test
    sudo make install

Then to run some examples:

    interprogram examples/hello_world.ip
    interprogram examples/matches1.ip
    interprogram examples/matches2.ip

## Was it used back in the day?

Absolutely, yes!  The following quote is from the book <i>The Last of the First,
CSIRAC: Ausralia's First Computer</i>, by Doug McCann and Peter Thorne, 2000,
ISBN 978-0-73-405168-4.  On page 30:

> Hill went on to develop a simple ‘English language’ code for CSIRAC called ‘INTERPROGRAM’. This was introduced in 1960 and was an automatic interpretive language which greatly simplified programming for users; following its introduction most clients adopted it because it was much more ‘user-friendly’ than the standard CSIRAC machine code and a more efficient use of their time.

and on page 66:

> Later on when INTERPROGRAM – the programming language developed by Geoff Hill – was adopted, many of the ordinary users used that. It was much more friendly than trying to write optimised code for CSIRAC, so the average user tended to use it. It wasn’t as efficient from a computing point of view, but it was a much more efficient use of their time. They didn’t have to learn the idiosyncrasies of this machine. They didn’t need to be super programmers, they just wanted to get a job done, so INTERPROGRAM courses were provided for them.

The book contains many more quotes from people who used INTERPROGRAM in
their work.

## License

MIT License.

## Contact

For more information on this project, to report bugs, or to suggest
improvements, please contact the author Rhys Weatherley via
[email](mailto:rhys.weatherley@gmail.com).
