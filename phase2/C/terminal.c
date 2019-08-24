#include <terminal.e>
#include <terminal.h>

#include <umps/arch.h>
#include <umps/types.h>
#include <stdarg.h>

/* Term used as standard output */
HIDDEN termreg_t *term_default;

/* Get the terminal status */
HIDDEN uint32_t term_status(termreg_t *term) {
  return (term->transm_status) & TERM_STATUS_MASK;
}

/* Select the terminal id for the standard output */
extern termreg_t *term_sel(uint8_t term) {
  return term_default = (termreg_t *) DEV_REG_ADDR(IL_TERMINAL, term);
}

/* Print string on given terminal. Returns the last status from the device */
extern uint32_t term_putchar_sel(termreg_t *term, char c) {
  uint32_t stat;

  /* If the device isn't ready, return */
  stat = term_status(term);
  if (stat != TERM_ST_READY && stat != TERM_ST_TRANSMITTED)
    return stat;

  term->transm_command = ((c << TERM_CHAR_OFFSET) | TERM_CMD_TRANSMIT);

  /* Wait while the device works */
  while ((stat = term_status(term)) == TERM_ST_BUSY);

  term->transm_command = TERM_CMD_ACK;

  return stat;
}

/* Print char on standard output. Returns the last status from the device */
extern uint32_t term_putchar(char c) {
  return term_putchar_sel(term_default, c);
}

/* Print string on given terminal. Returns the lsat status from the device */
extern uint32_t term_printstr_sel(termreg_t *term, const char *str) {
  uint32_t stat;

  while (*str)
    if ((stat = term_putchar_sel(term, *str++)) != TERM_ST_TRANSMITTED)
      return stat;
  return stat;
}

/* Print string on standard output. Returns the last status from the device */
extern uint32_t term_printstr(const char *str) {
  return term_printstr_sel(term_default, str);
}

/* Print an integer on given terminal. Returns the last status from the device */
extern uint32_t term_printi_sel(termreg_t *term, int n) {
  uint32_t stat;

  /* I guess no integer would ever be more than 64 digits long */
  uint8_t buff[64];
  int i;

  /* Print a - if the number is negative */
  if (n < 0 && (stat = term_putchar_sel(term, '-')) != TERM_ST_TRANSMITTED)
    return stat;

  /* Save digits in buffer */
  i = 0;
  do
    buff[i++] = n % 10;
  while (n /= 10);

  for (i--; i >= 0; i--)
    if ((stat = term_putchar_sel(term, buff[i] + '0')) != TERM_ST_TRANSMITTED)
      return stat;
  return stat;
}

/* Hidden function which actually does all the job for printf, This function is needed due to
 * the way C manages varargs.
 */
HIDDEN uint32_t _term_printf_sel(termreg_t *term, const char *format, va_list args) {
  uint32_t stat;

  for (; *format != '\0'; format++)
    if (*format == '%')
      switch (*(++format)) {
        case 'c':
          if ((stat = term_putchar_sel(term, va_arg(args, int))) != TERM_ST_TRANSMITTED)
            return stat;
          break;

        case 's':
          if ((stat = term_printstr_sel(term, va_arg(args, char *))) != TERM_ST_TRANSMITTED)
            return stat;
          break;

        case 'd':
          if ((stat = term_printi_sel(term, va_arg(args, int))) != TERM_ST_TRANSMITTED)
            return stat;
          break;

        default:
          if ((stat = term_putchar_sel(term, *format)) != TERM_ST_TRANSMITTED)
            return stat;
          break;
      }
    else if ((stat = term_putchar_sel(term, *format)) != TERM_ST_TRANSMITTED)
      return stat;

  return stat;
}

/* Print string formatted in a similar way to the standard C printf. For
 * details see terminal.e.
 * Returns the last status from the device
 */
extern uint32_t term_printf_sel(termreg_t *term, const char *format, ...) {
  uint32_t stat;

  va_list args;
  va_start(args, format);

  stat = _term_printf_sel(term, format, args);

  va_end(args);

  return stat;
}

/* Print string formatted in a similar way to the standard C printf on the standard output.
 * For details see terminal.e.
 * Returns the last status from the device
 */
extern uint32_t term_printf(const char *format, ...) {
  uint32_t stat;

  va_list args;
  va_start(args, format);

  stat = _term_printf_sel(term_default, format, args);

  va_end(args);

  return stat;
}
