#ifndef PHASE1_CONST_H_
#define PHASE1_CONST_H_

#define MAX_PROC 20

#define RAMBASE *((unsigned int *)0x10000000)
#define RAMSIZE *((unsigned int *)0x10000004)
#define RAMTOP (RAMBASE + RAMSIZE)

#endif
