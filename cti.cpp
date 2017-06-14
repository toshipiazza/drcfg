#include "dr_api.h"
#include "cti.h"
#include "cfg_impl.h"

#include <cstdint>

static void
at_cti(uintptr_t src, uintptr_t targ)
{
    safe_insert(src, targ);
}

dr_emit_flags_t
cti_event_app_instruction(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
                          bool for_trace, bool translating, void *user_data)
{
    if (!instr_is_cti(instr))
        return DR_EMIT_DEFAULT;
    if (instr_is_return(instr)) {
        // TODO: checking returns could help construct a more complete CFG in the case
        // that we see obfuscated control flow, i.e. returning to a different place than
        // to the original caller.
        return DR_EMIT_DEFAULT;
    }

    opnd_t target_opnd = instr_get_target(instr);
    if (opnd_is_reg(target_opnd) ||
            opnd_is_memory_reference(target_opnd)) {
        app_pc src = instr_get_app_pc(instr);
        dr_insert_clean_call(drcontext, bb, instr,
                             (void *)at_cti,
                             false,
                             2,
                             OPND_CREATE_INTPTR(src),
                             target_opnd); // duplicate the target operand
    }
    return DR_EMIT_DEFAULT;
}
