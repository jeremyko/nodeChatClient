#include "chatdialog.h"
#include "ui_chatdialog.h"
#include "netmanager.h"
#include <QtGui/QVBoxLayout>

ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::ChatDialog(QString myid, QString friendid, QString firstMsg,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    this->myid = myid;
    this->friendid = friendid;
    this->initWithThisMsg = firstMsg;

    qDebug("this->myid[%s]", this->myid.toUtf8().constData() );
    qDebug("this->friendid[%s]", this->friendid.toUtf8().constData() );

    QObject::connect(ui->btnSendMsg, SIGNAL(clicked()), this, SLOT( SendMsg()));

    QTimer::singleShot(0, this, SLOT(Initialize()));
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

bool ChatDialog::event ( QEvent * event )
{
    int returnValue = QWidget::event(event);

    if (event->type() == QEvent::Close)
    {
        qDebug("Close Event");
        emit ChatDlgClosing(friendid);
    }

    return returnValue;
}

void ChatDialog::Initialize()
{
    qDebug("Initialize");
    if(this->initWithThisMsg.length() > 0 )
    {
        ui->ChatMsgsTE->append(initWithThisMsg);
    }

    //QVBoxLayout* vbox = new QVBoxLayout(this);
    //vbox->addWidget(ui->ChatMsgsTE);
    //vbox->addWidget(ui->MyMsgTE);
    //vbox->addWidget(ui->btnSendMsg);
    //setLayout(vbox);
    /*

        QVBoxLayout vbox = new QVBoxLayout(this);

        QVBoxLayout vbox1 = new QVBoxLayout();
        QHBoxLayout hbox1 = new QHBoxLayout();
        QHBoxLayout hbox2 = new QHBoxLayout();

        QLabel windLabel = new QLabel("Windows", this);
        QTextEdit edit = new QTextEdit(this);
        edit.setEnabled(false);

        QPushButton activate = new QPushButton("Activate", this);
        QPushButton close = new QPushButton("Close", this);
        QPushButton help = new QPushButton("Help", this);
        QPushButton ok = new QPushButton("OK", this);

        vbox.addWidget(windLabel);

        vbox1.addWidget(activate);
        vbox1.addWidget(close, 0, AlignmentFlag.AlignTop);
        hbox1.addWidget(edit);
        hbox1.addLayout(vbox1);

        vbox.addLayout(hbox1);

        hbox2.addWidget(help);
        hbox2.addStretch(1);
        hbox2.addWidget(ok);

        vbox.addLayout(hbox2, 1);

        setLayout(vbox);


     */
}


void ChatDialog::SendMsg()
{
    NetManager::GetInstance().RequestChat( myid,  friendid,  ui->MyMsgTE->toPlainText () );
    ui->ChatMsgsTE->append(ui->MyMsgTE->toPlainText ());
    ui->MyMsgTE->clear();
}

void ChatDialog::AppendMsg(QString newMsg)
{
    qDebug("newMsg[%s]", newMsg.toUtf8().constData() );
    ui->ChatMsgsTE->append(newMsg);
}
