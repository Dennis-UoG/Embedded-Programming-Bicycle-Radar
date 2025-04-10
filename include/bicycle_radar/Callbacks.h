
#ifndef CALLBACKS_H
#define CALLBACKS_H
#include <iomanip>
#include <string>

#include "CallbackInterface.h"
#include "led_driver.h"
#include "imu_driver.h"
#include "tof_driver.h"
// #include "camera_driver.h"

class CollisionCallbacks: public CallbackInterface {
    public:
    CollisionCallbacks();
    std::string get_current_timestamp();
    void onEvent(int eventId, std::string eventData) override;

};

class RadarCallbacks: public CallbackInterface {
    public:
    RadarCallbacks(LedDriver& led_driver);
    LedDriver& led_driver;
    void onEvent(int eventId, std::string eventData) override;

};

class TestCallbacks: public CallbackInterface {
    public:
    void onEvent(int eventId, std::string eventData) override;
};



#endif //CALLBACKS_H
