#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sstream>
#include <typeinfo>
#include <thread>
#include <vector>
#include <csignal>


struct StructLidar {
    float Temperature;
    float Dist;
    int Strength;
};

void writeCSV(const std::string& filename, const std::vector<StructLidar>& data) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // Write CSV header
    file << "Temperature,Dist,Strength\n";

    // Write data
    for (const auto& entry : data) {
        file << entry.Temperature << "," << entry.Dist << "," << entry.Strength << "\n";
    }

    file.close();
    std::cout << "CSV file written successfully: " << filename << std::endl;
}

bool running = true;

void signalHandler(int signum) {
    std::cout << "\nCaught signal " << signum << ", exiting gracefully...\n";
    running = false;
}

int ToFThread(void (*func)()) {
    std::signal(SIGTERM, signalHandler);
    const char* portName = "/dev/ttyUSB1";
    int serialPort = open(portName, O_RDWR | O_NOCTTY);
    if (serialPort == -1) {
        std::cerr << "Error: Unable to open port " << portName << std::endl;
        return 1;
    }

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
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    // http://www.unixwiz.net/techtips/termios-vmin-vtime.html
    tty.c_cc[VMIN] = 9;
    tty.c_cc[VTIME] = 0;

    result = tcsetattr(serialPort, TCSANOW, &tty);
    if (result < 0)
    {
        perror ("error in tcsetattr");
        return 0;
    }

    unsigned int mask  = 0;
    std::vector<StructLidar> csv_output;

    // Read data from the port
    char buffer[9];
    while (running) {
        int bytesRead = read(serialPort, buffer, sizeof(buffer));
        if (bytesRead == 9) {
            if (buffer[0] == 0x59 && buffer[1] == 0x59) {
                StructLidar temp_structLidar = {};
                float dist = (mask | (uint8_t (buffer[2]) | uint16_t (buffer[3] << 8))) / 100.0;
                int strength = uint8_t (buffer[4]) | uint16_t (buffer[5] << 8);
                float temperature = (mask | (uint8_t (buffer[6]) | uint16_t (buffer[7] << 8))) / 8 - 256;
                temp_structLidar.Temperature = temperature;
                temp_structLidar.Dist = dist;
                temp_structLidar.Strength = strength;
                std::cout << "Distance: " << temp_structLidar.Dist << std::endl;
                // csv_output.push_back(temp_structLidar);
            }
            else {
                tcflush(serialPort, TCIFLUSH);
            }
        }
    }
    // writeCSV("tof_data.csv", csv_output);
    return 0;
}
