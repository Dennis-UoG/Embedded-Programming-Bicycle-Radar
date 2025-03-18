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

//#include "imu_driver.h"
#include "camera_driver.h"

bool camera_running = true;

void OnCollision()
{
    std::cout << "Collision detected" << std::endl;

}



int main() 
{
    //std::thread imu_thread(IMUThread, OnCollision);
    //imu_thread.detach();

    //std::thread camera_thread(CameraMainThread);
    //camera_thread.detach();

    CameraMainThread();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    camera_running = false;
    return 0;
}