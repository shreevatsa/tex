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


<object type="image/svg+xml" data="tangle-094.svg"></object>

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


<object type="image/svg+xml" data="tangle-096.svg"></object>


<object type="image/svg+xml" data="tangle-097.svg"></object>


<object type="image/svg+xml" data="tangle-098.svg"></object>


<object type="image/svg+xml" data="tangle-099.svg"></object>


<object type="image/svg+xml" data="tangle-100.svg"></object>


<object type="image/svg+xml" data="tangle-101.svg"></object>


<object type="image/svg+xml" data="tangle-102.svg"></object>


<object type="image/svg+xml" data="tangle-103.svg"></object>


<object type="image/svg+xml" data="tangle-104.svg"></object>


<object type="image/svg+xml" data="tangle-105.svg"></object>


<object type="image/svg+xml" data="tangle-106.svg"></object>


<object type="image/svg+xml" data="tangle-107.svg"></object>


<object type="image/svg+xml" data="tangle-108.svg"></object>


<object type="image/svg+xml" data="tangle-109.svg"></object>


<object type="image/svg+xml" data="tangle-110.svg"></object>


<object type="image/svg+xml" data="tangle-111.svg"></object>


