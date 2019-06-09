#ifndef PHASE15_SYSBP_H_
#define PHASE15_SYSBP_H_

#include <const_rikaya.h>

/* New and old area for the syscall/breapoint handler */
#define SYSBP_NAREA 0x200003D4
#define SYSBP_OAREA 0x20000348

#define CAUSE_SYSCALL 8
#define CAUSE_BP      9

#define SPEC_TYPE_SYSBP 0
#define SPEC_TYPE_TLB   1
#define SPEC_TYPE_TRAP  2

/* System calls */
#define SYS1 1
#define SYS2 2
#define SYS3 3
#define SYS4 4
#define SYS5 5
#define SYS6 6
#define SYS7 7
#define SYS8 8
#define SYS9 9
#define SYS10 10

#endif
