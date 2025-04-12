#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <iostream>
#include <unordered_map>
#include <gpiod.h>
#include <map>
#include <thread>
#include <chrono>
#include <yaml.h>



class LedDriver {
  public:
    bool running = true;
    LedDriver(int chipnumber, std::string filePath, std::vector<int> *gpioPins);
    std::vector<int> *gpioPins;
    int chipnumber;
    struct gpiod_chip *chip;
    std::unordered_map<std::string, std::pair<float, int>> parameters;
    std::unordered_map<int, std::string> pins_colour = {{20, "green"}, {21, "yellow"}, {26, "red"} };
    std::map<float, std::pair<std::string, int>> dist_colour_freq;
    std::unordered_map<std::string, struct gpiod_line*> colour_gpio;
    std::string current_colour = "green";
    int current_freq = 1;
    int Init();
    int FlashLED();
    int AdjustColourFrequency(float distance);

    ~LedDriver();

};
#endif //LED_DRIVER_H
