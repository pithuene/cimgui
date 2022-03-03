Editor
======

An inline rich-text/markdown editor.

Editing operations are performed on a piecetable like structure.
A document consists of a list of blocks, each block that contains editable text also contains a list of pieces which point to text inside shared buffers.

Inline styles are stored as part of a piece, therefore, a piece only ever has one inline style.

A cursor references a piece and an offset inside it.
