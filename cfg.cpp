#include "dr_api.h"
#include "drmgr.h"
#include "cfg_impl.h"
#include "cti.h"
#include "cbr.h"

#include <iostream>
#include <iomanip>

void dr_exit(void)
{
    std::cout << std::setw(2) << construct_json() << std::endl;
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
