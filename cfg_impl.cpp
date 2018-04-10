#include "cfg_impl.h"
#include "droption.h"

#include <unordered_map>
#include <unordered_set>
#include <mutex>

using nlohmann::json;

static droption_t<bool> racy
(DROPTION_SCOPE_CLIENT, "racy", false,
 "unused option", "");

static std::unordered_map<uintptr_t, std::unordered_set<uintptr_t>> cbr;
static std::mutex mtx;

void
safe_insert(uintptr_t src, uintptr_t trg)
{
/*
    if (!racy.get_value()) {
        std::lock_guard<std::mutex> g(mtx);
        cbr[src].insert(trg);
    } else
*/
        cbr[src].insert(trg);
}

static json
construct_json_impl()
{
    json j;
    std::transform(std::begin(cbr), std::end(cbr),
                   std::back_inserter(j["branches"]),
        [] (auto i) -> json {
            return {
                { "address", i.first  },
                { "targets", i.second }
            };
        });
    return j;
}

json
construct_json()
{
/*
    if (!racy.get_value()) {
        std::lock_guard<std::mutex> g(mtx);
        return construct_json_impl();
    } else
*/
        return construct_json_impl();
}

bool
branch_present(uintptr_t src, uintptr_t trg)
{
    if (!racy.get_value()) {
        std::lock_guard<std::mutex> g(mtx);
        return cbr[src].find(trg) != cbr[src].end();
    } else
        return cbr[src].find(trg) != cbr[src].end();
}
