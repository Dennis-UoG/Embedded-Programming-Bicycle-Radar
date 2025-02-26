#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "tcpserver.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class mainWindow;
}
QT_END_NAMESPACE

struct TcpStct{
    union{
        char b_char[8];
        float b_double;
    }b_union;
};

class mainWindow : public QMainWindow
{
    Q_OBJECT
public:
    mainWindow(QWidget *parent = nullptr);
    ~mainWindow();

signals:
    void SG_Listen(const quint16& port, const bool& flag);

    void SG_SendData(const QByteArray &data);

private slots:
    void on_pushButton_getIP_clicked();

    void on_pushButton_listen_clicked();

    void on_pushButton_ok_clicked();

public slots:
    void SL_receivedData(const QByteArray &data);

private:
    QString getLocalip();

private:
    Ui::mainWindow *ui;

    QThread * thread = nullptr;
    TcpServer*  M_TcpServer = nullptr;

    TcpStct m_TcpStct;
};
#endif // MAINWINDOW_H
