editor-core
===========

The editing engine for a structured richtext editor compatible with a flavor of markdown.
This module is strictly separated from any user-interface components because it is supposed to be used in non-interactive environments aswell.

The basic data structure is an extension of a piecetable text editor.
A document consists of a doubly-linked list of blocks, each of which contains a doubly-linked list of pieces.
The pieces reference a span of UTF-8 encoded runes which are stored in buffers on the document.

A cursor references a block, a piece and an offset inside it.

<!-- TODO: Inline styles are stored as part of a piece, therefore, a piece only ever has one inline style. -->

Every block ends with a piece containing a single null byte.
The cursor is placed on this piece to append text to the end.

Blocks are never empty, they always contain at least this one piece referencing a null byte.
The editor is also never empty and contains at least one block at all times.

The are functions to import markdown based on [md4c](https://github.com/mity/md4c).
