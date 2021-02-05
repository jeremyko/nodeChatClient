#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "netmanager.h"
#include "addnewfrienddialog.h"


#include <QtCore/QTimer>
#include <QList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTimer::singleShot(0, this, SLOT(Initialize()));

    ui->friendTree->setSortingEnabled(true);

    QTreeWidgetItem* header = new QTreeWidgetItem;
    header->setText(0, "user");
    header->setText(1, "nick");
    ui->friendTree->setHeaderItem(header);

    rowOnline = new QTreeWidgetItem(ui->friendTree);
    ui->friendTree->addTopLevelItem(rowOnline);
    rowOnline->setText(0, "Online");
    rowOnline->setText(1, "");

    rowOffline = new QTreeWidgetItem(ui->friendTree);
    ui->friendTree->addTopLevelItem(rowOffline);
    rowOffline->setText(0, "Offline");
    rowOffline->setText(1, "");

    connect(ui->friendTree, SIGNAL( itemDoubleClicked(QTreeWidgetItem*,int)) , this,
            SLOT( OnTreeLButtonDbClicked( QTreeWidgetItem* , int ))  );

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigFriendList(QStringList)),
                         this, SLOT(WhenMyFriendListComes(QStringList)));

    QObject::connect(ui->btnAddFriend, SIGNAL(clicked()), this, SLOT( AddFriend()));
    QObject::connect(ui->btnRemoveFriend, SIGNAL(clicked()), this, SLOT( RemoveFriend()));

    //등록 화면과 시그널을 공유함
    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigAddFriendOK(QString,QString,QString)),
                         this, SLOT(AddNewFriendItem(QString,QString,QString)));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigRemoveFriendFAIL(QString)),
                         this, SLOT(WhenRemoveFriendResultFAIL(QString)));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigRemoveFriendOK(QString)),
                         this, SLOT(WhenRemoveFriendResultOK(QString)));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigChatMsg(QStringList)),
                         this, SLOT(WhenChatMsgComes(QStringList)));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigSomeOneLoggedIn(QStringList)),
                         this, SLOT(WhenSomeOneLoggedIn(QStringList)));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigSomeOneLoggedOut(QStringList)),
                         this, SLOT(WhenSomeOneLoggedOut(QStringList)));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::OnTreeLButtonDbClicked( QTreeWidgetItem* item, int nIdex)
{
    qDebug("OnTreeLButtonDbClicked:[%d][%s]", nIdex, item->parent()->text(0).toUtf8().constData());
    if(item->parent()->text(0) == "Offline")
    {
        return;
    }
    qDebug("OnTreeLButtonDbClicked:[%s]", item->text(0).toUtf8().constData()); //id
    qDebug("OnTreeLButtonDbClicked:[%s]", item->text(1).toUtf8().constData()); //nick

    ChatDialog* pChatDlg = new ChatDialog(this->userid,item->text(0),"", this);
    QObject::connect(pChatDlg, SIGNAL(ChatDlgClosing(QString)), this, SLOT( OnChatDlgClosing(QString)));

    dlgMap.insert(item->text(0), pChatDlg);
    pChatDlg->show();
}

void MainWindow::OnChatDlgClosing(QString friendid)
{
    DlgMapT::iterator i =  dlgMap.find(friendid);
    if(i != dlgMap.end())
    {
        qDebug( "Found in DlgMap");
        ChatDialog* pChatDlg = i.value();
        int nRtn = dlgMap.remove(friendid);
        if(nRtn >0)
        {
            qDebug( "delete");
            delete pChatDlg;
            pChatDlg = NULL;
        }
    }
}

void MainWindow::WhenSomeOneLoggedIn(QStringList data)
{
    qDebug("WhenSomeOneLoggedIn");
    qDebug( "0: [%s]", data[0].toUtf8().constData() );
    qDebug( "1: [%s]", data[1].toUtf8().constData() );//id
    qDebug( "2: [%s]", data[2].toUtf8().constData() );//nick

    QTreeWidgetItem* subItem = new QTreeWidgetItem;
    subItem->setText(0, data[1]);
    subItem->setText(1, data[2]);

    //remove from offline
    QList<QTreeWidgetItem*> items =
            ui->friendTree->findItems(data[1],Qt::MatchExactly|Qt::MatchRecursive,0);

    qDebug( "items.count: [%d]", items.count());
    if (items.count() > 0)
    {
        QTreeWidgetItem* removeItem = items[0];
        rowOffline->removeChild(removeItem);
        delete removeItem;
    }

    rowOnline->addChild(subItem);

    DlgMapT::iterator i =  dlgMap.find(data[1]);
    if(i != dlgMap.end())
    {
        ChatDialog* pChatDlg = i.value();
        pChatDlg->SetInputEnabled(true);
    }
}

void MainWindow::WhenSomeOneLoggedOut(QStringList data)
{
    qDebug("WhenSomeOneLoggedOut");

    QTreeWidgetItem* subItem = new QTreeWidgetItem;
    subItem->setText(0, data[1]);
    subItem->setText(1, data[2]);

    //remove from online
    QList<QTreeWidgetItem*> items =
            ui->friendTree->findItems(data[1],Qt::MatchExactly|Qt::MatchRecursive,0);

    qDebug( "items.count: [%d]", items.count());
    if (items.count() > 0)
    {
        QTreeWidgetItem* removeItem = items[0];
        rowOnline->removeChild(removeItem);
        delete removeItem;
    }

    rowOffline->addChild(subItem);

    DlgMapT::iterator i =  dlgMap.find(data[1]);
    if(i != dlgMap.end())
    {
        ChatDialog* pChatDlg = i.value();
        pChatDlg->SetInputEnabled(false);
    }
}

void MainWindow::WhenChatMsgComes(QStringList msgData)
{
    qDebug( "0: [%s]", msgData[0].toUtf8().constData() );
    qDebug( "1: [%s]", msgData[1].toUtf8().constData() );//friendid
    qDebug( "2: [%s]", msgData[2].toUtf8().constData() );//myid
    qDebug( "3: [%s]", msgData[3].toUtf8().constData() );//msg

    //find chat dlg
    DlgMapT::iterator i =  dlgMap.find(msgData[1]);
    if(i != dlgMap.end())
    {
        qDebug( "Found in DlgMap");
        ChatDialog* pChatDlg = i.value();
        pChatDlg->AppendMsg(msgData[3]);
    }
    else
    {
        ChatDialog* pChatDlg = new ChatDialog(this->userid,msgData[1],msgData[3], this);
        QObject::connect(pChatDlg, SIGNAL(ChatDlgClosing(QString)), this, SLOT( OnChatDlgClosing(QString)));
        dlgMap.insert(msgData[1], pChatDlg);
        pChatDlg->show();
    }
}

void MainWindow::WhenRemoveFriendResultFAIL(QString err)
{
    qDebug("WhenRemoveFriendResultFAIL[%s]", err.toUtf8().constData());
}

void MainWindow::WhenRemoveFriendResultOK(QString friendid)
{
    qDebug("WhenRemoveFriendResultOK [%s]", friendid.toUtf8().constData());

    QTreeWidgetItem *pRemove = ui->friendTree->currentItem();

    int i = ui->friendTree->indexOfTopLevelItem(pRemove);
    qDebug("indexOfTopLevelItem[%d]", i);
    if(i < 0 )
    {
        ui->friendTree->takeTopLevelItem(i);
        delete pRemove;
    }
}


////////////////////////////////////////////////////////////////////////////////
void MainWindow::AddNewFriendItem(QString friendid,QString friendNick, QString OnOffLine)
{
    qDebug("AddNewFriendItem");
    QTreeWidgetItem* subItem = new QTreeWidgetItem;
    subItem->setText(0,  friendid);
    subItem->setText(1, friendNick);

    if(OnOffLine == "online")
    {
        rowOnline->addChild(subItem);
    }
    else
    {
        rowOffline->addChild(subItem);
    }
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
void MainWindow::RemoveFriend()
{
    qDebug("RemoveFriend");
    //request remove friend

    QTreeWidgetItem *pRemove = ui->friendTree->currentItem();

    NetManager::GetInstance().RequestRemoveFriend(this->userid,pRemove->text(0) );
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
    // FRIENDLIST|id1|nick1|online|id2|nick2|offline...
    for( int i = 1; i < friendList.length(); i++) //1 ==> except for FRIENDLIST
    {
        qDebug("[%d]friendlist [%s]", i, friendList[i].toLocal8Bit().constData() );

        if( i % 3 == 0 )
        {
            QTreeWidgetItem* subItem = new QTreeWidgetItem;
            subItem->setText(0, friendList[i-2] );
            subItem->setText(1, friendList[i-1]);
            //3rd-->on/off-line
            if( "online" == friendList[i] )
            {
                rowOnline->addChild(subItem);
            }
            else
            {
                rowOffline->addChild(subItem);
            }
        }
    }

    ui->friendTree->expandAll();
}

