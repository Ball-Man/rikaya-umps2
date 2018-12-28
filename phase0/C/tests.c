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
