#include "dr_api.h"
#include "drmgr.h"
#include "droption.h"
#include "cfg_impl.h"
#include "mbr.h"
#ifndef ARM
# include "cbr.h"
#endif
#include "app.h"

#include <iostream>
#include <fstream>
#include <iomanip>

#ifndef ARM
static droption_t<bool> no_cbr
(DROPTION_SCOPE_CLIENT, "no_cbr", false,
 "Don't count conditional branch instructions", "");
#endif

static droption_t<bool> no_cti
(DROPTION_SCOPE_CLIENT, "no_cti", false,
 "Don't count control transfer instructions", "");

static droption_t<std::string> output
(DROPTION_SCOPE_CLIENT, "output", "",
 "Output results to file", "");

void
dr_exit(void)
{
    if (output.get_value() == "")
        std::cout << std::setw(2) << construct_json() << std::endl;
    else {
        std::ofstream ofs(output.get_value());
        ofs << std::setw(2) << construct_json() << std::endl;
    }
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
#ifndef ARM
    if (!no_cbr.get_value())
        drmgr_register_bb_instrumentation_event(NULL, cbr_event_app_instruction, NULL);
#endif
    if (!no_cti.get_value())
        drmgr_register_bb_instrumentation_event(NULL, cti_event_app_instruction, NULL);
    dr_register_exit_event(dr_exit);
}
