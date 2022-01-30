#include "emitter.h"

Emitter::Emitter(QObject *parent) : QObject(parent)
{

}

Emitter& Emitter::instance()
{
    static Emitter emitter;
    return emitter;
}

void Emitter::getResults(const ResultType& caseRes)
{
    instance().send(caseRes);
}


void Emitter::send(const ResultType& caseResult)
{
    Q_EMIT addResults(caseResult);
}
