ops
===

Operations are what widget functions emit.
They encapsulate the available drawing functionality of nanovg, but enable position independant drawing, by "drawing" into an operation list and executing the list at a later point with some offset.
