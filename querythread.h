#ifndef QUERYTHREAD_H
#define QUERYTHREAD_H

#include <set>
#include <string>
#include <memory>

#include <QThread>

#include "configinfo.h"
#include "prompttext.h"


class IOContextManager;
class Collector;


class QueryThread : public QThread
{
    Q_OBJECT
public:
    explicit QueryThread(QObject *parent = nullptr);

    void run();

    std::shared_ptr<Collector> getCollector();

    void refine(const std::set<std::string>& cases
                , const std::string& codeLine
                , const std::string& app
                , const std::string& changeList = "");

signals:
    void enableButton(bool b);

private:
    void query();

    ConfigInfo confInfo_;

    std::shared_ptr<IOContextManager> iocMgr_;
    std::shared_ptr<Collector> collector_;
};

#endif // QUERYTHREAD_H
