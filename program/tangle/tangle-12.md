---
title: Tangle -- Introduction to the input phase
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

The main concerns of this chapter are:

- (123) Mentioning the existence of `get_next` (in the next chapter), `skip_ahead`, and `skip_comment`.

- (124 to 138) Merging the web file and change file

- (139) Function for converting an `@&` (etc.) control code to an internal (8-bit) code

- (140) Skipping ahead to the next control code

- (141) When in a comment, skipping to end of comment

Correspondingly, this part/chapter contains the following that are used outside this part:

- Globals defined here and used outside this section: `line`, `limit`, `loc` related to the current line number, its length, and how much of it has been read,

- Procedure `get_line`, which puts the next line (from either the WEB file or the .ch changefile) into `buffer` (and updates `limit` etc).

- Function `control_code`, which translates from occurrences of `@*` in the input (for various values of `*`) to corresponding tokens(?)

- Function `skip_ahead` and procedure `skip_comment`, which find the next control code and skip to end of comment, respectively.


<object type="image/svg+xml" data="tangle-123.svg"></object>


<object type="image/svg+xml" data="tangle-124.svg"></object>


<object type="image/svg+xml" data="tangle-125.svg"></object>


<object type="image/svg+xml" data="tangle-126.svg"></object>


<object type="image/svg+xml" data="tangle-127.svg"></object>

Below,

- the module name “Skip over comment lines in the change file...” can be read as “Look in the change file until finding a line starting with `@x`”.

- the module name “Skip to the next nonblank line...” can be read as “Look in the change file for the first nonblank line following the current line that starts with `@x`”.

- the module “Move `buffer` and `limit` to `change_buffer` and `change_limit`” is because `input_ln` puts the just-input line and its length in those variables, but we want to keep them in `change_buffer` and `change_limit` instead.

<object type="image/svg+xml" data="tangle-128.svg"></object>


<object type="image/svg+xml" data="tangle-129.svg"></object>


<object type="image/svg+xml" data="tangle-130.svg"></object>


<object type="image/svg+xml" data="tangle-131.svg"></object>

This `check_change` below is another top-level procedure.

<object type="image/svg+xml" data="tangle-132.svg"></object>


<object type="image/svg+xml" data="tangle-133.svg"></object>


<object type="image/svg+xml" data="tangle-134.svg"></object>


Perhaps the main procedure “exported” from this section/chapter is the `get_line` function below.

<object type="image/svg+xml" data="tangle-135.svg"></object>


<object type="image/svg+xml" data="tangle-136.svg"></object>


<object type="image/svg+xml" data="tangle-137.svg"></object>


<object type="image/svg+xml" data="tangle-138.svg"></object>


<object type="image/svg+xml" data="tangle-139.svg"></object>


<object type="image/svg+xml" data="tangle-140.svg"></object>


<object type="image/svg+xml" data="tangle-141.svg"></object>


<object type="image/svg+xml" data="tangle-142.svg"></object>


