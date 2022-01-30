#pragma once

#include <string>
#include <memory>

#include <libxml/tree.h>
#include <libxml/HTMLparser.h>

#ifndef LIBXMLXX_STATIC
#define LIBXMLXX_STATIC
#endif

#include <libxml++/libxml++.h>

#include "log.h"
#include "querymessage.h"
#include "casemessage.h"

class ParseHTML 
{
public:
    static auto parse(const std::string& str, const QueryMessage& queryMsg) 
    {
        try
        {
            BOOST_LOG(queryLog::get()) << "Start parse html" << std::endl;

            std::unique_ptr<xmlDoc, decltype(&xmlFreeDoc)> doc
            (
                htmlReadDoc((xmlChar*)str.c_str(), NULL, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING), &xmlFreeDoc
            );

            xmlNode* r = xmlDocGetRootElement(doc.get());

            std::unique_ptr<xmlpp::Element> root = std::make_unique<xmlpp::Element>(r);
            if(!root)
            {
                throw std::system_error(std::error_code{-1, std::generic_category()}, "parse html failed!");
            }

            std::string xpath = R"(/html/body/form/div/div/div/div/table[@id="MainContent_GridViewTestResult"]/tr[@align="left"])";
            auto elements = root->find(xpath);

            if(elements.empty())
            {
                throw std::system_error(std::error_code{-1, std::generic_category()}, "parse html failed!");
            }

            CaseMessage msg;
            msg.caseType = queryMsg.caseType;
            msg.caseName = queryMsg.caseName;

            for (const auto& child : elements)
            {
                auto items = child->find(".//td");

                unsigned int num{};
                std::string status;
                if (auto e = dynamic_cast<const xmlpp::Element*>(items[3]->find(".//font").front()))
                {
                    if (auto text_node = e->get_first_child_text())
                    {
                        num = std::stoul(text_node->get_content());
                    }
                }

                if (auto e = dynamic_cast<const xmlpp::Element*>(items[5]->find(".//font").front()))
                {
                    if (auto text_node = e->get_first_child_text())
                    {
                        status = text_node->get_content();
                    }
                }

                if (num != 0 && !status.empty())
                {
                    msg.results.emplace(num, status);
                }
            }

            BOOST_LOG(queryLog::get()) << "Finish parse html" << std::endl;

            return msg;
        }
        catch(std::exception&)
        {
            throw;
        }
    }

};
