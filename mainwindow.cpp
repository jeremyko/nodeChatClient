#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "netmanager.h"
#include "addnewfrienddialog.h"
#include <QtCore/QTimer>
//#include <QSplitter>
//#include <QStandardItemModel>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTimer::singleShot(0, this, SLOT(Initialize()));

    ui->friendTree->setSortingEnabled(TRUE);

    QTreeWidgetItem* header = new QTreeWidgetItem;
    header->setText(0, "user");
    header->setText(1, "nick");
    ui->friendTree->setHeaderItem(header);

    //QTreeWidgetItem* rowOnline = new QTreeWidgetItem(ui->friendTree);
    //rowOnline->setText(0, "Online");
    //rowOnline->setText(1, "");

    rowOnline = new QTreeWidgetItem(ui->friendTree);
    ui->friendTree->addTopLevelItem(rowOnline);
    rowOnline->setText(0, "Online");
    rowOnline->setText(1, "");

    rowOffline = new QTreeWidgetItem(ui->friendTree);
    ui->friendTree->addTopLevelItem(rowOffline);
    rowOffline->setText(0, "Offline");
    rowOffline->setText(1, "");
    ///////////////////////////////////////////////////

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigFriendList(QStringList)),
                         this, SLOT(WhenMyFriendListComes(QStringList)));

    QObject::connect(ui->btnAddFriend, SIGNAL(clicked()), this, SLOT( AddFriend()));

    //등록 화면과 시그널을 공유함
    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigAddFriendOK(QString)),
                         this, SLOT(AddNewFriendItem(QString)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
bool MainWindow::event(QEvent *event)
{
    int returnValue = QWidget::event(event);

    if (event->type() == QEvent::Polish)
    {
        LogInDialog logindlg(this);

        QObject::connect( &logindlg,SIGNAL(finished(int)), this, SLOT( mainExit() ));

        logindlg.show();
        logindlg.exec();

        return true;
    }

    return returnValue;
}
*/
////////////////////////////////////////////////////////////////////////////////
void MainWindow::AddNewFriendItem(QString friendid)
{
    qDebug("AddNewFriendItem");
    //TODO : on off line?
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::AddFriend()
{
    AddNewFriendDialog addFriendDlg(this);
    //QObject::connect( &addFriendDlg,SIGNAL(sigLogInSuccess(QString)), this, SLOT( MainShow(QString) ));

    addFriendDlg.userid = this->userid;
    addFriendDlg.show();
    addFriendDlg.exec();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::Initialize()
{
    qDebug("Initialize");

    LogInDialog logindlg(this);

    QObject::connect( &logindlg,SIGNAL(sigLogInExit()), this, SLOT( MainExit() ));
    QObject::connect( &logindlg,SIGNAL(sigLogInSuccess(QString)), this, SLOT( MainShow(QString) ));

    logindlg.show();
    logindlg.exec();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::MainExit()
{
    qDebug("mainExit");

    close();
}

////////////////////////////////////////////////////////////////////////////////
void MainWindow::MainShow(QString userid)
{
    this->userid = userid;
    qDebug("mainShow[%s]",+ this->userid.toUtf8().constData());

    this->show();

    //get friend list !!!!
    NetManager::GetInstance().RequestFriendList(this->userid);

}


void MainWindow::WhenMyFriendListComes(QStringList friendList)
{
    // FRIENDLIST|aaa|offline|bbb|offline|ccc|offline|
    for( int i = 1; i < friendList.length(); i++) //1 ==> except for FRIENDLIST
    {
        qDebug("[%d]friendlist [%s]", i, friendList[i].toLocal8Bit().constData() );

        if( i % 2 == 0 )
        {
            //2nd-->on/off-line
            if( "online" == friendList[i] )
            {
                QTreeWidgetItem* subItem = new QTreeWidgetItem;
                subItem->setText(0, friendList[i-1] );
                subItem->setText(1, "nick1");
                rowOnline->addChild(subItem);
            }
            else
            {
                QTreeWidgetItem* subItem = new QTreeWidgetItem;
                subItem->setText(0, friendList[i-1] );
                subItem->setText(1, "nick1");
                rowOffline->addChild(subItem);
            }
        }
    }
    /*
    QStringList::const_iterator constIterator;
    for (constIterator = friendList.constBegin();
         constIterator != friendList.constEnd();
         ++constIterator)
    {
        qDebug("[%d]friendlist [%s]", nIndex, (*constIterator).toLocal8Bit().constData() );
    }
    */
    ui->friendTree->expandAll();
}
