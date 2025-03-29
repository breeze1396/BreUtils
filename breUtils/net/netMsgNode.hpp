#pragma once

#include <string>
#include <vector>
#include <memory>

struct netMsgNode
{
    std::string SessionId;
    std::shared_ptr<std::vector<uint8_t>> Data;
};
