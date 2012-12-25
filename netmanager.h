#ifndef NETMANAGER_H
#define NETMANAGER_H


#include <QtNetwork/QHostAddress>
#include <QtCore/QString>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QtNetwork>
#include <QtCore/QTime>
#include <QMap>

#define DELIM "|"

static const int MaxBufferSize = 1024000;


class NetManager : public QObject
{
    Q_OBJECT

public:
    static NetManager& GetInstance()
    {
        static NetManager instance;
        return instance;
    }

    void DummyInit() ;
    bool SendMsg(QString& data);

    //request to server
    bool RequestLogIn(QString userid, QString passwd);
    bool RequestUserIdDupCheck(QString userid);
    bool RequestEnrollUser(QString userid, QString pass,QString nick);
    bool RequestFriendList(QString userid);
    bool RequestAddNewFriend(QString userid, QString friendid);
    bool RequestRemoveFriend(QString userid, QString friendid);
    bool RequestChat(QString userid, QString friendid, QString msg);

private slots:
    void ProcessReadyRead();
    void ServerConnected();
    void DisplayError(QAbstractSocket::SocketError socketError);

signals:
     void sigServerConnected();
     void sigServerError(QString errStr);
     void sigLogInOK();
     void sigLogInFAIL(QString errStr);

     void sigCheckIdOK();
     void sigCheckIdFAIL(QString errStr);

     void sigEnrollUSerOK();
     void sigEnrollUSerFAIL(QString errStr);

     void sigFriendList(QStringList friendList);

     void sigAddFriendOK(QString, QString,QString); //friendid
     void sigAddFriendFAIL(QString errStr);

     void sigRemoveFriendFAIL(QString errStr);
     void sigRemoveFriendOK(QString friendid);

     void sigChatMsg(QStringList msgData);
     void sigSomeOneLoggedIn(QStringList strList);
     void sigSomeOneLoggedOut(QStringList strList);

private:
    NetManager();
    QTcpSocket *tcpSocket;
    QNetworkSession *networkSession;

    QString hostName;
    quint16 hostPort;

    QByteArray totalBuffered;
    QString strPacketMsg;
    QString strPacketUsage;
    //void PacketHandle(QDataStream* in, uint nMsgLen);
    typedef void (NetManager::* pHandleFunc )(QStringList strList) ;
    QMap< QString, pHandleFunc > recvPacketHandleFuncMap;

    // server response handling ////////////////////////////
    void HandleResponseLogIn(QStringList strList);
    void HandleResponseEnrollUser(QStringList strList);
    void HandleResponseCheckId(QStringList strList);
    void HandleResponseFriendList(QStringList strList);
    void HandleResponseAddFriend(QStringList strList);
    void HandleResponseRemoveFriend(QStringList strList);
    ///////////////////////////////////////////////////
    void HandleChatMsg(QStringList strList);
    void HandleLoggedIn(QStringList strList);
    void HandleLoggedOut(QStringList strList);
    bool SendDataBlock (QByteArray msg );
};


#endif // NETMANAGER_H
