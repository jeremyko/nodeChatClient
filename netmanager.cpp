#include "netmanager.h"
#include "packets.h"

#include <QtNetwork/QtNetwork>

#define SERVER_HOST "localhost"
static const int SERVER_PORT = 8124;

//NetManager::NetManager(QObject *parent) //: QTcpSocket(parent)
////////////////////////////////////////////////////////////////////////////////
NetManager::NetManager()
{
    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, SIGNAL(connected()),this, SLOT(ServerConnected()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(ProcessReadyRead()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(DisplayError(QAbstractSocket::SocketError)));

    //connect server
    tcpSocket->connectToHost(SERVER_HOST, SERVER_PORT );
    totalBuffered.clear();

    //handling functions
    recvPacketHandleFuncMap["REGISTER"] = &NetManager::HandleResponseEnrollUser;
    recvPacketHandleFuncMap["LOGIN"] = &NetManager::HandleResponseLogIn;
    recvPacketHandleFuncMap["CHKID"] = &NetManager::HandleResponseCheckId;
    recvPacketHandleFuncMap["FRIENDLIST"] = &NetManager::HandleResponseFriendList;
    recvPacketHandleFuncMap["ADDFRIEND"] = &NetManager::HandleResponseAddFriend;


}

////////////////////////////////////////////////////////////////////////////////
void NetManager::HandleResponseFriendList(QStringList strList)
{
    emit sigFriendList(strList);
}

////////////////////////////////////////////////////////////////////////////////
void NetManager::HandleResponseEnrollUser(QStringList strList)
{
    qDebug("HandleResponseEnrollUser...");
    qDebug( "0: [%s]", strList[0].toUtf8().constData() );
    qDebug( "1: [%s]", strList[1].toUtf8().constData() );

    if( strList[1] == "FAIL" )
    {
        qDebug("HandleResponseEnrollUser...FAIL!!!");
        emit sigEnrollUSerFAIL( strList[2] );
    }
    else
    {
        qDebug("HandleResponseEnrollUser...OK");
        emit sigEnrollUSerOK();
    }
}
////////////////////////////////////////////////////////////////////////////////
void NetManager::HandleResponseAddFriend(QStringList strList)
{
    //ADDFRIEND|OK|friendid
    //ADDFRIEND|FAIL|err string
    qDebug("HandleResponseAddFriend...");

    qDebug( "0: [%s]", strList[0].toUtf8().constData() );
    qDebug( "1: [%s]", strList[1].toUtf8().constData() );

    if( strList[1] == "FAIL" )
    {
        qDebug("HandleResponseAddFriend...FAIL!!!");
        emit sigAddFriendFAIL( strList[2] );
    }
    else
    {
        qDebug("HandleResponseAddFriend...OK");
        emit sigAddFriendOK(strList[2]); //friendid
    }
}

////////////////////////////////////////////////////////////////////////////////
void NetManager::HandleResponseLogIn(QStringList strList)
{
    qDebug("HandleResponseLogIn...");

    qDebug( "0: [%s]", strList[0].toUtf8().constData() );
    qDebug( "1: [%s]", strList[1].toUtf8().constData() );

    if( strList[1] == "FAIL" )
    {
        qDebug("HandleLogIn...FAIL!!!");
        emit sigLogInFAIL( strList[2] );
    }
    else
    {
        qDebug("HandleLogIn...OK");
        emit sigLogInOK();
    }

    //LOGIN|FAIL|No such user exists!
}

////////////////////////////////////////////////////////////////////////////////
void NetManager::HandleResponseCheckId (QStringList strList)
{
    qDebug("HandleResponseCheckId...");
    //qDebug( "0: [%s]", strList[0].toUtf8().constData() );
    //qDebug( "1: [%s]", strList[1].toUtf8().constData() );
    if( strList[1] == "FAIL" )
    {
        qDebug("HandleLogIn...FAIL!!!");
        emit sigCheckIdFAIL( strList[2] );
    }
    else
    {
        qDebug("HandleLogIn...OK");
        emit sigCheckIdOK();
    }
}

////////////////////////////////////////////////////////////////////////////////
void NetManager::DummyInit()
{
}

////////////////////////////////////////////////////////////////////////////////
void NetManager::ServerConnected()
{
    qDebug("connected to server");
    emit sigServerConnected();
}

////////////////////////////////////////////////////////////////////////////////
bool NetManager::SendDataBlock ( QByteArray msg )
{
    QByteArray dataBlock ;
    QDataStream out(&dataBlock, QIODevice::WriteOnly);

    unsigned int nlen = msg.size();
    qDebug("nlen= %d", nlen);
    out << nlen ; // int 4 byte
    out.writeRawData (msg.data(), nlen );
    /* writeRawData 사용시다음처럼 4바이트로 전달이 되고, << msg.data();사용시에는 4바이트가 덧붙여짐
     data[0]=0
     data[1]=0
     data[2]=0
     data[3]=13
     */
    int nLenTot = dataBlock.length();
    qDebug("dataBlock.length()= %d", nLenTot);

    if ( tcpSocket-> write(dataBlock) != dataBlock.size())
    {
        //error!!
        qDebug("Error/NetManager::LogIn/%s", qPrintable(tcpSocket->errorString()) );
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool NetManager::RequestAddNewFriend(QString userid, QString friendid)
{
    QString usage = "ADDFRIEND";
    QByteArray msg = (usage + DELIM + userid+ DELIM + friendid).toUtf8();

    return SendDataBlock(msg);
}

////////////////////////////////////////////////////////////////////////////////
bool NetManager::RequestFriendList(QString userid)
{
    QString usage = "FRIENDLIST";
    QByteArray msg = (usage + DELIM + userid ).toUtf8();

    return SendDataBlock(msg);
}

////////////////////////////////////////////////////////////////////////////////
bool NetManager::RequestUserIdDupCheck(QString userid)
{
    QString usage = "CHKID";
    QByteArray msg = (usage + DELIM + userid ).toUtf8();

    return SendDataBlock(msg);
}

////////////////////////////////////////////////////////////////////////////////
bool NetManager::RequestEnrollUser(QString userid, QString pass,QString nick)
{
    QString usage = "REGISTER";
    QByteArray msg = (usage + DELIM + userid+ DELIM+pass + DELIM+nick ).toUtf8();

    return SendDataBlock(msg);
}

////////////////////////////////////////////////////////////////////////////////
bool NetManager::RequestLogIn(QString userid, QString passwd)
{
    QString usage = "LOGIN";
    QByteArray msg = (usage + DELIM + userid + DELIM +passwd).toUtf8();

    return SendDataBlock(msg);
}


////////////////////////////////////////////////////////////////////////////////
bool NetManager::SendMsg(QString& message)
{
    if (message.isEmpty())
        return false;

    QByteArray msg = message.toUtf8();
    QByteArray data = "MESSAGE " + QByteArray::number(msg.size()) + ' ' + msg;
    if ( tcpSocket-> write(data) != data.size())
    {
        //error!!
        qDebug("Error/NetManager::sendMsg/%s", qPrintable(tcpSocket->errorString()) );
        return false;
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////
void NetManager::ProcessReadyRead()
{
    //qDebug("sizeof uint [%u]", sizeof(unsigned int) );
    //qDebug("sizeof unsigned short int [%u]", sizeof( unsigned short int ) );
    //qDebug("sizeof size_t [%d]", sizeof(size_t) );
    /////////////
    qDebug("\n---- recved data ------\n");
    qint64 bytesAvailable = tcpSocket->bytesAvailable();
    qDebug("bytesAvailable[%lld]",  bytesAvailable );
    if(bytesAvailable < (int)sizeof(quint32))
    {
        qDebug("bytesAvailable less than [%d] wait...",  (int)sizeof(quint32) );
        return;
    }

    QByteArray  recved = tcpSocket->readAll();
    totalBuffered = totalBuffered + recved;
    qDebug("1.totalBuffered.size [%d]",  totalBuffered.size() );
    QDataStream in (&totalBuffered, QIODevice::ReadOnly);
    uint len;
    in >> len ; // int 4 byte
    qDebug("len=[%d]", len );

    if( (uint)totalBuffered.length() < len)
    {
        qDebug("패킷길이만큼 못받음...skip");
        return;
    }

    char* szData = new char [len+1]; //len은 null제외한 길이임.
    //char* szDataString = new char [len+1];
    memset(szData, 0x00, sizeof(szData));
    //memset(szDataString, 0x00, sizeof(szDataString));
    in.readRawData ( szData, len );
    szData[len]=0; // 전송되는 문자열에는 nulll이 없다.null 붙임.
    qDebug("szData=[%s]",  szData );

    strPacketMsg = szData;
    qDebug("strPacketMsg[%s]", strPacketMsg.toUtf8().constData());
    QStringList msgList = QString(szData).split(DELIM);
    delete [] (szData);
    //QString strUsage = msgList.first();
    QString strUsage = msgList.at(0);
    qDebug( "strUsage: [%s]", strUsage.toUtf8().constData() );

    //process packet data
    (*this.*((recvPacketHandleFuncMap[ strUsage ])))(msgList);

    //totalBuffered 에서 len + 4 만큼을 제거 => one packet length
    totalBuffered = totalBuffered.mid(len+4);
    qDebug("2.totalBuffered.size [%d]",  totalBuffered.size() ); // 0
}

////////////////////////////////////////////////////////////////////////////////
void NetManager::DisplayError(QAbstractSocket::SocketError socketError)
{
    QString errStr = tcpSocket->errorString();
    /*
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        errStr = "HostNotFoundError";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        errStr ="ConnectionRefusedError";
        break;
    default:
        errStr = tcpSocket->errorString();
        qCritical("The following error occurred: %s", qPrintable(tcpSocket->errorString()));

    }
    */
    qCritical("%s", qPrintable(errStr));
    emit sigServerError(errStr);
}

////////////////////////////////////////////////////////////////////////////////
/*
// find out which IP to connect to
QString ipAddress;
QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
// use the first non-localhost IPv4 address
for (int i = 0; i < ipAddressesList.size(); ++i) {
    if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
        ipAddressesList.at(i).toIPv4Address()) {
        ipAddress = ipAddressesList.at(i).toString();
        break;
    }
}
// if we did not find one, use IPv4 localhost
if (ipAddress.isEmpty())
    ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
*/
/*
QNetworkConfigurationManager manager;
if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
    // Get saved network configuration
    QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
    settings.endGroup();

    // If the saved network configuration is not currently discovered use the system default
    QNetworkConfiguration config = manager.configurationFromIdentifier(id);
    if ((config.state() & QNetworkConfiguration::Discovered) !=
        QNetworkConfiguration::Discovered) {
        config = manager.defaultConfiguration();
    }

    networkSession = new QNetworkSession(config, this);
    connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

    //getFortuneButton->setEnabled(false);
    //statusLabel->setText(tr("Opening network session."));
    networkSession->open();
}
*/
////////////////
/*
PACKETLEN tmp ;
memset(&tmp, 0x00, sizeof(PACKETLEN));
tmp.len = 99999;
memcpy(tmp.szTest, "100", sizeof(tmp.szTest));
tcpSocket->write((const char*)&tmp, sizeof(tmp));
*/

/////////////////
/*
QByteArray block;

QDataStream out(&block, QIODevice::WriteOnly);
out << quint32(tmp.len) ;
out << tmp.szTest;
tcpSocket->write(block);
*/
/*
QByteArray msg = (userid + "|" +passwd).toUtf8();
//unsigned int nlen = msg.size();
unsigned int nlen = 9999999; //test
QByteArray len;
len.setNum( nlen );

QByteArray data ;
data.append(len);
data.append(msg);
qDebug("data=%s", data.data() );
*/
/*
QByteArray msg = (userid + " " +passwd).toUtf8();
QByteArray data = "LOGIN " + QByteArray::number(msg.size()) + ' ' + msg;
if ( tcpSocket-> write(data) != data.size())
{
    qDebug("Error/NetManager::LogIn/%s", qPrintable(tcpSocket->errorString()) );
    return false;
}
*/
//out << userid.toUtf8();
//out << DELIM;
//out << passwd.toUtf8();
/*
 *
QByteArray msg = (userid +DELIM +passwd).toUtf8();
QString strLen = QString("%1").arg(msg.size(), 5, 10, QChar('0')).toUpper();
//QString s = "apple";
//QString t = s.leftJustified(8, '.');    // t == "apple..."
//QByteArray dataBlock = QByteArray::number(msg.size()) + DELIM +msg;
QByteArray dataBlock = strLen.toUtf8() + DELIM + msg;
if ( tcpSocket-> write(dataBlock) != dataBlock.size())
{
    //error!!
    qDebug("Error/NetManager::LogIn/%s", qPrintable(tcpSocket->errorString()) );
    return false;
}
return true;

 */
