#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ChatDialog(QWidget *parent = 0);
    ChatDialog(QString friendid, QString myid,QString firstMsg, QWidget *parent = 0);
    ~ChatDialog();
    QString myid,friendid, initWithThisMsg;

    void AppendMsg(QString newMsg);

signals:
    void ChatDlgClosing(QString friendid);

public slots:
    void SendMsg();
    void Initialize();

private:
    Ui::ChatDialog *ui;
    bool event(QEvent *event);
};

#endif // CHATDIALOG_H
