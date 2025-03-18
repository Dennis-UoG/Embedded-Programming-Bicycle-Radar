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



void OnCollision()
{
    std::cout << "Collision detected" << std::endl;

}



int main() 
{
    //std::thread imu_thread(IMUThread, OnCollision);
    //imu_thread.detach();

    std::thread camera_thread(CameraMainThread);
    camera_thread.detach();

    while(true){}
    return 0;
}