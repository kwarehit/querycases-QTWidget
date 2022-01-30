#pragma once

#include <functional>
#include <map>
#include <memory>
#include <fstream>
#include <filesystem>

#include <boost/format.hpp>
#include <boost/asio/strand.hpp>

#include "casemessage.h"
#include "log.h"
#include "iocontextmanager.h"
#include "resulttype.h"

namespace net = boost::asio; 

class Collector 
{
public:
    explicit Collector(net::io_context& ioc, IOContextManager& iocMgr)
        : strand_(ioc.get_executor())
        , iocMgr_(iocMgr)
    {
    }

    void setCallBack(std::function<void(const ResultType& caseRes)> f)
    {
        func_ = f;
    }

    void resetData(std::size_t totalNum)
    {
        totalNum_ = totalNum;
        inProgressingNum_ = 0;
        caseStatus_.clear();
    }

    void writeRecordToFile()
    {
        auto resultsPath = std::filesystem::current_path() / "qcrtReport.txt";
        auto closeFile = [](std::ofstream* f) { f->close(); };
        std::unique_ptr<std::ofstream, decltype(closeFile)> outf(new std::ofstream(resultsPath, std::ios::out | std::ios::trunc), closeFile);

        BOOST_LOG(queryLog::get()) << "Start to save qcrtReport.txt" << std::endl;

        for (auto& caseInfo : caseStatus_)
        {
            for (auto& caseName : caseInfo.second)
            {
                *outf << str(boost::format("%1%:%2%\n") % caseInfo.first %caseName.first);
                std::size_t index = 1;
                for (auto& caseRes : caseName.second)
                {
                    *outf << str(boost::format("(%1%:%2%:%3%)") % index++ %caseRes.first %caseRes.second);
                }
                *outf << "\n\n";
            }
        }

        BOOST_LOG(queryLog::get()) << "qcrtReport.txt have been saved in " << std::filesystem::current_path() << std::endl;
    }

    void aggregateFailed()
    {
        net::post(strand_, [this]()
        {
            inProgressingNum_++;

            BOOST_LOG(queryLog::get()) << inProgressingNum_ << "/" << totalNum_ << std::endl;

            if (inProgressingNum_ == totalNum_)
            {
                iocMgr_.stop();
            }
        });
    }


    void aggregate(const CaseMessage& msg) 
    {
        try
        {
            net::post(strand_, [m = std::move(msg), this]()
            {
                inProgressingNum_++;
                if (inProgressingNum_ <= totalNum_)
                {
                    caseStatus_[m.caseType][m.caseName] = m.results;
                }

                BOOST_LOG(queryLog::get()) << inProgressingNum_ << "/" << totalNum_ << std::endl;

                BOOST_LOG(queryLog::get()) << m.caseType << ":" << m.caseName << " have been done!" << std::endl;

                if (inProgressingNum_ == totalNum_)
                {
                    writeRecordToFile();

                    if(func_)
                    {
                        func_(caseStatus_);
                    }

                    iocMgr_.stop();
                }
            });
        }
        catch (std::exception& e)
        {
            BOOST_LOG(queryLog::get()) << "Collect data failed: " << e.what() << std::endl;
        }
    }

private:
    net::strand<net::io_context::executor_type> strand_;
    IOContextManager& iocMgr_;
    std::size_t totalNum_ = 0;
    std::size_t inProgressingNum_ = 0;
    ResultType caseStatus_;
    std::function<void(const ResultType& caseRes)> func_;
};
