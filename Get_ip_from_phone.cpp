#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <fstream>
#include <sstream>
#include <string>

// 解析网关 IP（手机热点 IP）
std::string getGatewayIp() {
    std::ifstream file("/proc/net/route");
    std::string line, gateway;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string iface, destination, gatewayHex;
        int flags;

        if (iss >> iface >> destination >> gatewayHex >> flags) {
            if (destination == "00000000") {  // 默认网关
                std::stringstream ss;
                int gw;
                ss << std::hex << gatewayHex;
                ss >> gw;

                gateway = std::to_string(gw & 0xFF) + "." +
                    std::to_string((gw >> 8) & 0xFF) + "." +
                    std::to_string((gw >> 16) & 0xFF) + "." +
                    std::to_string((gw >> 24) & 0xFF);
                break;
            }
        }
    }

    return gateway;
}

// 发送信号到服务器（手机热点）
void sendSignalToServer(const QString& serverIp, int port, const QString& message) {
    QTcpSocket socket;
    socket.connectToHost(serverIp, port);

    if (socket.waitForConnected(3000)) {
        socket.write(message.toUtf8());
        socket.waitForBytesWritten();
        qDebug() << "? Signal sent to server:" << message;
    }
    else {
        qDebug() << "? Failed to connect to server.";
    }
}

int main() {
    std::string serverIp = getGatewayIp();  //获取手机热点 IP
    if (serverIp.empty()) {
        qDebug() << "? Failed to detect hotspot IP.";
        return -1;
    }

    qDebug() << "?? Detected Hotspot IP:" << QString::fromStdString(serverIp);  // 打印出 IP

    int port = 12345;
    sendSignalToServer(QString::fromStdString(serverIp), port, "Ring");

    return 0;
}

