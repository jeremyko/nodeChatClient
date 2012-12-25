#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include "chatdialog.h"

namespace Ui {
class MainWindow;
}

typedef QMap<QString, ChatDialog*> DlgMapT;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void pressedRButton(QTreeWidget *, QTreeWidgetItem *pCurItem);
    void itemClicked(QTreeWidgetItem *, int);

public slots:
    void Initialize();
    void MainExit();
    void MainShow(QString userid);
    void WhenMyFriendListComes(QStringList friendList);
    void AddFriend();
    void RemoveFriend();
    void AddNewFriendItem(QString friendid,QString friendNick, QString OnOffLine);

    void WhenRemoveFriendResultFAIL(QString err);
    void WhenRemoveFriendResultOK(QString friendid);

    void WhenChatMsgComes(QStringList msgData);
    void WhenSomeOneLoggedIn(QStringList data);
    void WhenSomeOneLoggedOut(QStringList data);


    void OnTreeLButtonDbClicked( QTreeWidgetItem*, int);

    void OnChatDlgClosing(QString friendid);

private:
    Ui::MainWindow *ui;
    QString userid;
    QTreeWidgetItem* rowOnline, *rowOffline;
    DlgMapT dlgMap;
};

#endif // MAINWINDOW_H
