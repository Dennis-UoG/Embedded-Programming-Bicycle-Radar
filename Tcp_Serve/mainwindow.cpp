#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkInterface>
#include <QMessageBox>

mainWindow::mainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mainWindow)
{
    ui->setupUi(this);

    thread = new QThread;
    M_TcpServer = new TcpServer;
    M_TcpServer->moveToThread(thread);
    thread->start();

    connect(this, &mainWindow::SG_Listen, M_TcpServer, &TcpServer::SL_Listen);
    connect(this, &mainWindow::SG_SendData, M_TcpServer, &TcpServer::SL_SendData);

    connect(M_TcpServer, &TcpServer::SG_receivedData, this, &mainWindow::SL_receivedData);
}

mainWindow::~mainWindow()
{
    delete ui;

    if(M_TcpServer){
        M_TcpServer->deleteLater();
        M_TcpServer = nullptr;
    }

    if (thread) {
        if (thread->isRunning()) {
            thread->quit();  // 请求线程退出
            thread->wait();  // 等待线程退出
        }
        delete thread;  // 释放线程资源
        thread = nullptr; // 避免野指针
    }


}

// 获取本地IP
QString mainWindow::getLocalip()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            return address.toString();
        }
    }
    return "";
}

void mainWindow::SL_receivedData(const QByteArray &data)
{
    if(ui->checkBox_receive->isChecked()){
        ui->textBrowser->insertPlainText(data.toHex());
    }else{
        ui->textBrowser->insertPlainText(QString::fromUtf8(data));
    }
}

// 获取本机IP
void mainWindow::on_pushButton_getIP_clicked()
{
    ui->lineEdit_ip->setText(getLocalip());
}

// 开始监听
void mainWindow::on_pushButton_listen_clicked()
{
    bool ok;
    quint16 port = ui->lineEdit_port->text().toUInt(&ok);
    if(ok){
        if(ui->pushButton_listen->text() == "监听"){
            ui->pushButton_listen->setText("正在监听...");
            emit SG_Listen(port, true);
        }else{
            ui->pushButton_listen->setText("监听");
            emit SG_Listen(port, false);
        }
    } else{
        QMessageBox::information(this, tr("提示"), tr("请输入正确的端口号!"));
    }
}

// 发送数据
void mainWindow::on_pushButton_ok_clicked()
{
    QString AsciiStr = ui->textEdit_Send->toPlainText();
    if(ui->checkBox_send->isChecked()){
        emit SG_SendData(AsciiStr.toLatin1().toHex());
    }else{
        emit SG_SendData(AsciiStr.toLatin1());
    }
}

