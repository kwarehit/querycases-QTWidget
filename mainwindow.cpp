#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <thread>
#include <algorithm>
#include <numeric>
#include <set>

#include <QTextDocument>
#include <QTextBlock>

#include "log.h"
#include "emitter.h"
#include "collector.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widget_2->setLayout(ui->verticalLayout_2);
    ui->widget->setLayout(ui->verticalLayout);
    ui->centralwidget->setLayout(ui->gridLayout);

    ui->treeWidget->setHeaderLabel("Results");

    pThread_ = new QueryThread(this);
    pThread_->getCollector()->setCallBack(&Emitter::getResults);

    connect(pThread_, &QueryThread::enableButton, ui->pushButton_2, &QPushButton::setEnabled);

    BOOST_LOG(queryLog::get()) << promptText;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_plainTextEdit_textChanged()
{
    try
    {
        auto doc = ui->plainTextEdit->document();
        auto n = doc->blockCount();

        std::string codeLine;
        std::string app;
        std::string changeList;
        std::set<std::string> cases;
        int validNum{};
        for(auto i = 0; i<n; ++i)
        {
            QTextBlock t = doc->findBlockByNumber(i);
            QString s = t.text().trimmed();
            if(!s.isEmpty())
            {
                ++validNum;
                if(validNum == 1)
                {
                    codeLine = s.toStdString();
                }
                else if(validNum == 2)
                {
                    bool ok{};
                    s.toInt(&ok, 10);
                    if(ok)
                    {
                        changeList = s.toStdString();
                    }
                    else
                    {
                        app = s.toStdString();
                    }
                }
                else if(validNum == 3 && app.empty())
                {
                    app = s.toStdString();
                }
                else
                {
                    cases.insert(s.toStdString());
                }
            }
        }

        pThread_->refine(cases, codeLine, app, changeList);

    }
    catch (std::exception& ec)
    {
        BOOST_LOG(queryLog::get()) << ec.what() << std::endl;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    pThread_->start();
}


void MainWindow::on_pushButton_3_clicked()
{
    try
    {
        auto t = ui->lineEdit->text().trimmed();
        ui->treeWidget->searchText(t);
    }
    catch (std::exception& ec)
    {
        BOOST_LOG(queryLog::get()) << ec.what() << std::endl;
    }
}

