---
title: Tangle -- Stacks for output
---

<link rel="stylesheet" href="common.css">

<style>
</style>

<object type="image/svg+xml" data="tangle-077.svg"></object>

This struck me as a strange choice when I first encountered it: without saying how these data structures will be populated, we're describing how they'll produce the program? Basically describing Phase 2 before Phase 1? But now it makes sense and seems familiar, as the way DEK writes programs.

----

Let's summarize what we have so far:

- The "bytes" of the program (names of modules, names of macros, names of identifiers, and double-quoted strings) are written into `byte_mem`, with indexes in `byte_start`.

- The "tokens" of the program (the replacement text for simple and parametric macros, the replacement text for modules) are written into `tok_mem`, with indexes in `tok_start`.

- We have ways of looking up either modules or identifiers (any of the others) by name.

- We have the numeric equivalents (for numeric macros and double-quoted strings) and text equivalents (for simple macros, parametric macros, modules) (and the latter of these we can follow "to be continued" links for, using `text_link`).

- Given a name (or when looking at the replacement text for a macro / module), we can say what type it is.

Here is a visualization of some of all of this, for the memory at the end of phase one of reading POOLTYPE.web.

<style>
#namesAndEquivsDiv {
    width: 30%;
}
#textsDiv {
    width: 70%;
}
</style>
<div class="full-width">
<div class="areaofsanity">
 <div id="listNamesAndTexts" class="hbox">
  <div id="namesAndEquivsDiv"></div>
  <div id="textsDiv" class="vbox"></div>
 </div>
</div>
</div>
<script src="tangle-mem.js"></script>
<script>
namesAndEquivs(document.getElementById('namesAndEquivsDiv'));
tokMemListTextsResolved(document.getElementById('textsDiv'));
</script>

So in principle to output the entire program, this is what we'd need to do:

- Locate the replacement text for the unnamed module (it starts in `text_link[0]`), and start writing it out.

- When you encounter an identifier with a numeric equivalent (a numeric macro, or a double-quoted string), replace it with its numeric equivalent.

- When you encounter a module name, start (recursively) writing out *its* replacement text.

- When you encounter a simple macro, write out its replacement text.

- When you encounter a parametric macro, identify the parameter (where it begins and ends) and keep track of it, and write out the replacement text, replacing any encountered `#` with the parameter.


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

Here, “above” = “after” (i.e. greater indices in the array). And the way we avoid needing a separate stack for keeping track of parameters is this: When we encounter a macro (i.e. a macro call), we scan its parameter (argument) immediately (which occurs immediately after the macro call) and add it as a new text, with a new (dummy) name pointing to it. So, next, when actually expanding the definition of the macro, the last name always points to the parameter (argument) to the macro.


<object type="image/svg+xml" data="tangle-082.svg"></object>


<object type="image/svg+xml" data="tangle-083.svg"></object>


Read the following `push_level(p)` as: “start reading name *p*”.

<object type="image/svg+xml" data="tangle-084.svg"></object>


<object type="image/svg+xml" data="tangle-085.svg"></object>


<object type="image/svg+xml" data="tangle-086.svg"></object>


<object type="image/svg+xml" data="tangle-087.svg"></object>

The cases are:

 * base case / terminal case: if we've reached the end (of everything), return 0.

 * if we've reached the end of *current* text, `pop_level` (and return `{:module_number}` if necessary).

 * Most common case: read the next token, (if it's a macro act on it), and return it.


<object type="image/svg+xml" data="tangle-088.svg"></object>

Hmm, what about the a = 0? We'd return token 0. Can it even happen? I need to think about this.


<object type="image/svg+xml" data="tangle-089.svg"></object>

Read “put a parameter on the parameter stack” as “look for an argument, scan it, append it to the texts array”.


<object type="image/svg+xml" data="tangle-090.svg"></object>

Here “character” = “token”. The “copy the parameter into `tok_mem`” means: (1) add a new text, (2) add a new name that points to it.

Note that in case of `debug`, we add a nonempty name (namely `#`) (which is correspondingly undone by `pop_level`), else the new name is an empty one, just pointing to the parameter.


<object type="image/svg+xml" data="tangle-091.svg"></object>


The following is a really clever idea!

<object type="image/svg+xml" data="tangle-092.svg"></object>

— i.e., to start writing out the parameter, treat it as an expansion of the last name. (Note: this is not the macro expansion! This is just a dummy-string → argument “expansion”.)


<object type="image/svg+xml" data="tangle-093.svg"></object>

Why `name_ptr + '77777` (assumes 16-bit 2s complement) instead of `name_ptr - 1`? Need to think about this.