#include "addnewfrienddialog.h"
#include "ui_addnewfrienddialog.h"
#include "netmanager.h"

AddNewFriendDialog::AddNewFriendDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewFriendDialog)
{
    ui->setupUi(this);

    QObject::connect(ui->btnClose, SIGNAL(clicked()), this, SLOT( close()));
    QObject::connect(ui->btnAddToMyFriend, SIGNAL(clicked()), this, SLOT( AddToMyFriend()));

    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigAddFriendOK(QString)),
                         this, SLOT(AddFriendOK(QString)));
    QObject::connect(&NetManager::GetInstance(), SIGNAL(sigAddFriendFAIL(QString)),
                         this, SLOT(AddFriendFAIL(QString)));
}

AddNewFriendDialog::~AddNewFriendDialog()
{
    delete ui;
}

////////////////////////////////////////////////////////////////////////////////
void AddNewFriendDialog::AddToMyFriend()
{
    NetManager::GetInstance().RequestAddNewFriend(this->userid,
                                                  ui->friendIdInput->text() );
}


void AddNewFriendDialog::AddFriendOK(QString friendid)
{
    ui->infoLabel->setText("Adding New Friend Success:"+friendid);
}

void AddNewFriendDialog::AddFriendFAIL(QString err)
{
    ui->infoLabel->setText(err);
}
