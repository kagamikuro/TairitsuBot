#pragma once

#include "../processing/message_received.h"
#include "../utility/rng.h"

class SubjectiveRepeat final : public MessageReceived
{
private:
    RNG random_number_generator;
protected:
    Result process(const cq::Target& current_target, const std::string& message) override;
    Result process_creator(const std::string& message) override;
public:
    SubjectiveRepeat();
    ~SubjectiveRepeat() override = default;
};
