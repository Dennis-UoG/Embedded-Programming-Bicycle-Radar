//
// Created by Jiancheng Zhang on 10/02/2025.
//
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sstream>
#include <typeinfo>
#include <thread>


int main() {
    // Replace with your TTY device name
    const char* portName = "/dev/ttyUSB0";
    // Open the TTY port
    int serialPort = open(portName, O_RDWR | O_NOCTTY | O_NONBLOCK);
    // if (serialPort == -1) {
    //     std::cerr << "Error: Unable to open port " << portName << std::endl;
    //     return 1;
    // }
    // Configure the port
    struct StructLidar {
        float X;
        float Y;
        float Z;
    };
    struct termios tty;
    int result = tcgetattr(serialPort, &tty);
    if (result < 0) {
        perror ("error in tcgetattr");
        return 0;
    }
    cfsetospeed (&tty, B115200);
    cfsetispeed (&tty, B115200);
    // https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    // tty.c_cflag |= CREAD;
    // tty.c_iflag = 0;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag |= CREAD | CLOCAL;
    // tty.c_lflag |= ICANON;
    // tty.c_lflag &= ~ECHO; // Disable echo
    // tty.c_lflag &= ~ECHOE; // Disable erasure
    // tty.c_lflag &= ~ECHONL;
    // tty.c_lflag &= ~ISIG;
    tty.c_oflag = 0;
    // tty.c_lflag = 0;
    // http://www.unixwiz.net/techtips/termios-vmin-vtime.html
    // tty.c_cc[VTIME] = 0;
    // tty.c_cc[VMIN] = 1;

    result = tcsetattr(serialPort, TCSANOW, &tty);
    if (result < 0)
    {
        perror ("error in tcsetattr");
        return 0;
    }
    int mask  = 0;

    // Read data from the port
    char buffer[11];
    while (true) {
        int bytesRead = read(serialPort, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            if (buffer[0] == 0x55 && buffer[1] == 0x51) {
                StructLidar imu_data = {};
                float x = (mask | (int8_t (buffer[2]) | int16_t (buffer[3] << 8))) / 32768.0 * 16.0 * 9.8;
                float y = (mask | (int8_t (buffer[4]) | int16_t (buffer[5] << 8))) / 32768.0 * 16.0 * 9.8;
                float z = (mask | (int8_t (buffer[6]) | int16_t (buffer[7] << 8))) / 32768.0 * 16.0 * 9.8;
                imu_data.X = x;
                imu_data.Y = y;
                imu_data.Z = z;
                std::cout << "Temperature: " << imu_data.X << std::endl;
            }
            else {
                tcflush(serialPort, TCIFLUSH);
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(90));
        }
    }
    return 0;
}
