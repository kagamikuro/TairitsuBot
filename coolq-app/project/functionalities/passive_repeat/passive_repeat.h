#pragma once

#include "../../processing/message_received.h"
#include "../../utility/hash_dictionary.h"
#include "repeat_chain.h"

class PassiveRepeat final : public MessageReceived
{
private:
    HashDictionary<RepeatChain> repeat_chains;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    PassiveRepeat() :repeat_chains(499) {}
    ~PassiveRepeat() override = default;
};
