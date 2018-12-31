#include <tests.e>

#include <tape.e>
#include <terminal.e>

/* First test that reads from a tape */
extern void test_read() {
  char buffer[TAPE_BLOCK_SIZE];
  uint32_t stat;
  tapereg_t *tape = tape_get(0); /* Get tape device */

  /* Reset tape */
  if ((stat = tape_rewind(tape)) != TAPE_ST_READY) {
    term_printf("error status: %d", stat);
    return;
  }
  term_printf("---- tape start ----\n");

  /* Read till the end */
  while (!tape_eot(tape)) {
    if ((stat = tape_readblock(tape, buffer)) != TAPE_ST_READY) {
      term_printf("error status: %d", stat);
      return;
    }
    term_printf("%s\n%s\n\n",
      buffer, tape_eot(tape) ? "---- end of tape ----" : 
      tape_eob(tape) ? "---- end of block ----" : "---- end of file ----");
  }
}

/* Hidden function that moves the tape from one block index to another. Indexes
 * are one-based(since zero is used as 'termination' character)
 */
HIDDEN uint32_t tape_goto(tapereg_t *tape, uint32_t current, uint32_t next) {
  int delta = next - current,
      i;
  uint32_t stat = tape->status;

  /* Choose if the tape needs to skip forward or backward based on the delta value */
  if (delta > 0) {
    for (i = delta; i > 0; i--)
      if ((stat = tape_skipblock(tape)) != TAPE_ST_READY)
        return stat;
  }
  else if (delta < 0) {
    for (i = delta; i < 0; i++)
      if ((stat = tape_backblock(tape)) != TAPE_ST_READY)
        return stat;
  }
  return stat;
}

/* Second test, using two tapes */
extern void test_index() {
  char buffer_ind[TAPE_BLOCK_SIZE];
  char buffer_dict[TAPE_BLOCK_SIZE];
  uint32_t index = 1,
           stat;
  tapereg_t *tape_dict = tape_get(1);
  tapereg_t *tape_ind = tape_get(2);
  int i;

  /* Reset tapes */
  if ((stat = tape_rewind(tape_dict)) != TAPE_ST_READY) {
    term_printf("error status: %d", stat);
    return;
  }

  if ((stat = tape_rewind(tape_ind)) != TAPE_ST_READY) {
    term_printf("error status : %d", stat);
    return;
  }

  /* Read from the index tape till you can */
  while (!tape_eot(tape_ind)) {
    if ((stat = tape_readblock(tape_ind, buffer_ind)) != TAPE_ST_READY) {
      term_printf("error status: %d", stat);
      return;
    }
    
    /* For each non-zero value on the index tape, use the value to index the
     * dictionary tape
     */
    for (i = 0; i < TAPE_BLOCK_SIZE && buffer_ind[i] != 0; i++) {
      /* Update tape_dict position */
      if ((stat = tape_goto(tape_dict, index, buffer_ind[i])) != TAPE_ST_READY) {
        term_printf("error status : %d", stat);
        return;
      }
      index = buffer_ind[i] + 1; /* +1 since we're going to read from that block,
                                  * and reading a block increases the position by 1
                                  */

      /* Read from the selected block */
      if ((stat = tape_readblock(tape_dict, buffer_dict)) != TAPE_ST_READY) {
        term_printf("error status : %d", stat);
        return;
      }
      term_printf("%s\n", buffer_dict);
    }
  }
}
