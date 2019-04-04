---
title: Tangle -- Introduction
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

Like all WEB programs, it begins with a brief introduction.

<object type="image/svg+xml" data="tangle-001.svg"></object>

I think the “somewhat similar SAIL program” may have been “DOC”.

The “boostrapping“ refers to the fact that TANGLE is itself written in WEB, so to “translate” `tangle.web` into a `tangle.p` Pascal source file (before you have a working `tangle` program), it was originally needed to be done manually.

The “local Pascal compiler” was (what DEK called?) “Pascal-H”, as mentioned in the TeX program. As for “These features are also present”, I think we'd say “used” rather than “present”! Maybe DEK thinks of the source code along with the compiler (the entire process, from the author composing the text, to the actual assembly/binary instructions emitted) as “the program”.

An outline of the (initial part of) the whole program is given here in module 2.

<object type="image/svg+xml" data="tangle-002.svg"></object>


<object type="image/svg+xml" data="tangle-003.svg"></object>


<object type="image/svg+xml" data="tangle-004.svg"></object>


Disciplined use of `goto`, inventing control structures not present in Pascal (like break and continue and return, plus a few more).

<object type="image/svg+xml" data="tangle-005.svg"></object>


<object type="image/svg+xml" data="tangle-006.svg"></object>

Athough WEB papers over many of the defeciences of Pascal (as then standard), even DEK draws the line at default case labels.

<object type="image/svg+xml" data="tangle-007.svg"></object>

Unfortunately it may have been better to give these where they are defined, but here we have them all in one place.

<object type="image/svg+xml" data="tangle-008.svg"></object>


<object type="image/svg+xml" data="tangle-009.svg"></object>


<object type="image/svg+xml" data="tangle-010.svg"></object>

Exercise: Why don't the `mark_error` and `mark_fatal` cases need the `if` statement that `mark_harmless` does?
