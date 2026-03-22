#pragma once

#include <string>

struct Tree23Step
{
    std::string action;
    int line;
    int value;

    Tree23Step();
    Tree23Step(const std::string& actionName, int codeLine, int currentValue);
};
