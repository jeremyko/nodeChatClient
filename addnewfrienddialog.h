#ifndef ADDNEWFRIENDDIALOG_H
#define ADDNEWFRIENDDIALOG_H

#include <QDialog>

namespace Ui {
class AddNewFriendDialog;
}

class AddNewFriendDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddNewFriendDialog(QWidget *parent = 0);
    ~AddNewFriendDialog();

    QString userid;

public slots:
    void AddToMyFriend();
    void AddFriendOK(QString friendid);
    void AddFriendFAIL(QString err);

private:
    Ui::AddNewFriendDialog *ui;
};

#endif // ADDNEWFRIENDDIALOG_H
