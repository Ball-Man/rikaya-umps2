#include <tlb.e>
#include <tlb.h>

#include <umps/libumps.h>
#include <lang.e>
#include <sysbp.e>
#include <sysbp.h>

extern void tlb() {
  state_t *old_area = (state_t *)TLB_OAREA;

  if (!spec_set[SPEC_TYPE_TLB])
    Terminate_Process(0);

  /* Nothing to do here, call superior handler */
  memcpy(old_area, spec_oarea[SPEC_TYPE_TLB], sizeof(state_t));
  LDST(spec_narea[SPEC_TYPE_TLB]);
}
