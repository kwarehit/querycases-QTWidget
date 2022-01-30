#pragma once

#include <iostream>
#include <string>
#include <map>
#include <system_error>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include "log.h"
#include "collector.h"
#include "parsehtml.h"
#include "querymessage.h"

namespace beast = boost::beast;         
namespace http = beast::http;           
namespace net = boost::asio;            
using tcp = boost::asio::ip::tcp;       


class Worker : public std::enable_shared_from_this<Worker>
{
    tcp::resolver resolver_;
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_; 
    QueryMessage queryMsg_;
    http::request<http::empty_body> req_;
    http::response<http::string_body> res_;
    Collector& collector_;

public:
    explicit Worker(net::io_context& ioc, const QueryMessage& queryMsg, Collector& collector)
        : resolver_(net::make_strand(ioc))
        , stream_(net::make_strand(ioc))
        , collector_(collector)
        , queryMsg_(queryMsg)
    {
    }

    void start()
    {
        resolver_.async_resolve(
            queryMsg_.host,
            queryMsg_.port,
            [this, self = shared_from_this()](auto ec, auto results){
                onResolve(ec, results);
        });
    }

    void onResolve(beast::error_code ec, tcp::resolver::results_type results)
    {
        try
        {
            if (ec)
            {
                throw std::system_error(ec, ec.message());
            }

            stream_.expires_after(std::chrono::seconds(30));

            stream_.async_connect(
                        results,
                        [this, self = shared_from_this()](auto ec, auto end){
                onConnect(ec, end);
            });
        }
        catch(std::exception& ec)
        {
            BOOST_LOG(queryLog::get()) << ec.what() << std::endl;
            BOOST_LOG(queryLog::get()) << "Failure case->" << queryMsg_.caseType << ":" << queryMsg_.caseName << std::endl;
            stream_.close();
            collector_.aggregateFailed();
        }
    }

    void onConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
    {
        try
        {
            if(ec)
            {
                throw std::system_error(ec, ec.message());
            }

            stream_.expires_after(std::chrono::seconds(30));

            req_.version(11);
            req_.method(http::verb::get);
            req_.target(queryMsg_.target);
            req_.set(http::field::host, queryMsg_.host);

            http::async_write(
                stream_,
                req_,
                [this, self = shared_from_this()](auto ec, auto bytes_transferred){
                    onWrite(ec, bytes_transferred);
            });
        }
        catch(std::exception& ec)
        {
            BOOST_LOG(queryLog::get()) << ec.what() << std::endl;
            BOOST_LOG(queryLog::get()) << "Failure case->" << queryMsg_.caseType << ":" << queryMsg_.caseName << std::endl;
            stream_.close();
            collector_.aggregateFailed();
        }
    }

    void onWrite(beast::error_code ec, std::size_t bytes_transferred)
    {
        try
        {
            boost::ignore_unused(bytes_transferred);

            if(ec)
            {
                throw std::system_error(ec, ec.message());
            }

            BOOST_LOG(queryLog::get()) << "Querying case->" << queryMsg_.caseType << ":" << queryMsg_.caseName << std::endl;

            http::async_read(
                        stream_,
                        buffer_,
                        res_,
                        [this, self = shared_from_this()](auto ec, auto bytes_transferred){
                onRead(ec, bytes_transferred);
            });
        }
        catch(std::exception& ec)
        {
            BOOST_LOG(queryLog::get()) << ec.what() << std::endl;
            BOOST_LOG(queryLog::get()) << "Failure case->" << queryMsg_.caseType << ":" << queryMsg_.caseName << std::endl;
            stream_.close();
            collector_.aggregateFailed();
        }
    }

    void onRead(beast::error_code ec, std::size_t bytes_transferred)
    {
        try
        {
            boost::ignore_unused(bytes_transferred);

            stream_.expires_after(std::chrono::seconds(30));

            if(ec)
            {
                throw std::system_error(ec, ec.message());
            }

            if(res_.result() != http::status::ok)
            {
                throw std::system_error(std::error_code{-1, std::generic_category()}, res_.reason().data());
            }

            BOOST_LOG(queryLog::get()) << "Responsed case->" << queryMsg_.caseType << ":" << queryMsg_.caseName << std::endl;

            std::string str = res_.body();

            auto&& msg = ParseHTML::parse(str, queryMsg_);
            collector_.aggregate(msg);

            stream_.socket().shutdown(tcp::socket::shutdown_both, ec);

            if(ec && ec != beast::errc::not_connected)
                throw std::system_error(ec);
        }
        catch(std::exception& ec)
        {
            BOOST_LOG(queryLog::get()) << ec.what() << std::endl;
            BOOST_LOG(queryLog::get()) << "Failure case->" << queryMsg_.caseType << ":" << queryMsg_.caseName << std::endl;
            stream_.close();
            collector_.aggregateFailed();
        }
    }
};
