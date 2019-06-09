#include <trap.e>
#include <trap.h>

#include <umps/libumps.h>
#include <lang.e>
#include <sysbp.e>
#include <sysbp.h>

extern void trap() {
  state_t *old_area = (state_t *)TRAP_OAREA;

  if (!spec_set[SPEC_TYPE_TRAP])
    Terminate_Process(0);

  /* Nothing to do here, call superior handler */
  memcpy(old_area, spec_oarea[SPEC_TYPE_TRAP], sizeof(state_t));
  LDST(spec_narea[SPEC_TYPE_TRAP]);
}
