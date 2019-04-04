---
title: WEAVE
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

[WEAVE](http://texdoc.net/texmf-dist/doc/generic/knuth/web/tangle.pdf) complements [TANGLE](../tangle).

Recall the sizes:

| Program  | Pages   | Sections |Fraction of TeX|
| :------- | ------: | -------: | :-----------: |
| POOLTYPE | 7 + 4   | 20 + 2   | ≈ 1.4% to 2%  |
| GLUE     | 8 + 3   | 26 + 1   | ≈ 1.6% to 2%  |
| DVITYPE  | 47 + 7  | 111 + 2  | ≈ 8% to 10%   |
| TANGLE   | 66 + 9  | 187 + 2  | ≈ 13.5% to 14%|
| WEAVE    | 98 + 12 | 263 + 2  | ≈ 19% to 20.5%|
| TEX      | 478 + 57| 1378 + 2 |      100%     |


## Reasons for studying WEAVE

Many of the same reasons as for studying TANGLE. Plus:

- We've already studied a fair bit with TANGLE so hopefully this should be easy.

## What WEAVE needs to do

Before we start studying the WEAVE program itself, let's take a few minutes to imagine (speculate about?) a few things it needs to do:

- Read the `.web` file, and notice/detect where modules begin. Also, where the TeX, definition, and Pascal (code) parts begin, within each module.

- Keep track of what other modules are referenced in each module, so that we can later insert the "This code is used in..." cross-references.

- Understand the different kinds of keyword in the code part, because we'll need to format them later.

- Emit appropriate macros for each different kind of keyword.

- For indexing, store a map identifier → module, so that we can see what are the places an identifier is referenced or defined.

- Interpret all the WEAVE-specific control codes of WEB, such as those for inserting something into the index or for typesetting something as code.

Things we *don't* need to do:

- Keep track of replacment texts for macros and modules, and expand them.

- Keep track of where a module continues. (Except for the fact that it continues, I guess?)

- Strings as numbers, arithmetic.

- Breaking into lines: with the appropriate macros, TeX can do it.

## Outline of WEAVE

(Fill this in after reading and understanding the program.)

## Table of contents

(Note the “section” below is what is known as “module” in the program! I'll use both names interchangeably. Also, sometimes use “section”/“part”/“chapter” to refer to the divisions elow.)

<object type="image/svg+xml" data="weave-contents.svg"></object>

Because of the length of the WEAVE program, let's study each part separately. The horizontal lines below group the 23 parts into what seemed natural places to me.

- [1. Introduction](weave-1) 1
- [2. The character set](weave-2) 11
- [3. Input and output](weave-3) 19
- [4. Reporting errors to the user](weave-4) 29
- [5. Data structures](weave-5) 36
- [6. Searching for identifiers](weave-6) 55
- [7. Initializing the table of reserved words](weave-7) 63
- [8. Searching for module names](weave-8) 65
- [9. Lexical scanning](weave-9) 70
- [10. Inputting the next token](weave-10) 93
- [11. Phase one processing](weave-11) 108
- [12. Low-level output routines](weave-12) 121
- [13. Routines that copy TeX material](weave-13) 132
- [14. Parsing](weave-14) 139
- [15. Implementing the productions](weave-15) 144
- [16. Initializing the scraps](weave-16) 183
- [17. Output of tokens](weave-17) 200
- [18. Phase two processing](weave-18) 218
- [19. Phase three processing](weave-19) 239
- [20. Debugging](weave-20) 258
- [21. The main program](weave-21) 261
- [22. System-dependent changes](weave-22) 264
- [23. Index](weave-23) 265

