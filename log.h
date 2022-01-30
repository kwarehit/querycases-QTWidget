#pragma once

#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/support/date_time.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(queryLog, src::logger_mt)

template <typename F>
class console_ostream_backend : public sinks::text_ostream_backend
{
public:
    console_ostream_backend(F obj)
        :func_(obj)
    {
    }

    void consume(logging::record_view const & rec, sinks::text_ostream_backend::string_type const & message)
    {
        sinks::text_ostream_backend::consume(rec, message);

        if(func_)
        {
            func_(message);
        }
    }

private:
    F func_;
};


template <typename F>
void InitLog(F fObj)
{
    using backend_t = console_ostream_backend<F>;
    boost::shared_ptr<backend_t> backend = boost::make_shared<backend_t>(fObj);
    backend->add_stream(boost::shared_ptr< std::ostream >(&std::cout, boost::null_deleter()));
    backend->set_auto_newline_mode(boost::log::sinks::auto_newline_mode::disabled_auto_newline);

    using sink_t = sinks::synchronous_sink<backend_t>;
    boost::shared_ptr<sink_t> sink = boost::make_shared<sink_t>(backend);

    sink->set_formatter(expr::format("[%1%] %2%")
                        % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
                        % expr::smessage);

    attrs::local_clock TimeStamp;
    logging::core::get()->add_global_attribute("TimeStamp", TimeStamp);

    logging::core::get()->add_sink(sink);
}



