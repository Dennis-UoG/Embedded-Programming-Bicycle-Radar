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
    const char* portName = "/dev/tty.usbserial-1130";
    // Open the TTY port
    int serialPort = open(portName, O_RDWR | O_NOCTTY | O_NONBLOCK);
    // if (serialPort == -1) {
    //     std::cerr << "Error: Unable to open port " << portName << std::endl;
    //     return 1;
    // }
    // Configure the port
    struct StructLidar {
        float Temperature;
        float Dist;
        int Strength;
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
    unsigned int mask  = 0;

    // Read data from the port
    char buffer[9];
    while (true) {
        int bytesRead = read(serialPort, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            if (buffer[0] == 0x59 && buffer[1] == 0x59) {
                StructLidar temp_structLidar = {};
                float dist = (mask | (uint8_t (buffer[2]) | uint16_t (buffer[3] << 8))) / 100.0;
                int strength = uint8_t (buffer[4]) | uint16_t (buffer[5] << 8);
                float temperature = (mask | (uint8_t (buffer[6]) | uint16_t (buffer[7] << 8))) / 8 - 256;
                temp_structLidar.Temperature = temperature;
                temp_structLidar.Dist = dist;
                temp_structLidar.Strength = strength;
                std::cout << "Temperature: " << temp_structLidar.Temperature << std::endl;
            }
            else {
                tcflush(serialPort, TCIFLUSH);
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    return 0;
}
