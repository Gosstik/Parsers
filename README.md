__Input format__:\
1-st string: `start nonterminal` and `epsilon symbol`, separated by space.\
2-nd string: `nonterminal symbols`, separated by space.\
3-rd string: `terminal symbols`, separated by space.\
On other strings rules are defined according to the following template:\
`<nonterminal>` -> `<sequence.1>`|`<sequence.2>`|...|`<sequence.n>`

>1) `Terminals` must consist of any *one* symbol except for `` ` ``.
>2) `Start nonterminal` must not be listed among nonterminals.
>3) `Nonterminals` may consist of several symbols, but must not contain `` ` ``.
>4) If `start nonterminal` is the only one nonterminal in grammar, 2-nd line must be empty.
>4) Rule for each `nonterminal` must be defined on separate line.
>5) Each nonterminal must generate at least one sequence of symbols.
>6) `->` in rules_ must be separated by single spaces.
>7) In case `sequence.i` contains more than one symbol, symbols must be separated by `` ` ``.
>8) To use a space as a nonterminal, the keyword `'space'` together with quotes is used (it must be listed among the terminals).

**Example:**
```
S e
A1 B1 C1
a b c
S -> a`B1|e
A1 -> B1`a|a|B1`C1
B1 -> A1`B1`C1|C1`A1|b|a
C1 -> B1`A1|c|A1`B1|e
```

S e\
A' B' C'\
a b c\
S = a\`B'\
A' = B'\`a|a\
B' = A'\`B'\`C'|b\
C' = B'\`A'|c

S e\
A1 B1 C1\
a b c\
S = a'B1\
A1 = B1'a|a\
B1 = A1'B1'C1|b\
C1 = B1'A1|c

S e\
A1 B1 C1\
a b c\
S = a\`B1\
A1 = B1\`a|a\
B1 = A1\`B1\`C1|b\
C1 = B1\`A1|c

S e\
A1 B1 C1\
a b c\
S = a\`B1\
A1 = B1\`a | a | B1\`C1\
B1 = A1\`B1\`C1 | C1\`A1 | b | a\
C1 = B1\`A1 | c | A1\`B1

S e\
A1 B1 C1\
a b c\
S = a\`B1\
A1 = B1\`a|a|B1\`C1\
B1 = A1\`B1\`C1|C1\`A1|b|a\
C1 = B1\`A1|c|A1\`B1

S e\
A B C\
a b c\
S = aB\
A = Ba|a\
B = ABC|b\
C = BA|c

