#include "chatdialog.h"
#include "ui_chatdialog.h"

ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);

    //ui->  textEdit->setReadOnly(true);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}
