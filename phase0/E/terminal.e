#ifndef PHASE0_TERMINAL_E_
#define PHASE0_TERMINAL_E_

#include <umps/types.h>
#include <lang.h>

/* Choose the terminal for standard output */
extern termreg_t *term_sel(uint8_t term);

/* Print char on given terminal. Returns the last status from the device */
extern uint32_t term_putchar_sel(termreg_t *term, char c);

/* Print char on standard output. Returns the last status from the device */
extern uint32_t term_putchar(char c);

/* Print string on given terminal. Returns the last status from the device */
extern uint32_t term_printstr_sel(termreg_t *term, const char *str);

/* Print string on standard output. Returns the last status from the device */
extern uint32_t term_printstr(const char *str);

/* Print an integer on given terminal. Returns the last status from the device */
extern uint32_t term_printi_sel(termreg_t *term, int n);

/* Print an integer on standard output. Returns the last status from the device */
extern uint32_t term_printi(int n);

/* Print a string with arguments using a specific format.
 * The format string is similar to the C standard printf
 * function. However, since this function is mainly used for
 * test/debug output(so far) only some simple options are available:
 *    %d      integer argument
 *    %c      character argument
 *    %s      C string argument(with trailing \0)
 *    %%      to print a % symbol
 */
extern uint32_t term_printf_sel(termreg_t *term, const char *format, ...);

/* If you read all the comments in this module you probably know what this function does...
 * Prints a string using a specific format(detail above) on standard output. Returns the 
 * last status from the device
 */
extern uint32_t term_printf(const char *format, ...);

#endif
