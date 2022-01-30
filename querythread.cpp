#include "querythread.h"

#include <iterator>
#include <boost/format.hpp>

#include "refinecases.h"
#include "log.h"
#include "collector.h"
#include "iocontextmanager.h"
#include "spendtime.h"
#include "worker.h"
#include "querymessage.h"


QueryThread::QueryThread(QObject *parent)
    : QThread(parent)
{
    auto n = std::thread::hardware_concurrency();
    iocMgr_ = std::make_shared<IOContextManager>(n-3);
    collector_ = std::make_shared<Collector>(iocMgr_->getAUXIOContext(), *iocMgr_);
}

std::shared_ptr<Collector> QueryThread::getCollector()
{
    return collector_;
}

void QueryThread::refine(const std::set<std::string>& cases
            , const std::string& codeLine
            , const std::string& app
            , const std::string& changeList)
{
    confInfo_ = RefineCases::refine(cases, codeLine, app, changeList);

    std::string queryHead = str(boost::format(querySummaryHead)%confInfo_.codeLine %confInfo_.changeList %confInfo_.APP);

    std::string queryBody;
    if(!confInfo_.caseItems.empty())
    {
        for(auto&& itemCase : confInfo_.caseItems)
        {
            queryBody += str(boost::format(querySummaryCaseHead)
                            %itemCase.second.size()
                            %itemCase.first);

            int n = 1;
            for(auto&& caseName : itemCase.second)
            {
                queryBody += str(boost::format(querySummaryCaseBody)%n++%caseName);
            }
        }
    }

    BOOST_LOG(queryLog::get()) << queryHead << queryBody << std::endl;
}

void QueryThread::query()
{
    try
    {
        BOOST_LOG(queryLog::get()) << "Start query\n";

        std::size_t totalCountItems = std::accumulate(confInfo_.caseItems.cbegin(), confInfo_.caseItems.cend()
            , (std::size_t)0, [](std::size_t i, auto& v) { return i + v.second.size(); });
        collector_->resetData(totalCountItems);
        auto&& listMsg = GetQueryMessage::get(confInfo_);

        Q_EMIT enableButton(false);

        {
            SpendTime t;
            for (auto&& m : listMsg)
            {
                std::make_shared<Worker>(iocMgr_->getIOContext(), m, *collector_)->start();
            }

            if(listMsg.empty())
            {
                BOOST_LOG(queryLog::get()) << "The list of cases are empty!" << std::endl;
            }
            else
            {
                iocMgr_->run();
                BOOST_LOG(queryLog::get()) << "All cases had been done!" << std::endl;
            }
        }

        Q_EMIT enableButton(true);

    }
    catch (std::exception& ec)
    {
        Q_EMIT enableButton(true);
        iocMgr_->stop();

        BOOST_LOG(queryLog::get()) << ec.what() << std::endl;
    }
}

void QueryThread::run()
{
    query();
}
