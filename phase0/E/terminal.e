#ifndef FASE0_TERMINAL_E_
#define FASE0_TERMINAL_E_

#include <umps/types.h>
#include <lang.h>

/* Choose the terminal for standard output */
extern termreg_t *term_sel(uint8_t term);

/* Print char on given terminal */
extern bool term_putchar_sel(termreg_t *term, char c);

/* Print char on standard output */
extern bool term_putchar(char c);

/* Print string on given terminal */
extern void term_printstr_sel(termreg_t *term, const char *str);

/* Print string on standard output */
extern void term_printstr(const char *str);

#endif
