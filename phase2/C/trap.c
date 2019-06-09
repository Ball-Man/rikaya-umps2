#include <trap.e>
#include <trap.h>

#include <umps/libumps.h>
#include <lang.e>
#include <sysbp.e>
#include <sysbp.h>
#include <scheduler.e>

extern void trap() {
  state_t *old_area = (state_t *)TRAP_OAREA;
  old_area->pc_epc += WORD_SIZE;

  if (!cur_proc->spec_set[SPEC_TYPE_TRAP])
    Terminate_Process(0);

  /* Nothing to do here, call superior handler */
  memcpy(old_area, cur_proc->spec_oarea[SPEC_TYPE_TRAP], sizeof(state_t));
  LDST(cur_proc->spec_narea[SPEC_TYPE_TRAP]);
}
