#pragma once

#include <string>
#include <map>

struct CaseMessage 
{
    std::string caseType;
    std::string caseName;
    std::map<unsigned int, std::string, std::greater<unsigned int>> results;
};