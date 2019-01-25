---
title: Tangle -- Data structures
---

If you were trying to read the program, this is probably where you got overwhelmed.

In this part DEK introduces a dizzying array of... well, arrays, with complicated purposes and little motivation. So let's try providing some.

Recall that Pascal did not have good support for strings. Recall also that after the first phase, we want to have read everything from the input file(s) into memory, so we should hold in memory everything that we need in order to output the tangled program. This includes many “strings” the output is supposed to contain, which therefore need to be stored in memory as characters in some consecutive positions of memory somewhere.

## What do we need to store

Specifically, we need the following kinds of strings (lists of characters):

- The names of modules, e.g. module 11 may have name `Types in the outer block`.

- The names of macros, e.g. if we have a macro definition like `define print(#) ≡ write(term_out, #)`, then we need to store the string `print` somewhere (at least during the first phase, so that we can recognize it when we see it again).

  - This includes zero-parameter macros like `define return ≡ goto exit`, where we need to store `return` in memory somewhere. 

  - This also includes numeric macros like `define spotless = 0`, where we need to store `spotless`.

- The double-quoted strings that TANGLE is supposed to translate into (string pool) numbers, like `"A"` into `65`. (There aren't any of length > 1 in the TANGLE program itself, but there are many in TeX of course.)

- The identifiers in the program (e.g. variable or function names), like `limit` (the name of a variable) or `error` (the name of a procedure).

All of these "names" (strings, names of identifiers, names of modules) are stored in a huge array called `byte_mem[][]`: for every integer `i` up to the number of names stored, the name `i` starts at position `byte_start[i]` in the array `byte_mem[i%ww][]`, i.e. it occupies positions `byte_mem[i%ww][byte_start[i]]` to `byte_mem[i%ww][byte_start[i+ww] - 1]`.

Such integers `i` from `0` to the number of names stored (= `name_ptr` $\le$ `max_names`) are said to be of type `name_pointer`.

We also need to store 

## Getting to the names and vice-versa

And it's not enough to just stuff these names into memory somewhere and forget about them: we also need a way to get from their "id" (a number) to the name, and vice-versa.

This 










