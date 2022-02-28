Editor
======

An inline rich-text/markdown editor.

Editing operations are performed on a piecetable like structure.
A document consists of a list of blocks that contains editable text uses its own piecetable, whose pieces are references into shared buffers.

Inline styles are stored as part of a piece, therefore, a piece only ever has one inline style.
