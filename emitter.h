#ifndef EMITTER_H
#define EMITTER_H

#include <QObject>

#include "resulttype.h"

class Emitter : public QObject
{
    Q_OBJECT
public:
    explicit Emitter(QObject *parent = nullptr);

    static Emitter& instance();

    static void getResults(const ResultType& caseRes);

    void send(const ResultType& caseResult);

signals:
    void addResults(const ResultType& caseResult);

};

#endif // EMITTER_H
