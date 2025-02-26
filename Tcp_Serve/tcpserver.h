#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>

class TcpServer : public QObject
{
    Q_OBJECT

signals:
    void SG_receivedData(const QByteArray &data);

public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();
    bool startServer(quint16 port);

private:
    void SendData(QTcpSocket* targetSocket, const QByteArray &data);

private:
    QTcpServer * server = nullptr;
    QList<QTcpSocket*> activeSockets; // 改用容器存储连接
    QMap<QTcpSocket*, QString> clientMap1; // socket与客户端ID映射
    QMap<QString, QTcpSocket*> clientMap2; // 客户端ID与socket映射

public slots:
    void SL_Listen(const quint16& port, const bool& flag);
    void SL_HandleNewConnection();
    void SL_ProcessData(QTcpSocket* socket);
    void SL_SendData(const QByteArray &data);
};

#endif // TCPSERVER_H
