#ifndef PHASE15_SYSBP_E_
#define PHASE15_SYSBP_E_

/* Main handler for system calls and breakpoints */
extern void sysbp();

/* Handler for sys3: terminate current process and its tree */
extern void sys3();

#endif
