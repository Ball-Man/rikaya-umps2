#include <tape.e>
#include <terminal.e>
#include <umps/arch.h>
#include <umps/libumps.h>
#include <lang.h>

void main(void)
{
  char buffer[TAPE_BLOCK_SIZE];

  term_sel(0); /* Select standard output */

  tapereg_t *tape = tape_get(0); /* Get tape device */
  while (!tape_eot(tape)) {
    tape_readblock(tape, buffer);
    term_printf("block content:\n%s\ntape end? %s\n",
      buffer, tape_eot(tape) ? "yes" : "no");
  }

  WAIT();
  *((uint32_t *) MCTL_POWER) = 0x0FF;
  while (1) ;
}
