#include "repeat_chain.h"

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
    if (repeated) return repeats.manipulate(id, [](int& value) { return ++value; });
    repeats.set(id, 1);
    return 1;
}
