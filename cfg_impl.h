#ifndef CFG_IMPL_H_
#define CFG_IMPL_H_

#include "json.hpp"
void safe_insert(uintptr_t src, uintptr_t trg);
nlohmann::json construct_json();
bool branch_present(uintptr_t src, uintptr_t trg);

#endif
