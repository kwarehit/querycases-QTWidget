#pragma once

#include <memory>
#include <vector>
#include <thread>

#include <boost/noncopyable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>



class IOContextManager : public boost::noncopyable
{
public:
    explicit IOContextManager(std::size_t size) 
    {
        if (size <= 0) 
        {
            size = 1;
        }

        for (auto i = 0; i < size; ++i) 
        {
            auto iocInfo = std::make_shared<boost::asio::io_context>();
            auto workGuard = std::make_shared<WorkGuard>(iocInfo->get_executor());

            iocInfos_.push_back(iocInfo);
            workGuards_.push_back(workGuard);
        }

        ioc_ = std::make_shared<boost::asio::io_context>();
        work_ = std::make_shared<WorkGuard>(ioc_->get_executor());
    }

    void run() 
    {
        if(workGuards_.empty())
        {
            workGuards_.resize(iocInfos_.size());
            for (auto i = 0; i < iocInfos_.size(); ++i)
            {
                if(iocInfos_[i]->stopped())
                {
                    workGuards_[i] = std::make_shared<WorkGuard>(iocInfos_[i]->get_executor());
                    iocInfos_[i]->restart();
                }
            }
        }

        if(!work_ && ioc_->stopped())
        {
            work_ = std::make_shared<WorkGuard>(ioc_->get_executor());
            ioc_->restart();
        }

        std::vector<std::shared_ptr<std::thread>> threads;
        for (auto i = 0; i < iocInfos_.size(); ++i) 
        {
            threads.emplace_back(std::make_shared<std::thread>(
                [](auto ioc) { ioc->run(); }, iocInfos_[i])
            );
        }

        std::shared_ptr<std::thread> t = std::make_shared<std::thread>([](auto ioc) {ioc->run(); }, ioc_);

        for (auto i = 0; i < iocInfos_.size(); ++i) 
        {
            threads[i]->join();
        }

        t->join();
    }

    void stop() 
    {
        workGuards_.clear();

        for (auto i = 0; i < iocInfos_.size(); ++i) 
        {
            iocInfos_[i]->stop();
        }

        work_.reset();
        ioc_->stop();
    }

    boost::asio::io_context& getIOContext() 
    {
        boost::asio::io_context& ioc = *iocInfos_[nextIOContext++];
        if (nextIOContext == iocInfos_.size()) 
        {
            nextIOContext = 0;
        }

        return ioc;
    }

    boost::asio::io_context& getAUXIOContext() 
    {
        return *ioc_;
    }

    using IOContextPtr = std::shared_ptr<boost::asio::io_context>;
    using WorkGuard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using WorkGuardPtr = std::shared_ptr<WorkGuard>;

private:
    std::vector<IOContextPtr> iocInfos_;
    std::vector<WorkGuardPtr> workGuards_;
    std::shared_ptr<boost::asio::io_context> ioc_;
    WorkGuardPtr work_;
    std::size_t nextIOContext = 0;
};
