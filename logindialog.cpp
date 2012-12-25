#include "logindialog.h"
#include "ui_logindialog.h"
#include "registerdialog.h"
#include "netmanager.h"

#include "mainwindow.h"
////////////////////////////////////////////////////////////////////////////////
LogInDialog::LogInDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogInDialog)
{
    ui->setupUi(this);

    NetManager::GetInstance().DummyInit();

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigServerConnected()),
                         this, SLOT(ServerConnected()));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigServerError(QString)),
                         this, SLOT(ServerError(QString)));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigLogInOK()),
                         this, SLOT(WhenLogInOK()));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigLogInFAIL(QString)),
                         this, SLOT(WhenlogInFAIL(QString)));


    QObject::connect(ui->btnClose, SIGNAL(clicked()), this, SLOT( ExitNow()));
    QObject::connect(ui->btnLogin, SIGNAL(clicked()), this, SLOT(LogIn()));
    QObject::connect(ui->btnRegister, SIGNAL(clicked()), this, SLOT(RegisterUser()));

    ui->progressMsg->setStyleSheet("QLabel { color : red; }");

    //init state disable
    ui->btnLogin->setEnabled(false);
    ui->btnRegister->setEnabled(false);

    ui->UserPasswdInput->setEchoMode(QLineEdit::Password);
}
////////////////////////////////////////////////////////////////////////////////
LogInDialog::~LogInDialog()
{
    delete ui;
}
////////////////////////////////////////////////////////////////////////////////
void LogInDialog::ServerConnected()
{
    qDebug("LogInDialog::serverConnected()");
    ui->btnLogin->setEnabled(true);
    ui->btnRegister->setEnabled(true);
}
////////////////////////////////////////////////////////////////////////////////
void LogInDialog::ServerError(QString errStr)
{
    ui->progressMsg->setText(errStr);
    qDebug("LogInDialog::serverError! [%s]", qPrintable(errStr) );
}
////////////////////////////////////////////////////////////////////////////////
void LogInDialog::LogIn()
{
    qDebug("logIn");
    NetManager::GetInstance().RequestLogIn( ui->UserIDInput->toPlainText() ,
                                     ui->UserPasswdInput->text()  );
}
////////////////////////////////////////////////////////////////////////////////
void LogInDialog::WhenLogInOK()
{
    qDebug("logIn OK");
    emit sigLogInSuccess(ui->UserIDInput->toPlainText());

    close();
}

////////////////////////////////////////////////////////////////////////////////
void LogInDialog::WhenlogInFAIL(QString err)
{
    qDebug("logIn FAIL[%s]" , err.toUtf8().constData());
    ui->progressMsg->setText(err);
}

////////////////////////////////////////////////////////////////////////////////
void LogInDialog::RegisterUser()
{
    qDebug("RegisterUser");
    RegisterDialog dlg(this);

    dlg.show();
    dlg.exec();
}

void LogInDialog::ExitNow()
{
    qDebug("ExitNow");
    emit sigLogInExit();
}
