#ifndef PHASE15_INTERRUPT_H_
#define PHASE15_INTERRUPT_H_

#include <umps/arch.h>

#define INTERRUPT_NAREA 0x2000008C
#define INTERRUPT_OAREA 0x20000000

#define TERM_LINE DEV_REG_ADDR(IL_TERMINAL, 0)
#define DEV_LINE_BASE DEV_REG_ADDR(3, 0)
#define DEV_LINE_SIZE 0x10

/* ACKs are 1 for every device */
#define DEV_CMD_ACK 1

/* Status codes 0 - 4 are the same for all devices */
#define DEV_ST_NOT_INSTALLED 0
#define DEV_ST_READY 1

#define INTER_DEVICES_BASE 0x1000003C
#define INTER_DEVICES(line) (INTER_DEVICES_BASE + (line - 3) * WS)

#endif
