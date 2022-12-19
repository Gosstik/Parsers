# Parcers
Here you can find implementation of the parsing strings algorithms for context-free languages. Exactly:
1) Earley parser [(wiki)](https://en.wikipedia.org/wiki/Earley_parser) [(code)](https://github.com/Gostik007/Parsers/blob/master/src/BasicEarleyParser.h)
2) LR(1) parser [(wiki)](https://en.wikipedia.org/wiki/LR_parser) [(code)](https://github.com/Gostik007/Parsers/blob/master/src/BasicLR1Parser.h)

## Installation
### Requirements:
cmake version 3.23  
C++20  
Boost C++ library

### One of the possible way to install requirements:
```
sudo apt-get update && upgrade
sudo apt-get install cmake
sudo apt-get install g++
sudo apt-get install libboost-all-dev
```

### Installation Instruction:
Open terminal and write the following:
```
git clone git@github.com:Gostik007/Parsers.git
cd Parsers
mkdir build
cd build
cmake ../
make
```

## How To Use:
To set grammar you should put it in the file `./Parsers/GrammarText/GrammarText.txt` using __input format__.

### Input format
1-st string: `start nonterminal` and `epsilon symbol`, separated by `` ` ``.\
2-nd string: `nonterminal symbols`, separated by `` ` ``.\
3-rd string: `terminal symbols`, separated by `` ` ``.\
On other strings rules are defined according to the following template:\
`<nonterminal>` -> `<sequence.1>` | `<sequence.2>` | ... | `<sequence.n>`

>1) `Terminals` may consist of any **one** symbol. Symbols `` \ `` and `|` must be escaped (`\\` and `\|` accordingly). Symbol `` ` `` always must be escaped as `` `\`` ``
>2) `Start nonterminal` must not be listed among nonterminals.
>3) `Nonterminals` may consist of several symbols, but must not contain escape symbols.
>4) If `start nonterminal` is the only one nonterminal in grammar, 2-nd line must be empty.
>4) Rule for each `nonterminal` must be defined on separate line.
>5) Each nonterminal must generate at least one sequence of symbols.
>6) `->` in rules must be separated by single spaces.
>7) Each nonterminal in `sequence.i` must be separated from other symbols by `` ` ``. If there are several terminals in a row, `` ` `` may be omitted.

### Examples:
1) Grammar that produces sequences `a^mb^lc^k`, where 0 <= m, l, k <= 3.
```
S`e
A`B`C
a`b`c
S -> A`B`C
A -> aaa | aa | a | e
B -> bbb | bb | b | e
C -> ccc | cc | c | e
```
2) Palindromes.
```
S`e

a`b
S -> a`S`a | b`S`b | a | b | e
```
3) Balanced bracket sequence.
```
S`e

(`)`[`]`{`}
S -> (`S`)`S | [`S`]`S | {`S`}`S | e
```
4) Grammar that reproduces sentences as `Max is an immaculate student.` or `Dan is an impartial teacher.`.
```
BEGIN`'empty'
NAME`ADJ_CONSONANT`ADJ_VOWEL`OCCUPATION
a`b`c`d`e`f`g`h`i`j`k`l`m`n`o`p`q`r`s`t`u`v`w`x`y`z`A`B`C`D`E`F`G`H`I`J`K`L`M`N`O`P`Q`R`S`T`U`V`W`X`Y`Z` `.
BEGIN -> NAME` `is a `ADJ_CONSONANT` `OCCUPATION`. | NAME` is an `ADJ_VOWEL` `OCCUPATION`.
NAME -> Max | Dan | Jack | Thomas | Harry
ADJ_CONSONANT -> carefree | brilliant
ADJ_VOWEL -> impartial | immaculate
OCCUPATION -> teacher | student | programmer | designer
```
Last example is not LR(1)-grammar and only Earley parser will be able to process queries; 

### Launch:
Open `Parsers` folder in terminal and write:
```
./bin/Parsers
```