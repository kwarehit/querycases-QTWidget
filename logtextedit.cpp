#include "logtextedit.h"
#include "log.h"


LogTextEdit::LogTextEdit(QWidget *parent)
    : QTextEdit(parent)
{
    connect(this, &LogTextEdit::addLog, this, &LogTextEdit::logText);

    setSelf();
    InitLog(&LogTextEdit::getLog);
}

void LogTextEdit::setSelf()
{
    pThis_ = this;
}

void LogTextEdit::getLog(const std::string& s)
{
    if(pThis_)
    {
        QString str(s.c_str());
        Q_EMIT pThis_->addLog(str);
    }
}

void LogTextEdit::logText(const QString& s)
{
    moveCursor(QTextCursor::End);
    insertPlainText(s);
    moveCursor(QTextCursor::End);
}

