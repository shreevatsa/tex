---
title: Tangle -- Producing the output
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


(TODO: Comment on these sections later.)

1. We don't write directly to the output file but into an output buffer (`out_buf`) that at any time holds up to `out_buf_size = 2 * line_length` elements. This number is called `out_ptr`, so:

        $$0 \le \texttt{out_ptr} \le \textt{out_buf_size} = \texttt{2 \times line_length}$$

2. We explicitly keep track of the (last) places where a break is acceptable:

        $$0 \le textt{semi_ptr} \le \texttt{break_ptr} \le \min(\texttt{out_ptr}, \texttt{line_length})$$

3. For numbers and signs, we don't put them in `out_buf` directly, but accumulate them into `{out_state, out_val, out_app, out_sign, last_sign}`.

Another way of putting the last point above in context:

    [compact data strctures] ---(get_output)---> [linear stream of non-macro tokens] ---(send_out etc)--> [out_buf, out_state, etc] ---(flush_buffer)--> [output_file]

<object type="image/svg+xml" data="tangle-094.svg"></object>

(This `out_buf_size` is twice `line_length`.)

Cases to think about:

| Input                | Output                  |        Comments        |
| :------------------- | :---------------------- | :--------------------: |
| E - 15 + 17          | E + 2                   |    add the numbers     |
| E - 15 + 17 * y      | itself: E - 15 + 17 * y |                        |
| 1E - 15 + 17         | itself: 1E - 15 + 17    |                        |
| -15 + 17.5           | itself: -15 + 17.5      | only integer constants |
| x * y (where y = -2) | x * (-2)                |                        |
| x - y (where y = -2) | x + 2                   |                        |
| x - 0.1              | itself: x - 0.1         |   need to remember -   |


<object type="image/svg+xml" data="tangle-095.svg"></object>

We can think of these states as keeping track of a queue of what has been seen, but not yet output:

- after reading `(` or after reading the entire `(x-15+19-2)`: `misc`: the queue is empty.

- after reading `x`: `num_or_id`: the queue contains a note to break if necessary. (We don't actually insert the break rightaway, because this number may turn out to be part of a “fraction” like 6.022E23, or the user may ask for a “join” (`@&`) next.)

- after reading `t@&`: `unbreakable`: the queue contains a note to *not* insert a break, i.e. to “join” (`@&`).

- after reading `x-`: `sign`: the queue contains a sign, namely the sign of `out_app` (actually we'll always have `out_app` be ±1 at this point, I think).

- after reading `x-15`: `sign_val`: the queue contains: (1) `-1` or `out_sign` (` ` or `+` or empty), (2) `|out_val|`.

- after reading `x-15+`: `sign_val_sign`: the queue contains: (1) `-1` or `out_sign`, (2) `|out_val|`, (3) `sgn(out_app)`

- after reading `x-15+19` or after reading `x-15+19-2`: `sign_val_val`: the queue contains: (1) `-1` or `out_sign`, (2) `|out_val|`, (3) `out_app`.

It's a bit annoying that these states have been given numbers such that the values matter, but there's not much to it and it can be easily rewritten if desired (see section 106).

<object type="image/svg+xml" data="tangle-096.svg"></object>

The `flush_buffer` procedure defined below has (only) two callers: one is the `check_break` macro defined below, which is called various times after printing various stuff that makes a break possible. The other is the section 98 below.

<object type="image/svg+xml" data="tangle-097.svg"></object>

Interesting that in case of error it just proceeds, having printed only part of the line.

This section below is the other caller of the `flush_buffer` procedure defined just above, and is called after the last line has been output (because the last line needs to be flushed too).

<object type="image/svg+xml" data="tangle-098.svg"></object>


<object type="image/svg+xml" data="tangle-099.svg"></object>

^ Don't we need a check for `k ≥ out_ptr`? What happens if they overlap?

<object type="image/svg+xml" data="tangle-100.svg"></object>


<object type="image/svg+xml" data="tangle-101.svg"></object>


<object type="image/svg+xml" data="tangle-102.svg"></object>


<object type="image/svg+xml" data="tangle-103.svg"></object>


<object type="image/svg+xml" data="tangle-104.svg"></object>


<object type="image/svg+xml" data="tangle-105.svg"></object>


<object type="image/svg+xml" data="tangle-106.svg"></object>

^Note: First line: `out_app` was ±1 (I think), so it makes sense to multiply `v` by it.

Question: Does `send_sign` after `unbreakable` result in the `unbreakable` getting ignored? Is this documented in the WEB manual?

Below: read the `bad_case` as something like `dump_case`. There's nothing “bad” about it AFAICT.

<object type="image/svg+xml" data="tangle-107.svg"></object>


<object type="image/svg+xml" data="tangle-108.svg"></object>


<object type="image/svg+xml" data="tangle-109.svg"></object>


<object type="image/svg+xml" data="tangle-110.svg"></object>


<object type="image/svg+xml" data="tangle-111.svg"></object>


