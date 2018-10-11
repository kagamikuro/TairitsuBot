#pragma once

struct Result
{
public:
    bool matched;
    bool succeeded;
    explicit Result(const bool value_matched = false, const bool value_succeeded = false) :
        matched(value_matched), succeeded(value_succeeded) {}
};
