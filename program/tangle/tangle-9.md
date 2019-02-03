---
title: Tangle -- Stacks for output
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

Let's summarize what we have so far:

- The "bytes" of the program (names of modules, names of macros, names of identifiers, and double-quoted strings) are written into `byte_mem`, with indexes in `byte_start`.

- The "tokens" of the program (the replacement text for simple and parametric macros, the replacement text for modules) are written into `tok_mem`, with indexes in `tok_start`.

- We have ways of looking up either modules or identifiers (any of the others) by name.

- We have the numeric equivalents (for numeric macros and double-quoted strings) and text equivalents (for simple macros, parametric macros, modules) (and the latter of these we can follow "to be continued" links for, using `text_link`).

- Given a name (or when looking at the replacement text for a macro / module), we can say what type it is.

Here is a visualization of some of all of this, for the memory at the end of phase one of reading POOLTYPE.web.

        name(string)    name_number    equiv(text:n or num:n)    text_number    text_link    text(tokens)

So in principle to output the entire program, this is what we'd need to do:

- Locate the replacement text for the unnamed module (it starts in `text_link[0]`), and start writing it out.

- When you encounter an identifier with a numeric equivalent (a numeric macro, or a double-quoted string), replace it with its numeric equivalent.

- When you encounter a module name, start (recursively) writing out *its* replacement text.

- When you encounter a simple macro, write out its replacement text.

- When you encounter a parametric macro, identify the parameter (how?) and write out the replacement text: note might have to put the parameter in some variable or on some stack or something.


<object type="image/svg+xml" data="tangle-077.svg"></object>

This struck me as a strange choice when I first encountered it: without saying how these data structures will be populated, we're describing how they'll produce the program? Basically describing Phase 2 before Phase 1? But now it makes sense and seems familiar, as the way DEK writes programs.

-----

At any given point of time, we are expanding the text for

- **name #n** (either a module or macro), i.e. the name that starts at `byte_start[n]` in `byte_mem`,

- this name is that of **module #m** (or 0 if name #n is that of a macro),

- the replacement text we're expanding is **text #r**, i.e. the one that starts at `tok_start[r]` in `tok_mem` (note `r` is one of `equiv[n]` or `text_link[equiv[n]]` or `text_link[text_link[equiv[n]]]` or...),

    - we're next going to read position **byte b**, i.e. `tok_mem[z][b]` where `z` is `r mod zz` and `b ≥ tok_start[r]`

- this replacement text **ends at position e**, i.e. `tok_mem[z][e]` where `e` is `tok_start[r + zz]`.

This is our state, `(n, m, r, b, e)`.

<object type="image/svg+xml" data="tangle-078.svg"></object>


<object type="image/svg+xml" data="tangle-079.svg"></object>


<object type="image/svg+xml" data="tangle-080.svg"></object>


<object type="image/svg+xml" data="tangle-081.svg"></object>


<object type="image/svg+xml" data="tangle-082.svg"></object>


<object type="image/svg+xml" data="tangle-083.svg"></object>


<object type="image/svg+xml" data="tangle-084.svg"></object>


<object type="image/svg+xml" data="tangle-085.svg"></object>


<object type="image/svg+xml" data="tangle-086.svg"></object>


<object type="image/svg+xml" data="tangle-087.svg"></object>


<object type="image/svg+xml" data="tangle-088.svg"></object>


<object type="image/svg+xml" data="tangle-089.svg"></object>


<object type="image/svg+xml" data="tangle-090.svg"></object>


<object type="image/svg+xml" data="tangle-091.svg"></object>


<object type="image/svg+xml" data="tangle-092.svg"></object>


<object type="image/svg+xml" data="tangle-093.svg"></object>

