#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <iostream>
#include <unordered_map>
#include <gpiod.h>
#include <map>
#include <thread>
#include <chrono>
#include <vector>

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
#include <gpiod.h>

#include <nlohmann/json.hpp>
//#include <yaml.h>

using json = nlohmann::json;

class LedDriver {
  public:
    bool running = true;
    LedDriver(int chipnumber, json *data, std::vector<int> *gpioPins);
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
