#include "repeat_chain.h"

RepeatChain::RepeatChain() :repeats(499) {}

int RepeatChain::new_message(const int64_t id, const std::string& message)
{
    if (message != last_message)
    {
        last_message = message;
        length = 0;
        repeats.clear();
    }
    const bool repeated = repeats.contains(id);
    length++;
    if (repeated) return ++repeats.get_value(id);
    repeats.insert(id, 1);
    return 1;
}
