#ifndef REFINECASES_H
#define REFINECASES_H

#include <string>
#include <set>
#include <map>
#include <list>
#include <regex>
#include <memory>

#include "configinfo.h"



struct RefineCases
{
    static auto refine(const std::set<std::string>& cases
                       , const std::string& codeLine
                       , const std::string& app
                       , const std::string& changeList = "")
    {
        try
        {
            ConfigInfo info;

            if(cases.empty())
            {
                return info;
            }

            info.codeLine = codeLine;
            info.APP = app;
            info.changeList=changeList;

            std::map<std::string, std::string> regStrToType
            {
                 {R"(^\w+(\.\w+)+$)", "BBT"}
                ,{R"(^integration(\\\w+)+\.py <\w+\.\w+>$)", "LRT"}
                ,{R"(^\w+(\\\w+)+_st\.py <\w+\.\w+\.\w+>$)", "STest"}
                ,{R"(^\w+(\\\w+)+_wt\.py <\w+\.\w+\.\w+>$)", "WTest"}
            };

            for(auto&& c : cases)
            {
                for(auto& item : regStrToType)
                {
                    std::smatch marchRes;
                    if(std::regex_match(c, marchRes, std::regex(item.first)))
                    {
                        info.caseItems[item.second].push_back(c);
                    }
                }
            }

            return info;

        }
        catch (std::exception&)
        {
            throw;
        }
    }
};




#endif // REFINECASES_H
