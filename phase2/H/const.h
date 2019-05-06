#ifndef PHASE1_CONST_H_
#define PHASE1_CONST_H_

#define MAX_PROC 20

/* Memory */
#define RAMBASE *((unsigned int *)0x10000000)
#define RAMSIZE *((unsigned int *)0x10000004)
#define RAMTOP (RAMBASE + RAMSIZE)
#define FRAMESIZE 4096

/* Processor state */
#define ST_CUR_INTERRUPTS 1
#define ST_CUR_USER_MODE 1 << 1
#define ST_PREV_INTERRUPTS 1 << 2
#define ST_PREV_USER_MODE 1 << 3
#define ST_IM_ALL 0xFF00
#define ST_LCL_TIMER 1 << 27
#define ST_CP0 1 << 28

#endif
