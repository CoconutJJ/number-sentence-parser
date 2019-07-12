# number-sentence-parser
Parse and evaluate a number sentence!

## How to use:
Create a file and enter a number sentence of the following form:
~~~
Let G be the set of valid number sentences.

A number sentence, P, is a member of G, if P satisfies:

Base Case: P = c where c is in R
Inductive Step:
P = (A k B) where k is a member of {+,-,*,/} and A,B are members of G
OR
P = -A where A is a member of G
~~~
Example: `((0.34) - (2.3 - 3.14))`
~~~
$ gcc number_sentence_parser.c
$ ./a.out <filename>
~~~

