#include "bicycle_radar/led_driver.h"

/// @file led_driver.cpp
/// @brief Implements the driver for controlling LEDs with GPIO pins.
/// @details Provides functionality for initializing GPIO pins, adjusting LED color and frequency
/// based on distance, and flashing LEDs dynamically based on the set parameters.

/// @fn LedDriver::LedDriver(int chipnumber, json* data, std::vector<int>* gpioPins)
/// @brief Constructs an LedDriver object with specific GPIO configurations.
/// @param chipnumber The GPIO chip number controlling the LED pins.
/// @param data A pointer to a JSON object containing distance-frequency mappings for LED colors.
/// @param gpioPins A vector of GPIO pin numbers used to control the LEDs.
/// @details Initializes the color-distance-frequency mappings using the provided JSON data and sets up internal parameters.
LedDriver::LedDriver(int chipnumber, json *data, std::vector<int> *gpioPins) {
    this->gpioPins = gpioPins;
    this->chipnumber = chipnumber;
    for (const auto& color : {"red", "yellow", "green"}) {
        if (data->contains(color)) {
            float distance = (*data)[color]["distance"].get<float>();
            int freq = (*data)[color]["freq"].get<int>();
            std::pair<float, int> dist_freq = {distance, freq};
            this->parameters[color] = dist_freq;
        } else {
            std::cout << color << " not found in JSON file.\n";
        }
    }
    for (const auto& param : this->parameters) {
        this->dist_colour_freq[param.second.first] = std::make_pair(param.first, param.second.second);
    }
}

/// @fn int LedDriver::Init()
/// @brief Initializes the GPIO pins for LED control.
/// @return 0 if initialization is successful, 1 on failure.
/// @details Opens the GPIO chip and requests the specified GPIO lines for output. Maps colors to corresponding pins,
/// and sets initial GPIO state to high.
int LedDriver::Init() {
    struct gpiod_chip *ichip = gpiod_chip_open_by_number(this->chipnumber);
    this->chip = ichip;
    if (!this->chip) {
        std::cerr << "Failed to open GPIO chip: " << this->chipnumber << std::endl;
        return 1;
    }

    for (int pin: *this->gpioPins) {
        struct gpiod_line *line = gpiod_chip_get_line(chip, pin);
        if (!line) {
            std::cerr << "Failed to get GPIO line " << pin << std::endl;
        }
        if (gpiod_line_request_output(line, "gpio_test", 0) < 0) {
            std::cerr << "Failed to request GPIO line for output" << std::endl;
            gpiod_line_release(line);
        }
        gpiod_line_set_value(line, 1);
        std::string colour = this->pins_colour[pin];
        this->colour_gpio[colour] = line;
    }

    return 0;
}

/// @fn int LedDriver::AdjustColourFrequency(float distance)
/// @brief Adjusts the LED's color and flashing frequency based on the given distance.
/// @param distance The distance value to determine the LED color and frequency.
/// @return 0 if the adjustment is successful.
/// @details Iterates through the color-distance-frequency mapping to determine the appropriate LED behavior
/// based on the proximity value.
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

/// @fn int LedDriver::FlashLED()
/// @brief Executes the flashing sequence of the LED based on the current color and frequency.
/// @return 0 when the execution ends (likely when the driver stops running).
/// @details Continuously flashes the currently active LED color with an interval calculated from the set frequency.
/// Ensures proper on/off toggling of GPIO pins.
int LedDriver::FlashLED() {
    while (running) {
        if (!this->current_colour.empty()) {
            gpiod_line *line = this->colour_gpio[this->current_colour];
            long sleeptime = 1000 / this->current_freq;
            gpiod_line_set_value(line, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleeptime/2));
            gpiod_line_set_value(line, 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleeptime/2));
        }
    }
    return 0;
}

/// @fn LedDriver::~LedDriver()
/// @brief Destroys the LedDriver object.
/// @details Releases all allocated GPIO resources, terminating the LED control safely.
LedDriver::~LedDriver() {
    for (const auto& param : this->colour_gpio) {
        gpiod_line_release(param.second);
    }
    gpiod_chip_close(this->chip);
}