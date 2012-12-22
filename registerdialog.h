#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RegisterDialog(QWidget *parent = 0);
    ~RegisterDialog();

public slots:
    void userIdDupCheck();
    void WhenCheckIdOK();
    void WhenCheckIdFAIL(QString errStr);
    void WhenEnrollUSerOK();
    void WhenEnrollUSerFAIL(QString errStr);
    void enrollUser();

private:
    Ui::RegisterDialog *ui;

};

#endif // REGISTERDIALOG_H
