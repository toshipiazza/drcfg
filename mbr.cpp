#include "dr_api.h"
#include "mbr.h"
#include "cfg_impl.h"
#include "app.h"
#include "droption.h"

#include <cstdint>

#ifndef ARM
static droption_t<bool> instrument_ret
(DROPTION_SCOPE_CLIENT, "instrument_ret", false,
 "Count return instructions as control flow instructions", "");
#endif

static void
at_cti(uintptr_t src, uintptr_t targ)
{
    safe_insert(src, targ);
}

dr_emit_flags_t
cti_event_app_instruction(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
                          bool for_trace, bool translating, void *user_data)
{
    if (!instr_is_mbr(instr))
        return DR_EMIT_DEFAULT;
#ifndef ARM
    if (instr_is_cbr(instr)) {
        // we handle cti's more efficiently in cbr.cpp, but not on ARM
        return DR_EMIT_DEFAULT;
    }
#endif
    if (app_should_ignore_tag(tag))
        return DR_EMIT_DEFAULT;

    app_pc src = instr_get_app_pc(instr);
#ifndef ARM
    // we resist this check on ARM because it's not easy to identify return instructions
    if (instr_is_return(instr)) {
        // checking returns could help construct a more complete CFG in the case that
        // we see obfuscated control flow, i.e. returning to a different place than to
        // the original caller, but it's *slow*.
        if (instrument_ret.get_value()) {
            dr_insert_mbr_instrumentation(drcontext, bb, instr,
                    (void *)at_cti, SPILL_SLOT_2);
        }
        return DR_EMIT_DEFAULT;
    }
#endif
    dr_insert_mbr_instrumentation(drcontext, bb, instr,
            (void *)at_cti, SPILL_SLOT_2);
    return DR_EMIT_DEFAULT;
}
