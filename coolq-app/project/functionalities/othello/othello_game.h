#pragma once

#include "../../processing/message_received.h"

class OthelloGame final : public MessageReceived
{
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
public:
    OthelloGame() = default;
    ~OthelloGame() override = default;    
};
