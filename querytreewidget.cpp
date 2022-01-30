#include "querytreewidget.h"
#include <QColor>
#include <QTableWidget>

#include "emitter.h"
#include "collector.h"
#include "log.h"


QueryTreeWidget::QueryTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    qRegisterMetaType<ResultType>();
    connect(&Emitter::instance(), &Emitter::addResults, this, &QueryTreeWidget::displayResults);
}

void QueryTreeWidget::displayResults(const ResultType& caseResult)
{
    BOOST_LOG(queryLog::get()) << "Start to update Results UI" << std::endl;

    clear();

    for (auto& caseInfo : caseResult)
    {
        QTreeWidgetItem* caseTypeItem = new QTreeWidgetItem(this);
        caseTypeItem->setText(0, QString::fromStdString(caseInfo.first));
        caseTypeItem->setToolTip(0, QString::fromStdString(caseInfo.first));

        for (auto& caseName : caseInfo.second)
        {
            QTreeWidgetItem* twItem=new QTreeWidgetItem(caseTypeItem);
            twItem->setText(0, QString::fromStdString(caseName.first));
            twItem->setToolTip(0, QString::fromStdString(caseName.first));

            QTableWidget* subTableWidget = new QTableWidget(caseName.second.size(), 2, this);
            QStringList headerText;
            headerText << "ChangeList" << "Result";
            subTableWidget->setHorizontalHeaderLabels(headerText);

            int i = 0;
            for (auto& caseRes : caseName.second)
            {
                auto itemNum = new QTableWidgetItem(QString("%1").arg(caseRes.first));
                itemNum->setToolTip(QString("%1").arg(caseRes.first));
                subTableWidget->setItem(i,0,itemNum);

                auto itemStatus = new QTableWidgetItem(QString::fromStdString(caseRes.second));
                itemStatus->setToolTip(QString::fromStdString(caseRes.second));
                subTableWidget->setItem(i++,1,itemStatus);
            }

            QTreeWidgetItem* twStatus=new QTreeWidgetItem(twItem);
            setItemWidget(twStatus, 0, subTableWidget);
        }
    }

    setItemsExpandable(true);
    expandAll();
}


void QueryTreeWidget::searchText(QString text)
{
    for(QTreeWidgetItemIterator it(this); *it; ++it)
    {
        auto t = (*it)->text(0);
        if(!t.isEmpty())
        {
            if(t == text)
            {
                (*it)->setBackground(0, QColor(Qt::GlobalColor::cyan));
                this->setCurrentItem(*it, 0);
                this->scrollToItem(*it);
            }
        }
    }
}
