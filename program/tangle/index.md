---
title: TANGLE
---

<style>
object {
    border: 2px solid grey;
    width: 100%;
}
img {
    max-width: 100%;
}
</style>

For our next act, after having studied [`POOLtype`](../pooltype), we shall now study [TANGLE](http://texdoc.net/texmf-dist/doc/generic/knuth/web/tangle.pdf). Unlike POOLtype, this is not a trivial program; it is the bedrock on which WEB rests (or rested). It is much longer than POOLtype:

- `tangle.pdf` is 75 pages to POOLtype's 11

- Excluding contents, index and the “system dependencies” part, it is 66 pages to POOLtype's 7.

- It has 187 + 2 sections, to POOLtype's 20 + 2.

So it's about 9 times bigger. TeX in turn is about 7 times bigger than TANGLE, so we're nicely in between.

## Comparison of sizes

See [List of WEB files](https://docs.google.com/spreadsheets/d/1Fn5fZiYOYX39mWytZ7vmMe-KIe_boAxG6l6yihTn2ck/edit#gid=0).

| Program  | Pages   | Sections |Fraction of TeX|
| :------- | ------: | -------: | :-----------: |
| POOLTYPE | 7 + 4   | 20 + 2   | ≈ 1.4% to 2%  |
| GLUE     | 8 + 3   | 26 + 1   | ≈ 1.6% to 2%  |
| DVITYPE  | 47 + 7  | 111 + 2  | ≈ 8% to 10%   |
| TANGLE   | 66 + 9  | 187 + 2  | ≈ 13.5% to 14%|
| WEAVE    | 98 + 12 | 263 + 2  | ≈ 19% to 20.5%|
| TEX      | 478 + 57| 1378 + 2 |      100%     |


## Reasons for studying TANGLE

- It's a nontrival example of WEB, good preparation for TeX.

- Its way of doing input/input is similar to TeX's; in particular its way of writing out its output has a lot of similarities with how TeX expands macros.

- DEK himself alludes to (or compares with) the TeX program in a few places.

- We get to see how DEK writes a parser.

- It illustrates many things about the TeX program and the way DEK thinks, e.g. describing data-structures and how to use them, before describing how to populate them.

- If we can parse WEB files, it opens up conversion / translation of WEB programs (like TeX) to other languages... or at least putting the program on the web (HTML, with cross-references).

## What TANGLE needs to do

Before we start studying the TANGLE program itself, let's take a few minutes to think about a few things it needs to do: (These are just haphazard thoughts that occurred to me before reading the program; some of them are wrong and TANGLE works differently. The point of the exercise is to know what sort of things to expect.)

- Read the `.web` file, and notice/detect where modules begin. Also, where the Pascal (code) part begins, within each module.

- Store a map module → text (code) of module, to replace when the module is referenced. Similarly for macros.

- For modules, keep track of a module being continued, to append to its text. Keep track of what the top-level modules are.

- Strings: they should get numbers (ideally, identical strings should get the same number) and ultimately be written out to the pool file.

- Some arithmetic (e.g. in array dimensions and numeric macros)

- When looking at code, split into identifiers, to detect macros.

- Also to break lines: output with at most 72 characters per line or whatever.

## Outline of TANGLE

Based on what I've read so far (up to section 11), this is my understnding of how TANGLE works.

Recall that TANGLE needs to turn a WEB source file like (say) `pooltype.web` into a Pascal file `pooltype.p`. It has to do this while expanding macros and module (section) references, and also supporting some additional features like treating strings as numbers. To do this, it operates in two phases: 

1. (Compression) First it reads the `.web` file (and the `.ch` changefile if any) into a compressed representation in memory, one which is aware of e.g. what the strings and modules are.

2. (Expansion) Then it writes out this in-memory representation into the output file.

So the program can be thought of as:

        [read] -> [data structures] -> [write]

And we an try to learn each of the three parts separately (starting with the data structures of course).

The data structures: Mainly (see top of `/program/tangle/tangle-9` on this site),

- **Names** -- these are anything that are **sequences of bytes**. Everything from Pascal keywords like "begin" or "in", to names of variables, procedures, WEB macros, WEB module names, double-quoted strings (to be translated by WEB), etc. Names that are modules are identified separately, and for all other names we keep track of their “ilk” (numeric macro, simple macro, parametric macro, none-of-these), for modules and macros, we keep track of their **equivalents** (the numeric value or “text” that the name should be replaced with, see below.)

- **Texts** -- these are **sequences of tokens**. 

  - A *token*, which occupies 1 or 2 bytes, is either a printable ASCII character, one of several special values like "AND_SIGN" (encoded as non-printable ASCII characters less than 128), or an identifier (the name number), module (the name number) or module number.

  - A *text* therefore is a sequence of the above: the replacement text for a macro or module.

The "write" is further decomposed into:

- Linearizing the tree (or DAG, I guess), into a sequence of tokens. (Done by repeated calls to `get_output`.)

- Producing the textual output, from this sequence of tokens. (Done by `send_the_output` looping to repeatedly call `get_output`, and then branching to call one of `send_val`, `send_sign`, or `send_out`.)

## Table of contents

(Note the “section” below is what is known as “module” in the program! I'll use both names interchangeably.)

<object type="image/svg+xml" data="tangle-contents.svg"></object>

Because of its length, let's study each part separately. The horizontal lines below group the 20 parts into what seemed natural places to me.

- [1. Introduction](tangle-1) 1
- [2. The character set](tangle-2) 11
- [3. Input and output](tangle-3) 19
- [4. Reporting errors to the user](tangle-4) 29

----

- [5. Data structures](tangle-5) 37
- [6. Searching for identifiers](tangle-6) 50
- [7. Searching for module names](tangle-7) 65
- [8. Tokens](tangle-8) 70

----

- [9. Stacks for output](tangle-9) 77
- [10. Producing the output](tangle-10) 94
- [11. The big output switch](tangle-11) 112

----

- [12. Introduction to the input phase](tangle-12) 123
- [13. Inputting the next token](tangle-13) 143
- [14. Scanning a numeric definition](tangle-14) 156
- [15. Scanning a macro definition](tangle-15) 163
- [16. Scanning a module](tangle-16) 171

----

- [17. Debugging](tangle-17) 179
- [18. The main program](tangle-18) 182
- [19. System-dependent changes](tangle-19) 188

----

- [20. Index](tangle-20) 189
