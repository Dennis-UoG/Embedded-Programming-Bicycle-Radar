#include "tcpserver.h"


TcpServer::TcpServer(QObject *parent)
    : QObject{parent}
    , server(new QTcpServer(this))  // 将 server 设置为 mainWindow 的子对象
{
    connect(server, &QTcpServer::newConnection, this, &TcpServer::SL_HandleNewConnection);
}

TcpServer::~TcpServer()
{
    // 关闭所有客户端连接
    foreach(QTcpSocket* socket, activeSockets) {
        socket->disconnectFromHost(); // 优雅断开
        if(socket->state() != QAbstractSocket::UnconnectedState) {
            socket->abort(); // 强制断开
        }
        socket->deleteLater();
    }
    activeSockets.clear();
    clientMap1.clear();
    clientMap2.clear();

    // 关闭服务器
    if(server->isListening()) {
        server->close();
    }
    delete server;
}

// 开始监听
bool TcpServer::startServer(quint16 port)
{
    if (server->listen(QHostAddress::Any, port)) {
        return true;
    } else {
        qDebug()<<"Error : Failed to start the server: " + server->errorString();
        return false;
    }
}

// 发送数据
void TcpServer::SendData(QTcpSocket* targetSocket, const QByteArray &data)
{
    if (targetSocket && targetSocket->state() == QAbstractSocket::ConnectedState)
    {
        targetSocket->write(data);
        targetSocket->flush();
    } else {
        qDebug() << "Error: Invalid target socket";
    }
}

void TcpServer::SL_Listen(const quint16 &port, const bool &flag)
{
    if (flag) {
        // 开始监听
        if (startServer(port)) {
            qDebug() << "TCP连接成功!";
        } else {
            qDebug() << "TCP连接失败!";
        }
    } else {
        // 停止监听
        qDebug() << "正在断开TCP监听...";
        qDebug() << "正在断开TCP监听...";

        // 关闭所有客户端连接
        foreach(QTcpSocket* socket, activeSockets) {
            socket->disconnectFromHost();
            if(socket->state() != QAbstractSocket::UnconnectedState) {
                socket->abort();
            }
            socket->deleteLater();
        }
        activeSockets.clear();
        clientMap1.clear();
        clientMap2.clear();

        // 关闭服务器
        if(server->isListening()) {
            server->close();
        }
        qDebug() << "TCP监听已断开，等待重新开始监听。";
    }
}


// 连接socket 开始接收或者发送数据
void TcpServer::SL_HandleNewConnection()
{
    while(server->hasPendingConnections()) {
        QTcpSocket* newSocket = server->nextPendingConnection();
        QString clientId = QString("CLIENT_%1").arg(quintptr(newSocket));
        activeSockets.append(newSocket);
        clientMap1.insert(newSocket, clientId);
        clientMap2.insert(clientId, newSocket);
        connect(newSocket, &QTcpSocket::readyRead,
                this, [this, newSocket](){ SL_ProcessData(newSocket); });
        connect(newSocket, &QTcpSocket::disconnected,
                this, [this, newSocket](){
                    qDebug() << "客户端断开:" << clientMap1.value(newSocket);
                    clientMap2.remove(clientMap1.value(newSocket));
                    clientMap1.remove(newSocket);
                    activeSockets.removeOne(newSocket);
                    newSocket->deleteLater();
                });
    }
}

// socket接收数据
void TcpServer::SL_ProcessData(QTcpSocket* socket)
{
    if(socket && socket->bytesAvailable()) {
        QByteArray data = socket->readAll();

        // 数据处理
        emit SG_receivedData(data);
    }
}

// socket发送数据
void TcpServer::SL_SendData(const QByteArray &data)
{
    for(int i = 0;i < activeSockets.size(); i++){
        SendData(activeSockets[i], data);
    }
}
