#include <terminal.e>
#include <terminal.h>

#include <umps/arch.h>
#include <umps/types.h>

/* Term used as standard output */
HIDDEN termreg_t *term_default;

/* Get the terminal status */
HIDDEN uint32_t term_status(termreg_t *term) {
  return (term->transm_status) & TERM_STATUS_MASK;
}

/* Select the terminal id for the standard output */
extern termreg_t *term_sel(uint8_t term) {
  return term_default = (termreg_t *) DEV_REG_ADDR(IL_TERMINAL, 0);
}

/* Print string on given terminal */
extern bool term_putchar_sel(termreg_t *term, char c) {
  uint32_t stat;

  /* If the device isn't ready, return */
  stat = term_status(term);
  if (stat != TERM_ST_READY && stat != TERM_ST_TRANSMITTED)
    return false;

  term->transm_command = ((c << TERM_CHAR_OFFSET) | TERM_CMD_TRANSMIT);

  /* Wait while the device works */
  while ((stat = term_status(term)) == TERM_ST_BUSY);

  term->transm_command = TERM_CMD_ACK;

  if (stat == TERM_ST_TRANSMITTED)
    return true;
  return true;
}

/* Print char on standard output */
extern bool term_putchar(char c) {
  return term_putchar_sel(term_default, c);
}

/* Print string on given terminal */
extern void term_printstr_sel(termreg_t *term, const char *str) {
  while (*str)
    if (!term_putchar_sel(term, *str++))
      return;
}

/* Print string on standard output */
extern void term_printstr(const char *str) {
  while (*str)
    if (!term_putchar(*str++))
      return;
}
