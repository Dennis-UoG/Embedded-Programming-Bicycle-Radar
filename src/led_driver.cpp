#include <iostream>
#include <thread>
#include <chrono>
#include <yaml-cpp/yaml.h>

#include "bicycle_radar/led_driver.h"


LedDriver::LedDriver(std::string chipname, std::string filePath, std::vector<int> gpioPins) {
    this->gpioPins = gpioPins;
    this->chipname = chipname;
    YAML::Node data = YAML::LoadFile(filePath);

    for (const auto& color : {"red", "yellow", "green"}) {
        if (data[color]) {
            float distance = data[color]["distance"].as<float>();
            int freq = data[color]["freq"].as<int>();
            std::pair<float, int> dist_freq = {distance, freq};
            this->parameters[color] = dist_freq;
        } else {
            std::cout << color << " not found in YAML file.\n";
        }
    }
    for (const auto& param : this->parameters) {
        this->dist_colour_freq[param.second.first] = std::make_pair(param.first, param.second.second);
    }
}

int LedDriver::Init() {
    gpiod_chip *chip = gpiod_chip_open(this->chipname.c_str());
    if (!chip) {
        std::cerr << "Failed to open GPIO chip: " << CHIP_NAME << std::endl;
        return 1;
    }

    for (int pin: this->gpioPins) {
        gpiod_line *line = gpiod_chip_get_line(chip, pin);
        if (!line) {
            std::cerr << "Failed to get GPIO line " << pin << std::endl;
        }
        if (gpiod_line_request_output(line, "gpio_test", 0) < 0) {
            std::cerr << "Failed to request GPIO line for output" << std::endl;
            gpiod_line_release(line);
        }
        std::string colour = this->pins_colour[pin];
        this->colour_gpio[colour] = line;
    }

    return 0;
}

int LedDriver::AdjustColourFrequency(float distance) {
    for (const auto& param : this->dist_colour_freq) {
        if (distance <= param.first) {
            this->current_colour = param.second.first;
            this->current_freq = param.second.second;
            break;
        }
        this->current_colour = param.second.first;
        this->current_freq = param.second.second;
    }
    return 0;
}

int LedDriver::FlashLED() {
    while (running) {
        if (!this->current_colour.empty()) {
            gpiod_line *line = this->colour_gpio[this->current_colour];
            long sleeptime = 1 / this->current_freq;
            gpiod_line_set_value(line, 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleeptime/2));
            gpiod_line_set_value(line, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleeptime/2));
        }
    }

    for (const auto& param : this->colour_gpio) {
        gpiod_line_release(param.second);
    }
    gpiod_chip_close(gpiod_chip_open(this->chipname.c_str()));
    return 0;
}