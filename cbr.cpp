/* **********************************************************
 * Copyright (c) 2014 Google, Inc.  All rights reserved.
 * Copyright (c) 2008 VMware, Inc.  All rights reserved.
 * **********************************************************/

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of VMware, Inc. nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL VMWARE, INC. OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include "dr_api.h"
#include "cbr.h"
#include "cfg_impl.h"
#include "app.h"

#include <cstdint>

static void
taken_or_not(uintptr_t src, uintptr_t targ)
{
    dr_mcontext_t mcontext = {sizeof(mcontext),DR_MC_ALL,};
    void *drcontext = dr_get_current_drcontext();

    safe_insert(src, targ);

    /* Remove the bb from the cache so it will be re-built the next
     * time it executes.
     */
    /* Since the flush will remove the fragment we're already in,
     * redirect execution to the fallthrough address.
     */
    dr_flush_region((app_pc)src, 1);
    dr_get_mcontext(drcontext, &mcontext);
    mcontext.pc = (app_pc)targ;
    dr_redirect_execution(&mcontext);
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

    bool insert_targ = !branch_present((uintptr_t)src, (uintptr_t)targ);
    bool insert_fall = !branch_present((uintptr_t)src, (uintptr_t)fall);

    if (insert_targ || insert_fall) {
        instr_t *label = INSTR_CREATE_label(drcontext);
        instr_set_meta_no_translation(instr);
        if (instr_is_cti_short(instr))
            instr = instr_convert_short_meta_jmp_to_long(drcontext, bb, instr);
        instr_set_target(instr, opnd_create_instr(label));

        if (insert_fall) {
            dr_insert_clean_call(drcontext, bb, NULL, (void *)taken_or_not, false, 2,
                                 OPND_CREATE_INTPTR(src), OPND_CREATE_INTPTR(fall));
        }
        instrlist_preinsert(bb, NULL, INSTR_XL8
                (XINST_CREATE_jump
                 (drcontext,
                  opnd_create_pc(fall)), fall));
        instrlist_meta_preinsert(bb, NULL, label);

        if (insert_targ) {
            dr_insert_clean_call(drcontext, bb, NULL, (void *)taken_or_not, false, 2,
                                 OPND_CREATE_INTPTR(src), OPND_CREATE_INTPTR(targ));
        }
        instrlist_preinsert(bb, NULL, INSTR_XL8
                (XINST_CREATE_jump
                 (drcontext,
                  opnd_create_pc(targ)), targ));
    }

    return DR_EMIT_STORE_TRANSLATIONS;
}
