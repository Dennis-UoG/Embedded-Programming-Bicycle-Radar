//
// Created by Jiancheng Zhang on 02/02/2025.
//
#include <gpiod.h>
#include <iostream>
#include <thread>
#include <chrono>

#define CHIP_NAME "/dev/gpiochip0"
#define GPIO_PIN 20

int main() {
    gpiod_chip *chip = gpiod_chip_open(CHIP_NAME);
    if (!chip) {
        std::cerr << "Failed to open GPIO chip: " << CHIP_NAME << std::endl;
        return 1;
    }

    gpiod_line *line = gpiod_chip_get_line(chip, GPIO_PIN);
    if (!line) {
        std::cerr << "Failed to get GPIO line " << GPIO_PIN << std::endl;
        gpiod_chip_close(chip);
        return 1;
    }

    if (gpiod_line_request_output(line, "gpio_test", 0) < 0) {
        std::cerr << "Failed to request GPIO line for output" << std::endl;
        gpiod_line_release(line);
        gpiod_chip_close(chip);
        return 1;
    }

    for (int i = 0; i < 10; ++i) {
        gpiod_line_set_value(line, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        gpiod_line_set_value(line, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;
}