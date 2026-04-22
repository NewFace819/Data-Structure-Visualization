#include "Tree23Step.h"

Tree23Step::Tree23Step()
{
    action = "";
    line = -1;
    value = 0;
}

Tree23Step::Tree23Step(const std::string& actionName, int codeLine, int currentValue)
{
    action = actionName;
    line = codeLine;
    value = currentValue;
}
