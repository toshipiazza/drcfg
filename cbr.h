#ifndef CBR_H_
#define CBR_H_

dr_emit_flags_t
cbr_event_app_instruction(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
                          bool for_trace, bool translating, void *user_data);

#endif
