#include "dr_api.h"
#include "droption.h"
#include "app.h"

static droption_t<bool> only_from_app
(DROPTION_SCOPE_CLIENT, "only_from_app", false,
 "Only count app, not lib, instructions",
 "Count only instructions in the application itself, ignoring instructions in "
 "shared libraries.");
static app_pc exe_start;

void
app_init(void)
{
    /* Get main module address */
    if (only_from_app.get_value()) {
        module_data_t *exe = dr_get_main_module();
        if (exe != NULL)
            exe_start = exe->start;
        dr_free_module_data(exe);
    }
}

bool
app_should_ignore_tag(void *tag)
{
    /* Only count in app BBs */
    if (only_from_app.get_value()) {
        module_data_t *mod = dr_lookup_module(dr_fragment_app_pc(tag));
        if (mod != NULL) {
            bool from_exe = (mod->start == exe_start);
            dr_free_module_data(mod);
            return !from_exe;
        }
    }
    return false;
}
