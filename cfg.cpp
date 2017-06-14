#include "dr_api.h"
#include "drmgr.h"

#include "json.hpp"
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using json = nlohmann::json;

static std::unordered_map<uintptr_t, std::unordered_set<uintptr_t>> cbr;

static void at_cti(uintptr_t src, uintptr_t targ)
{ cbr[src].insert(targ); }

static dr_emit_flags_t
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

static void
taken_or_not(uintptr_t src, uintptr_t targ)
{ cbr[src].insert(targ); }

static dr_emit_flags_t
cbr_event_app_instruction(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
                          bool for_trace, bool translating, void *user_data)
{
    if (!instr_is_cbr(instr))
        return DR_EMIT_DEFAULT;

    app_pc src = instr_get_app_pc(instr);
    app_pc fall = (app_pc)decode_next_pc(drcontext, (byte *)src);
    app_pc targ = instr_get_branch_target_pc(instr);

    instr_t *label = INSTR_CREATE_label(drcontext);
    instr_set_meta_no_translation(instr);
    if (instr_is_cti_short(instr))
        instr = instr_convert_short_meta_jmp_to_long(drcontext, bb, instr);
    instr_set_target(instr, opnd_create_instr(label));

    dr_insert_clean_call(drcontext, bb, NULL, (void *)taken_or_not,
                         false, 2, OPND_CREATE_INTPTR(src), OPND_CREATE_INTPTR(fall));
    instrlist_preinsert(bb, NULL,
                        INSTR_XL8(INSTR_CREATE_jmp
                                  (drcontext, opnd_create_pc(fall)), fall));

    instrlist_meta_preinsert(bb, NULL, label);

    dr_insert_clean_call(drcontext, bb, NULL, (void *)taken_or_not,
                         false, 2, OPND_CREATE_INTPTR(src), OPND_CREATE_INTPTR(targ));
    instrlist_preinsert(bb, NULL,
                        INSTR_XL8(INSTR_CREATE_jmp
                                  (drcontext, opnd_create_pc(targ)), targ));

    return DR_EMIT_DEFAULT;
}

void dr_exit(void)
{
    json j;
    std::transform(std::begin(cbr), std::end(cbr),
                   std::back_inserter(j["branches"]),
        [] (auto i) -> json {
            return {
                { "address", i.first  },
                { "targets", i.second }
            };
        });
    std::cout << std::setw(2) << j << std::endl;
    drmgr_exit();
}

DR_EXPORT
void dr_init(client_id_t id)
{
    dr_set_client_name("Dynamic CFG-generator",
                       "toshi.piazza@gmail.com");
    if (!drmgr_init())
        DR_ASSERT_MSG(false, "drmgr_init failed!");

    if (!drmgr_register_bb_instrumentation_event(NULL, cbr_event_app_instruction, NULL))
        DR_ASSERT_MSG(false, "fail to register cbr_event_app_instruction!");
    if (!drmgr_register_bb_instrumentation_event(NULL, cti_event_app_instruction, NULL))
        DR_ASSERT_MSG(false, "fail to register cti_event_app_instruction!");
    dr_register_exit_event(dr_exit);
}
