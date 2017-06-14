#ifndef CTI_H_
#define CTI_H_

dr_emit_flags_t
cti_event_app_instruction(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
                          bool for_trace, bool translating, void *user_data);

#endif
