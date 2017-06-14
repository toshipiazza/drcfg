#include "dr_api.h"
#include "cbr.h"
#include "cfg_impl.h"
#include "app.h"

#include <cstdint>

static void
taken_or_not(uintptr_t src, uintptr_t targ)
{
    safe_insert(src, targ);
}

dr_emit_flags_t
cbr_event_app_instruction(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
                          bool for_trace, bool translating, void *user_data)
{
    if (!instr_is_cbr(instr))
        return DR_EMIT_DEFAULT;
    if (app_should_ignore_tag(tag))
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
