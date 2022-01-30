#pragma once

#include <string>
#include <map>

#include <boost/format.hpp>

#ifndef POCO_STATIC
#define POCO_STATIC
#endif

#include <Poco/URI.h>

#include "log.h"
#include "configinfo.h"



struct QueryMessage
{
    std::string host;
    std::string port;
    std::string target;
    std::string caseType;
    std::string caseName;
};

class GetQueryMessage
{
public:
    static auto get(const ConfigInfo& confInfo) 
    {
        std::list<QueryMessage> lstMsgs;
        for (auto&& item : confInfo.caseItems)
        {
            Poco::URI uri;
            uri.setScheme("http");
            uri.setHost("qcrt.hexagonmetrology.com");
            uri.setPath("TestResult/SearchTestResult.aspx");

            for (auto&& c : item.second) 
            {
                Poco::URI::QueryParameters queryParam
                {
                      {"IsQueryString", "True"  }
                    , {"TestType",      str(boost::format("'%1%'")%item.first) }
                    , {"CodeLine",      confInfo.codeLine }
                    , {"ChangeList",    confInfo.changeList }
                    , {"TestMethod",    c}
                    , {"OS",            "Windows"}
                    , {"APP",           confInfo.APP }
                };

                uri.setQueryParameters(queryParam);
                uri.normalize();

                QueryMessage queryMsg{};
                queryMsg.host = uri.getHost();
                queryMsg.port = std::to_string(uri.getPort());
                queryMsg.target = "/" + uri.getPathAndQuery();
                queryMsg.caseType = item.first;
                queryMsg.caseName = c;

                lstMsgs.push_back(queryMsg);
            }
        }

        return lstMsgs;
    }

};
