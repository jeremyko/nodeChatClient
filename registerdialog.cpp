#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "netmanager.h"

#include <QtGui/QMessageBox>

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    QObject::connect(ui->btnCancelRegister, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(ui->btnUserIdDupCheck, SIGNAL(clicked()), this, SLOT( userIdDupCheck()));
    QObject::connect(ui->btnRegister, SIGNAL(clicked()), this, SLOT(enrollUser() ));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigCheckIdOK()),
                         this, SLOT(WhenCheckIdOK()));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigCheckIdFAIL(QString)),
                         this, SLOT(WhenCheckIdFAIL(QString)));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigEnrollUSerOK()),
                         this, SLOT(WhenEnrollUSerOK()));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigEnrollUSerFAIL(QString)),
                         this, SLOT(WhenEnrollUSerFAIL(QString)));


    //ui->lblErrorText->setStyleSheet("QLabel { background-color : red; color : blue; }");
    ui->lblErrorText->setText("");

    ui->PasswdInput->setEchoMode(QLineEdit::Password);
    ui->ConfirmPasswdInput->setEchoMode(QLineEdit::Password);

}

////////////////////////////////////////////////////////////////////////////////
RegisterDialog::~RegisterDialog()
{
    delete ui;
}

////////////////////////////////////////////////////////////////////////////////
//slots
void RegisterDialog::userIdDupCheck()
{
    qDebug("userIdDupCheck");
    ui->lblErrorText->setText("");
    NetManager::GetInstance().RequestUserIdDupCheck( ui->UserIDInput->toPlainText() );
}

void RegisterDialog::WhenCheckIdOK()
{
    qDebug("WhenCheckIdOK");
    ui->lblErrorText->setStyleSheet("QLabel { color : blue; }");
    ui->lblErrorText->setText("Your id is available");
}

void RegisterDialog::WhenCheckIdFAIL(QString errStr)
{
    qDebug("WhenCheckIdFAIL");
    ui->lblErrorText->setStyleSheet("QLabel { color : red; }");
    ui->lblErrorText->setText(errStr);
    ui->lblErrorText->setFocus();
}

void RegisterDialog::WhenEnrollUSerOK()
{
    qDebug("WhenEnrollUSerOK");
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->setText("enrollment success");
    msgBox->setWindowModality(Qt::NonModal);
    msgBox->setInformativeText("Welcome,"+ui->UserIDInput->toPlainText());
    msgBox->setStandardButtons(QMessageBox::Close);
    msgBox->setDefaultButton(QMessageBox::Close);
    msgBox->exec();

    close();
}

void RegisterDialog::WhenEnrollUSerFAIL(QString errStr)
{
    qDebug("WhenEnrollUSerFAIL");
    ui->lblErrorText->setStyleSheet("QLabel { color : red; }");
    ui->lblErrorText->setText(errStr);
}

void RegisterDialog::enrollUser()
{
    qDebug("enroll User");
    ui->lblErrorText->setText("");

    if(ui->PasswdInput->text() != ui->ConfirmPasswdInput->text() )
    {
        ui->lblErrorText->setStyleSheet("QLabel { color : red; }");
        ui->lblErrorText->setText("check your password !!");
        return;
    }

    qDebug("enroll User:RequestEnrollUser");
    NetManager::GetInstance().RequestEnrollUser( ui->UserIDInput->toPlainText(),
                                                 ui->PasswdInput->text(),
                                                 ui->NickInput->toPlainText());
}
