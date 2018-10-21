#pragma once

#include "../../processing/message_received.h"
#include "../../utility/dictionary.h"
#include "repeat_chain.h"

class PassiveRepeat final : public MessageReceived
{
private:
    Dictionary<RepeatChain> repeat_chains;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    PassiveRepeat() = default;
    ~PassiveRepeat() override = default;
};
