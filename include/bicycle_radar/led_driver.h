#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <iostream>
#include <gpiod.h>
#include <map>


class LedDriver {
  public:
    bool running = true;
    LedDriver(std::string chipname, std::string filePath, std::vector<int> gpioPins);
    std::vector<int> gpioPins;
    std::string chipname;
    std::unordered_map<std::string, std::pair<float, int>> parameters;
    std::unordered_map<int, std::string> pins_colour = {{20, "green"}, {21, "yellow"}, {26, "red"} };
    std::map<float, std::pair<std::string, int>> dist_colour_freq;
    std::unordered_map<std::string, gpiod_line> colour_gpio;
    std::string current_colour = "green";
    int current_freq = 1;
    int Init();
    int FlashLED();
    int AdjustColourFrequency(float distance);

};
#endif //LED_DRIVER_H
