#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LogInDialog;
}

class LogInDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LogInDialog(QWidget *parent = 0);
    ~LogInDialog();

signals:
    void sigLogInSuccess(QString);
    void sigLogInExit();

public slots:
    void ServerConnected();
    void ServerError(QString errStr);
    void LogIn();
    void WhenLogInOK();
    void WhenlogInFAIL(QString errStr);
    void RegisterUser();

    void ExitNow();

private:
    Ui::LogInDialog *ui;


};

#endif // LOGINDIALOG_H
