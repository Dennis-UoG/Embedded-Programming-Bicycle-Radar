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

#include "bicycle_radar/tof_driver.h"

/// @class ToFSensor
/// @brief Handles ToF sensor operations such as initialization, data processing, serialization, and event triggering.

/// @brief Constructs the ToFSensor object.
/// @param port The name of the serial port used to communicate with the ToF sensor.
/// @param trigger Pointer to an EventTrigger object to handle event triggering.
ToFSensor::ToFSensor(std::string port, EventTrigger* trigger){
    this->portName = port;
    this->eventTrigger = trigger;

}

/// @brief Writes sensor data to a CSV file.
/// @param filename The name of the CSV file to write the data to.
/// @param data A vector of StructLidar containing the sensor data.
/// @details The function formats sensor readings into a CSV file with a header: `Temperature,Dist,Strength`.
void ToFSensor::writeCSV(const std::string& filename, const std::vector<ToFSensor::StructLidar>& data) {
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

/// @brief Serializes StructLidar data into a string format.
/// @param data The StructLidar object to serialize.
/// @return A string containing serialized sensor data in the format "Temperature Dist Strength".
std::string ToFSensor::serialize(const StructLidar &data) {
    std::ostringstream oss;
    oss << data.Temperature << " " << data.Dist << " " << data.Strength;
    return oss.str();
}

/// @brief Deserializes a string into a StructLidar object.
/// @param data A string containing serialized sensor data in the format "Temperature Dist Strength".
/// @return A StructLidar object populated with the deserialized data.
ToFSensor::StructLidar ToFSensor::deserialize(const std::string& data) {
    ToFSensor::StructLidar imu;
    std::istringstream iss(data);
    iss >> imu.Temperature >> imu.Dist >> imu.Strength;
    return imu;
}

/// @brief Initializes the ToF sensor and configures its serial port for communication.
/// @return 0 if initialization is successful, 1 on error.
int ToFSensor::Init(){
    serialPort = open(this->portName.c_str(), O_RDWR | O_NOCTTY);
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
        return 1;
    }
    return 0;
}

/// @brief Main execution loop for the ToF sensor.
/// @return 0 if the loop runs successfully, error code otherwise.
/// @details The function reads data continuously from the ToF sensor via the serial port, parses it, and triggers events based on the data.
/// It also stores data in a CSV file when not in debug mode.
int ToFSensor::Run() {
    unsigned int mask  = 0;
#ifndef DEBUG
    std::vector<StructLidar> csv_output;
#endif
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
#ifndef DEBUG
//                std::cout << "Distance: " << temp_structLidar.Dist << std::endl;
#endif
                if (strength > 60) {
                    eventTrigger->triggerEvent(2, serialize(temp_structLidar));
                }
            }
            else {
                tcflush(serialPort, TCIFLUSH);
            }
        }
    }
#ifndef DEBUG
     writeCSV("tof_data.csv", csv_output);
#endif
    return 0;
}
