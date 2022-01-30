#ifndef QUERYTREEWIDGET_H
#define QUERYTREEWIDGET_H

#include <string>
#include <map>
#include <memory>

#include <QTreeWidget>

#include "resulttype.h"

class Collector;

class QueryTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit QueryTreeWidget(QWidget *parent = nullptr);

    void searchText(QString text);

signals:

public slots:
    void displayResults(const ResultType& caseResult);

private:

};

#endif // QUERYTREEWIDGET_H
