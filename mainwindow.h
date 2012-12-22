#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void Initialize();
    void MainExit();
    void MainShow(QString userid);
    void WhenMyFriendListComes(QStringList friendList);
    void AddFriend();
    void AddNewFriendItem(QString friendid);


private:
    Ui::MainWindow *ui;
    QString userid;
    QTreeWidgetItem* rowOnline, *rowOffline;
    //bool event(QEvent *event);
};

#endif // MAINWINDOW_H
