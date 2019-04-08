---
title: Tangle -- Inputting the next token
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


This entire chapter is about the `get_next` function (defined from 145 to 155, the end), which gets the next token of input.

This function produces the next token in *almost* the form that ultimately goes into the data structures (`tok_mem` etc -- what we saw in chapters 5 to 8) except that instead of two-byte tokens for identifiers, module names and module numbers, it instead returns a certain special one-byte value and then other procedures (which?) turn them into the actual two-byte tokens.


<object type="image/svg+xml" data="tangle-143.svg"></object>


<object type="image/svg+xml" data="tangle-144.svg"></object>


<object type="image/svg+xml" data="tangle-145.svg"></object>


<object type="image/svg+xml" data="tangle-146.svg"></object>


<object type="image/svg+xml" data="tangle-147.svg"></object>


<object type="image/svg+xml" data="tangle-148.svg"></object>


<object type="image/svg+xml" data="tangle-149.svg"></object>


<object type="image/svg+xml" data="tangle-150.svg"></object>


<object type="image/svg+xml" data="tangle-151.svg"></object>


<object type="image/svg+xml" data="tangle-152.svg"></object>


<object type="image/svg+xml" data="tangle-153.svg"></object>


<object type="image/svg+xml" data="tangle-154.svg"></object>


<object type="image/svg+xml" data="tangle-155.svg"></object>


