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
#include <vector>
#include <csignal>

struct StructImu {
    float acc_X;
    float acc_Y;
    float acc_Z;
};

void writeCSV(const std::string& filename, const std::vector<StructImu>& data) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // Write CSV header
    file << "acc_X,acc_Y,acc_Z\n";

    // Write data
    for (const auto& entry : data) {
        file << entry.acc_X << "," << entry.acc_Y << "," << entry.acc_Z << "\n";
    }

    file.close();
    std::cout << "CSV file written successfully: " << filename << std::endl;
}
bool running = true;

void signalHandler(int signum) {
    std::cout << "\nCaught signal " << signum << ", exiting gracefully...\n";
    running = false;
}

int IMUThread(void (*func)()) {
    std::signal(SIGTERM, signalHandler);
    const char* portName = "/dev/ttyUSB0";
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
    int mask  = 0;
    std::vector<StructImu> csv_output;
    // Read data from the port
    // https://wit-motion.yuque.com/wumwnr/ltst03/wegquy?#%20《WT61协议》
    char buffer[11];
    while (running) {
        int bytesRead = read(serialPort, buffer, sizeof(buffer));
        if (bytesRead == 11) {
            if (buffer[0] == 0x55 && buffer[1] == 0x51) {
                StructImu imu_data = {};
                float x = (mask | (int8_t (buffer[2]) | int16_t (buffer[3] << 8))) / 32768.0 * 16.0 * 9.8;
                float y = (mask | (int8_t (buffer[4]) | int16_t (buffer[5] << 8))) / 32768.0 * 16.0 * 9.8;
                float z = (mask | (int8_t (buffer[6]) | int16_t (buffer[7] << 8))) / 32768.0 * 16.0 * 9.8;
                imu_data.acc_X = x;
                imu_data.acc_Y = y;
                imu_data.acc_Z = z;
                std::cout << "accelerationX: " << imu_data.acc_X << std::endl;
                if(false/*imu_data.acc_X > 10.0 || imu_data.acc_X < -10.0*/) // Threshold for collision detection
                {
                    func();
                }
                // csv_output.push_back(imu_data);
            }
            else {
                tcflush(serialPort, TCIFLUSH);
            }
        }
    }
    // writeCSV("imu_data.csv", csv_output);
    return 0;
}
