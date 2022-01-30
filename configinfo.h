#pragma once

#include <string>
#include <map>
#include <list>

struct ConfigInfo
{
    std::string codeLine;
    std::string changeList;
    std::string APP;
    std::map<std::string, std::list<std::string>> caseItems;
};
