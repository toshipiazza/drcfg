#ifndef APP_H_
#define APP_H_
#include "dr_api.h"
#include "drmgr.h"
#include "droption.h"

extern droption_t<bool> txt;
extern droption_t<std::string> output;

void app_init(void);
bool app_should_ignore_tag(void *tag);

#endif
