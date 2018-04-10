#include "dr_api.h"
#include "drmgr.h"
#include "droption.h"
#include "cfg_impl.h"
#include "cti.h"
#include "cbr.h"
#include "app.h"

#include <iostream>
#include <fstream>
#include <iomanip>

static droption_t<bool> no_cbr
(DROPTION_SCOPE_CLIENT, "no_cbr", false,
 "Don't count conditional branch instructions", "");

static droption_t<bool> no_cti
(DROPTION_SCOPE_CLIENT, "no_cti", false,
 "Don't count control transfer instructions", "");

droption_t<std::string> output
(DROPTION_SCOPE_CLIENT, "output", "",
 "Output results to file", "");

droption_t<bool> txt
(DROPTION_SCOPE_CLIENT, "txt", false,
 "Print text output while running instead of json at the end of the run - you will have to take care of the doubles youself", "");

static void *mutex;
static int tcls_idx;
static FILE *out = stdout;

typedef struct {
  void *addr;
} per_thread_t;


void
dr_exit(void)
{
    if (!txt.get_value()) {
        if (output.get_value() == "")
            std::cout << std::hex << std::setw(2) << construct_json() << std::endl;
        else {
            std::ofstream ofs(output.get_value());
            ofs << std::hex << std::setw(2) << construct_json() << std::endl;
        }
    }
    drmgr_exit();
}

void 
event_thread_context_init(void *drcontext, bool new_depth)
{
    per_thread_t *data;

    if (new_depth) {
        data = (per_thread_t *) dr_thread_alloc(drcontext, sizeof(per_thread_t));
        drmgr_set_cls_field(drcontext, tcls_idx, data);
    } else {
        data = (per_thread_t *) drmgr_get_cls_field(drcontext, tcls_idx);
    }
    memset(data, 0, sizeof(*data));
}

void
event_thread_context_exit(void *drcontext, bool thread_exit)
{
    if (thread_exit) {
        per_thread_t *data = (per_thread_t *) drmgr_get_cls_field(drcontext, tcls_idx);
        dr_thread_free(drcontext, data, sizeof(per_thread_t));
        drmgr_set_cls_field(drcontext, tcls_idx, NULL);
    }
}

dr_emit_flags_t
event_app_instruction(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
                      bool for_trace, bool translating, void *user_data)
{
    per_thread_t *data = (per_thread_t *) drmgr_get_cls_field(drcontext, tcls_idx);
    if (data->addr != NULL) {
        dr_mutex_lock(mutex);
        if (txt.get_value()) {
            fprintf(out, "INDIRECT jmp/call from %p to %p\n", data->addr, tag);
        } else {
            safe_insert((uintptr_t)data->addr, (uintptr_t)tag);
        }
        dr_mutex_unlock(mutex);
        data->addr = NULL;
    }
    
    if (!drmgr_is_last_instr(drcontext, instr))
        return DR_EMIT_DEFAULT;
    if (app_should_ignore_tag(tag))
        return DR_EMIT_DEFAULT;
    
    if (!no_cbr.get_value() && instr_is_cbr(instr))
      data->addr = instr_get_app_pc(instr);
    else if (!no_cti.get_value() && instr_is_cbr(instr))
      data->addr = instr_get_app_pc(instr);
    
    if (app_should_ignore_tag(tag))
        return DR_EMIT_DEFAULT;
}

DR_EXPORT
void
dr_client_main(client_id_t id, int argc, const char *argv[])
{
    dr_set_client_name("Dynamic CFG-generator", "toshi.piazza@gmail.com");
    if (!droption_parser_t::parse_argv(DROPTION_SCOPE_CLIENT, argc, argv, NULL, NULL)) {
        std::cout << droption_parser_t::usage_short(DROPTION_SCOPE_CLIENT) << std::endl;
        exit(1);
    }

    app_init();
    drmgr_init();

    if (output.get_value() != "") {
        if ((out = fopen(output.get_value().c_str(), "w")) == NULL)
            DR_ASSERT_MSG(false, "can not write -outputfile");
    }
    
    mutex = dr_mutex_create();
    tcls_idx = drmgr_register_cls_field(event_thread_context_init, event_thread_context_exit);
    DR_ASSERT_MSG(tcls_idx != -1, "memory problems");
    drmgr_register_bb_instrumentation_event(NULL, event_app_instruction, NULL);
/*    
    if (!no_cbr.get_value())
        drmgr_register_bb_instrumentation_event(NULL, cbr_event_app_instruction, NULL);
    if (!no_cti.get_value())
        drmgr_register_bb_instrumentation_event(NULL, cti_event_app_instruction, NULL);
*/
    dr_register_exit_event(dr_exit);
}
