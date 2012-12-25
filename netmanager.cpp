#include "netmanager.h"

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
    recvPacketHandleFuncMap["DELETEFRIEND"] = &NetManager::HandleResponseRemoveFriend;
    recvPacketHandleFuncMap["CHATMSG"] = &NetManager::HandleChatMsg;
    recvPacketHandleFuncMap["LOGGED-IN"] = &NetManager::HandleLoggedIn;
    recvPacketHandleFuncMap["LOGGED-OUT"] = &NetManager::HandleLoggedOut;

}

void NetManager::HandleLoggedOut(QStringList strList)
{
    qDebug("HandleLoggedOut...");
    emit sigSomeOneLoggedOut(strList);

}

void NetManager::HandleLoggedIn(QStringList strList)
{
    qDebug("HandleLoggedIn...");
    emit sigSomeOneLoggedIn(strList);

}

void NetManager::HandleChatMsg(QStringList strList)
{
    qDebug("HandleChatMsg...");

    emit sigChatMsg(strList);
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
void NetManager::HandleResponseRemoveFriend(QStringList strList)
{
    //DELETEFRIEND|OK|friendid
    //DELETEFRIEND|FAIL|err string
    qDebug("HandleResponseRemoveFriend...");

    qDebug( "0: [%s]", strList[0].toUtf8().constData() );
    qDebug( "1: [%s]", strList[1].toUtf8().constData() );
    qDebug( "2: [%s]", strList[2].toUtf8().constData() );

    if( strList[1] == "FAIL" )
    {
        qDebug("HandleResponseRemoveFriend...FAIL!!!");
        emit sigRemoveFriendFAIL( strList[2] );
    }
    else
    {
        qDebug("HandleResponseRemoveFriend...OK");
        emit sigRemoveFriendOK(strList[2] ); //friendid
    }
}
////////////////////////////////////////////////////////////////////////////////
void NetManager::HandleResponseAddFriend(QStringList strList)
{
    //ADDFRIEND|OK|friendid|nick|online
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
        emit sigAddFriendOK(strList[2],strList[3],strList[4] ); //friendid,nick, on/offline
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
    /* writeRawData 사용시 다음처럼 4바이트로 전달이 되고, << msg.data();사용시에는 4바이트가 덧붙여짐
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

bool NetManager::RequestChat(QString userid, QString friendid, QString chatMsg)
{
    QString usage = "CHATMSG";
    QByteArray msg = (usage + DELIM + userid+ DELIM + friendid+ DELIM + chatMsg).toUtf8();

    return SendDataBlock(msg);
}

////////////////////////////////////////////////////////////////////////////////
bool NetManager::RequestRemoveFriend(QString userid, QString friendid)
{
    QString usage = "DELETEFRIEND";
    QByteArray msg = (usage + DELIM + userid+ DELIM + friendid).toUtf8();

    return SendDataBlock(msg);
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
    int packetHeaderLen = (int)sizeof(quint32);
    qDebug("\n---- recved data ------\n");
    qint64 bytesAvailable = tcpSocket->bytesAvailable();
    qDebug("bytesAvailable[%lld]",  bytesAvailable );
    if(bytesAvailable <= packetHeaderLen)
    {
        qDebug("bytesAvailable less than [%d] wait...",  (int)sizeof(quint32) );
        return;
    }

    QByteArray  recved = tcpSocket->readAll();
    totalBuffered = totalBuffered + recved;
    qDebug("1.totalBuffered.size [%d]",  totalBuffered.size() );
    QDataStream in (&totalBuffered, QIODevice::ReadOnly);

    //get msg length
    uint nMsgLen;
    in >> nMsgLen ; // int 4 byte
    qDebug("nMsgLen=[%d]", nMsgLen );

    if( (uint)totalBuffered.length() < nMsgLen)
    {
        qDebug("패킷길이만큼 못받음...skip");
        return;
    }

    //PacketHandle(&in, nMsgLen);

    char* szData = new char [nMsgLen+1]; //len은 null제외한 길이임.
    memset(szData, 0x00, sizeof(szData));
    in.readRawData ( szData, nMsgLen );
    szData[nMsgLen]=0; // 전송되는 문자열에는 nulll이 없다.null 붙임.
    qDebug("szData=[%s]",  szData );
    QString strPacketMsg = QString::fromUtf8( szData);
    QStringList msgList = strPacketMsg.split(DELIM);
    delete [] (szData);
    QString strUsage = msgList.at(0);
    qDebug( "strUsage: [%s]", strUsage.toUtf8().constData() );

    //process packet data
    (*this.*((recvPacketHandleFuncMap[ strUsage ])))(msgList);

    //totalBuffered 에서 len + 4 만큼을 제거 => one packet length
    totalBuffered = totalBuffered.mid(nMsgLen+4);
    int nSize = totalBuffered.size();
    qDebug("totalBuffered.size [%d]",  nSize ); // 0

    while( totalBuffered.size() >= (int) (nMsgLen + packetHeaderLen) )
    {
        QDataStream inTmp(&totalBuffered, QIODevice::ReadOnly);
        inTmp >> nMsgLen ;
        qDebug("#nMsgLen=[%d]", nMsgLen );

        char* szData = new char [nMsgLen+1]; //len은 null제외한 길이임.
        memset(szData, 0x00, sizeof(szData));
        inTmp.readRawData ( szData, nMsgLen );
        szData[nMsgLen]=0; // 전송되는 문자열에는 nulll이 없다.null 붙임.
        qDebug("#szData=[%s]",  szData );

        strPacketMsg = QString::fromUtf8( szData);
        qDebug("#strPacketMsg[%s]", strPacketMsg.toUtf8().constData());
        QStringList msgList = QString(szData).split(DELIM);
        delete [] (szData);
        QString strUsage = msgList.at(0);
        qDebug( "#strUsage: [%s]", strUsage.toUtf8().constData() );
        //process packet data
        (*this.*((recvPacketHandleFuncMap[ strUsage ])))(msgList);

        //PacketHandle(inTmp, nMsgLen);

        //totalBuffered 에서 len + 4 만큼을 제거 => one packet length
        totalBuffered = totalBuffered.mid(nMsgLen+4);
        int nSize = totalBuffered.size();
        qDebug("#totalBuffered.size [%d]",  nSize ); // 0
    }
}

//test
/*
void NetManager::PacketHandle(QDataStream* in, uint nMsgLen)
{
    char* szData = new char [nMsgLen+1]; //len은 null제외한 길이임.
    memset(szData, 0x00, sizeof(szData));
    in->readRawData ( szData, nMsgLen );
    szData[nMsgLen]=0; // 전송되는 문자열에는 nulll이 없다.null 붙임.
    qDebug("#szData=[%s]",  szData );

    strPacketMsg = QString::fromUtf8( szData);
    qDebug("#strPacketMsg[%s]", strPacketMsg.toUtf8().constData());
    QStringList msgList = QString(szData).split(DELIM);
    delete [] (szData);
    QString strUsage = msgList.at(0);
    qDebug( "#strUsage: [%s]", strUsage.toUtf8().constData() );
    //process packet data
    (*this.*((recvPacketHandleFuncMap[ strUsage ])))(msgList);

}
*/
////////////////////////////////////////////////////////////////////////////////
void NetManager::DisplayError(QAbstractSocket::SocketError socketError)
{
    QString errStr ;

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

    qCritical("%s", qPrintable(errStr));
    emit sigServerError(errStr);
}

