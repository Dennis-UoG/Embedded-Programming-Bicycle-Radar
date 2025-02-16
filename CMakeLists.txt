cmake_minimum_required(VERSION 3.25)
project(bicycle_radar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_library(GPIOD_LIBRARY NAMES gpiod REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(Libcamera REQUIRED libcamera)
include_directories(/usr/include)
add_executable(bicycle_radar
        src/ToF_driver.cpp
        src/led_driver.cpp
        src/imu_driver.cpp
        src/camera_driver.cpp
)
target_link_libraries(bicycle_radar PRIVATE ${GPIOD_LIBRARY})
target_include_directories(bicycle_radar PRIVATE ${Libcamera_INCLUDE_DIRS})
target_link_libraries(bicycle_radar PRIVATE ${Libcamera_LIBRARIES})