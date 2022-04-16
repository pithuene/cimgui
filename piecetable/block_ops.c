// Functions mainly operating on blocks

#include "block_ops.h"
#include "piece_ops.h"
#include <assert.h>
#include <stdio.h>

// TODO: Import only needed for editor_ensure_not_empty. Would be nice if it could be removed.
#include "editor_ops.h"

void block_append_pieces(editor_t *ed, block_t *block, piecetable_piece_t *first, piecetable_piece_t *last) {
  assert(first != NULL);
  assert(last != NULL);

  block_remove_terminator(ed, block);

  first->prev = block->last_piece;
  if (block->last_piece) {
    block->last_piece->next = first;
  }
  if (!block->first_piece) {
    block->first_piece = first;
  }
  block->last_piece = last;
  last->next = NULL;
  if (!piece_is_blockterminator(ed, last)) {
    piecetable_piece_t *blockterminator = editor_create_new_blockterminator(ed);
    block->last_piece->next = blockterminator;
    blockterminator->prev = block->last_piece;
    block->last_piece = blockterminator;
  }
}

bool block_remove_terminator(editor_t *ed, block_t *block) {
  if (block->last_piece && piece_is_blockterminator(ed, block->last_piece)) {
    piecetable_piece_t *blockterminator = block->last_piece;
    block->last_piece = blockterminator->prev;
    if (blockterminator->prev) {
      blockterminator->prev->next = NULL;
    } else {
      block->first_piece = NULL;
    }
    free(blockterminator);
    return true;
  }
  return false;
}

void editor_insert_block_after(editor_t *ed, block_t *after, block_t *new_block) {
  if (after->next) {
    after->next->prev = new_block;
    new_block->next = after->next;
  } else {
    ed->last = new_block;
  }
  after->next = new_block;
  new_block->prev = after;
}

void editor_delete_block(editor_t *ed, block_t *block) {
  if (block->prev) {
    block->prev->next = block->next;
  } else {
    ed->first = block->next;
  }

  if (block->next) {
    block->next->prev = block->prev;
  } else {
    ed->last = block->prev;
  }

  free(block);

  editor_ensure_not_empty(ed);
}

bool editor_block_is_empty(editor_t *ed, block_t *block) {
  // Block is completely empty
  if (!block->first_piece) return true;

  if (block->first_piece == block->last_piece // Block contains a single piece
      && piece_is_blockterminator(ed, block->first_piece)) {
    return true;
  }

  return false;
}

void editor_split_block_at_cursor(editor_t *ed, editor_cursor_t *cursor) {
  // The last content piece that remains in the cursor block
  piecetable_piece_t *first = NULL;
  // The first piece of the new block 
  piecetable_piece_t *second = NULL;

  split_piece_at_cursor(cursor, &first, &second);

  // Create and insert the new block
  second->prev = NULL;
  
  block_t *new_block = editor_copy_block(cursor->block);
  new_block->first_piece = second;
  new_block->last_piece = cursor->block->last_piece;

  editor_insert_block_after(ed, cursor->block, new_block);

  // Append a new block_terminator to the first block and fix all links
  piecetable_piece_t *first_blockterminator = editor_create_new_blockterminator(ed);
  first_blockterminator->prev = first;
  cursor->block->last_piece = first_blockterminator;
  if (first) {
    first->next = first_blockterminator;
  } else {
    // Split occured at the beginning of a block
    cursor->block->first_piece = first_blockterminator;
  }

  // Place cursor at beginning of new block
  *cursor = (editor_cursor_t){
    .block = new_block,
    .piece = second,
    .offset = 0,
  };
}

void editor_block_check_health(block_t *block) {
  if (!block) {
    printf("Checking health of null block!\n");
    return;
  }
  if (block->first_piece && !block->last_piece) {
    printf("Checked block has a first piece but no last piece!\n");
  }
  if (block->last_piece && !block->first_piece) {
    printf("Checked block has a last piece but no first piece!\n");
  }
  if (block->first_piece->prev != NULL) {
    printf("First piece of checked block has a prev of %p!\n", (void *) block->first_piece->prev);
  }
  if (block->last_piece->next != NULL) {
    printf("Last piece of checked block has a next of %p!\n", (void *) block->last_piece->next);
  }

  piecetable_piece_t *curr = block->first_piece;
  while (curr) {
    if (curr->next) {
      if (curr->next->prev != curr) {
        printf(
          "Piece of editor block %p broken. next->prev points to %p instead of %p.\n",
          (void*) block,
          (void*) curr->next->prev,
          (void*) curr
        );
      }
    }
    curr = curr->next;
  }
}

void editor_block_turn_into(editor_t *ed, block_t *old, block_t *new_block) {
  new_block->first_piece = old->first_piece;
  new_block->last_piece = old->last_piece;

  new_block->next = old->next;
  new_block->prev = old->prev;

  if (old->prev) {
    old->prev->next = new_block;
  } else {
    ed->first = new_block;
  }

  if (old->next) {
    old->next->prev = new_block;
  } else {
    ed->last = new_block;
  }

  free(old);
}

block_heading_t *editor_create_block_heading(editor_t *ed, uint8_t level, piecetable_piece_t *first, piecetable_piece_t *last) {
  block_heading_t *new_heading = (block_heading_t*) malloc(sizeof(block_heading_t));
  *new_heading = (block_heading_t){
    .block = {
      .type = blocktype_heading,
      .first_piece = first,
      .last_piece = last,
    },
    .level = level,
  };
  return new_heading;
}

block_bullet_t *editor_create_block_bullet(editor_t *ed, uint8_t indentation_level, piecetable_piece_t *first, piecetable_piece_t *last) {
  block_bullet_t *new_bullet = (block_bullet_t*) malloc(sizeof(block_bullet_t));
  *new_bullet = (block_bullet_t){
    .block = {
      .type = blocktype_bullet,
      .first_piece = first,
      .last_piece = last,
    },
    .indentation_level = indentation_level,
  };
  return new_bullet;
}

block_paragraph_t *editor_create_block_paragraph(editor_t *ed, piecetable_piece_t *first, piecetable_piece_t *last) {
  block_paragraph_t *new_paragraph = (block_paragraph_t*) malloc(sizeof(block_paragraph_t));
  *new_paragraph = (block_paragraph_t){
    .block = {
      .type = blocktype_paragraph,
      .first_piece = first,
      .last_piece = last,
    }
  };
  return new_paragraph;
}

block_t *editor_copy_block(block_t *block) {
  switch (block->type) {
    case blocktype_bullet: {
      block_bullet_t *new_block = (block_bullet_t *) malloc(sizeof(block_bullet_t));
      *new_block = *((block_bullet_t *) block);
      return (block_t *) new_block;
    }
    case blocktype_heading: {
      block_heading_t *new_block = (block_heading_t *) malloc(sizeof(block_heading_t));
      *new_block = *((block_heading_t *) block);
      return (block_t *) new_block;
    }
    case blocktype_paragraph: // Fallthrough
    default: {
      block_paragraph_t *new_block = (block_paragraph_t *) malloc(sizeof(block_paragraph_t));
      *new_block = *((block_paragraph_t *) block);
      return (block_t *) new_block;
    }
  }
}

