#include <iostream>
#include <thread>
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

void OnCollision()
{
    std::cout << "Collision detected!" << std::endl;

}



int main() 
{
    std::thread imu_thread(IMUThread, OnCollision);
    imu_thread.detach();

    return 0;
}