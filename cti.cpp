#include "dr_api.h"
#include "cti.h"
#include "cfg_impl.h"
#include "app.h"
#include "droption.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdint>

static droption_t<bool> instrument_ret
(DROPTION_SCOPE_CLIENT, "instrument_ret", false,
 "Count return instructions as control flow instructions", "");

static void
at_cti(uintptr_t src, uintptr_t targ)
{
    safe_insert(src, targ);
    if (txt.get_value()) {
//        if (output.get_value() == "") {
            std::cout << "INDIRECT jmp/call from " << std::hex << src << " to " << targ << std::endl;
/*        } else {
            std::ofstream ofs(output.get_value());
            ofs << "INDIRECT from " << std::hex << src << " to " << targ << std::endl;
        }
*/    }
}

dr_emit_flags_t
cti_event_app_instruction(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
                          bool for_trace, bool translating, void *user_data)
{
    if (!instr_is_cti(instr))
        return DR_EMIT_DEFAULT;
    if (instr_is_cbr(instr)) {
        // we already handle cbr's, more efficiently
        return DR_EMIT_DEFAULT;
    }
    if (app_should_ignore_tag(tag))
        return DR_EMIT_DEFAULT;

    app_pc src = instr_get_app_pc(instr);
    if (instr_is_return(instr)) {
        // checking returns could help construct a more complete CFG in the case that
        // we see obfuscated control flow, i.e. returning to a different place than to
        // the original caller.
        if (instrument_ret.get_value()) {
            dr_insert_clean_call(drcontext, bb, instr, (void *)at_cti, false, 2,
                                 OPND_CREATE_INTPTR(src), OPND_CREATE_MEMPTR(DR_REG_XSP, 0));
        }
        return DR_EMIT_DEFAULT;
    }

    opnd_t target_opnd = instr_get_target(instr);
    if (opnd_is_reg(target_opnd) || opnd_is_memory_reference(target_opnd)) {
        dr_insert_clean_call(drcontext, bb, instr, (void *)at_cti, false, 2,
                             OPND_CREATE_INTPTR(src), target_opnd);
    }
    return DR_EMIT_DEFAULT;
}
