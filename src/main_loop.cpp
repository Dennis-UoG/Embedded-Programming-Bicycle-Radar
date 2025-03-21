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

class OnCollisionCallback : public CallbackInterface {
    public:
        void onEvent(int eventId, const std::string& eventData) override {
            std::cout << "Collision detected! Data = " << eventData << std::endl;
        }
};



int main() 
{
    EventTrigger eventTrigger;
    eventTrigger.addCallback(new OnCollisionCallback());


    std::thread imu_thread(IMUThread, eventTrigger);
    imu_thread.detach();

    //std::thread camera_thread(CameraMainThread);
    //camera_thread.detach();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    camera_running = false;
    return 0;
}