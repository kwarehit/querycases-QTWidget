#ifndef LOGTEXTEDIT_H
#define LOGTEXTEDIT_H

#include <QTextEdit>

class LogTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit LogTextEdit(QWidget *parent = nullptr);

    static void getLog(const std::string& s);

signals:
    void addLog(const QString& s);

public slots:
    void logText(const QString& s);

private:
    void setSelf();

    inline static LogTextEdit* pThis_ = nullptr;
};

#endif // LOGTEXTEDIT_H
