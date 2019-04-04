---
title: Tangle -- The big output switch
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


In the previous part we saw that the token stream is turned into an output buffer + some state (basically a queue), by calling three functions `send_out`, `send_val`, and `send_sign`.

In this part we'll see exactly when those three functions are called respectively (and with what arguments), i.e. the complex rules on what happens if Token X is followed by Token Y are all here.

The cases are:

----

- 0: (do nothing)

----

- `[A-Za-z]`: ident, 1

- IDENTIFIER: ident, k

----

- `[0-9]`: scan number, sendval(n), if E goto get_fraction

- `CHECK_SUM`: sendval

- OCTAL: sendval

- HEX: sendval

- NUMBER: sendval

- `.`:

  - If followed by another `.`, then str, 2

  - If followed by `[0-9]`, goto get_fraction

  - Followed by anything else: misc, `.`

----

- `+`, `-`: sign

----

- `AND_SIGN`: ident, 3

- `NOT_SIGN`: ident, 3

- IN: ident, 2

- OR: ident, 2

- `LEFT_ARROW`: ident, 2

- `NOT_EQUAL`: ident, 2

- `<=`

- `>=`

- `==`

- `..`

----

- `'`: str, k

----

- (other printable chars)

----

- `BEGIN_COMMENT`

- `END_COMMENT`

- `MODULE_NUMBER`

----

- `JOIN`

----

- `VERBATIM`

----

- `FORCE_LINE`

----

- everything else is an error, as it's an unprintable character

----

The above is incomplete in details (just read the code below), and can probably be better presented as a table of: if you've just seen token X, and you see token Y, what should you do.


<object type="image/svg+xml" data="tangle-112.svg"></object>


<object type="image/svg+xml" data="tangle-113.svg"></object>


<object type="image/svg+xml" data="tangle-114.svg"></object>


<object type="image/svg+xml" data="tangle-115.svg"></object>


<object type="image/svg+xml" data="tangle-116.svg"></object>


<object type="image/svg+xml" data="tangle-117.svg"></object>


<object type="image/svg+xml" data="tangle-118.svg"></object>


<object type="image/svg+xml" data="tangle-119.svg"></object>


<object type="image/svg+xml" data="tangle-120.svg"></object>


<object type="image/svg+xml" data="tangle-121.svg"></object>


<object type="image/svg+xml" data="tangle-122.svg"></object>


