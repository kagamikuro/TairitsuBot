#pragma once

#include "../processing/message_received.h"

class TranslateHikari final : public MessageReceived
{
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
public:
    TranslateHikari() = default;
    ~TranslateHikari() override = default;
};
