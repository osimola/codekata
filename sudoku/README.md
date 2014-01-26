Sudoku
======

A story: Once I was in a C interview in a large-ish software
company. You may have heard about it. Anyway, the interviewer asked if
I knew sudoku. I told them that I knew, but I probably couldn't hack
together a solver in the two hour time slot. So, I was asked to write
a solution verifier, which I wrote quickly and then we chatted about
code portability. Later I was told I had failed that one.

This began as an attempt to write the solver in two hours. The two
hour version with constraint propagation as the only algorithm even
worked, but only for the simplest one star cases. Since then, I
improved it with detection for squares where only one a single value
fits (turns out most puzzles require only a couple of guesses at
maximum) and tracing back the solution.

Requires glib for hash map and pkg-config for glib. Compiles with
Make. Works like this:

```
./qsudoku < 5star.txt
```

The input file format is what you would expect for program written in
C, in two hours while concentrating on the algorithm. They look like
this:

```
.....32..
....4...8
54...8.76
.6..7...9
4..8.5..7
7...6..8.
28.7...45
1...3....
..41.....
```

Anything expanding past 9 characters is read as the beginning of the
next line. Anything that is not a number is read as empty. Simple.