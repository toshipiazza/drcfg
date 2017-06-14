#include "cfg_impl.h"

#include <unordered_map>
#include <unordered_set>
#include <mutex>

using nlohmann::json;

static std::unordered_map<uintptr_t, std::unordered_set<uintptr_t>> cbr;
static std::mutex mtx;

void safe_insert(uintptr_t src, uintptr_t trg)
{
    std::lock_guard<std::mutex> g(mtx);
    cbr[src].insert(trg);
}

json construct_json()
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
