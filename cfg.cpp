#include "dr_api.h"
#include "drmgr.h"
#include "droption.h"
#include "cfg_impl.h"
#include "cti.h"
#include "cbr.h"
#include "app.h"

#include <iostream>
#include <iomanip>

void
dr_exit(void)
{
    std::cout << std::setw(2) << construct_json() << std::endl;
    drmgr_exit();
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
    drmgr_register_bb_instrumentation_event(NULL, cbr_event_app_instruction, NULL);
    drmgr_register_bb_instrumentation_event(NULL, cti_event_app_instruction, NULL);
    dr_register_exit_event(dr_exit);
}
