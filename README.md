# 🚴 PiBike Radar – Raspberry Pi-Based Smart Rearview System

> **Afraid of what’s behind you? Let your bike keep watch!**

Ever feel the urge to constantly glance behind you while cycling? Wondering if that whoosh was just a car tailing too close? Worry no more — **PiBike Radar** has your back (literally)! With this smart rearview radar system, your regular bicycle transforms into a futuristic riding companion.

Here's how the PiBike Radar looks when mounted on a bike seat post:

![Radar System Mounted on Seat Post](https://github.com/Dennis-UoG/Embedded-Programming-Bicycle-Radar/blob/Discussion/image2.png?raw=true)



## 🌟 Features That Make You Say “Whoa!”

- **Real-Time Rear Radar**  
  Using a ToF (Time-of-Flight) sensor, PiBike constantly tracks the distance of vehicles or objects approaching from behind.

- **Color-Coded LED Alerts**  
  Green means go, yellow means caution, and red means 🚨! LED flashes change based on how close the object is. Even better? You can set those thresholds from your **phone’s browser**.

- **Crash Detection with Auto Snapshot**  
  IMU (gyroscope + accelerometer) detects crashes or falls. When it senses impact, the system instantly snaps a photo from the rear camera.

- **Web-Based Control Panel**  
  No command-line black magic needed. Just open your phone and go to  
  **`G16-raspberrypi.local:8080`**  
  — change LED distance thresholds, check saved images, and even download them.



# Hardware



# Building the Code & Developer Notes

## Before Start

### Clone

```bash
git clone https://github.com/Dennis-UoG/Embedded-Programming-Bicycle-Radar.git
```

### Dependencies

- libcamera-dev==0.4.0
- libopencv-dev==4.6.0
- nlohmann-json3-dev==3.11.2
- libgpiod-dev==1.6.3
- libnghttp2-14==1.52.0
- cmake==3.25.1

### Udev Rules Reloads

First, copy the rules file ```imu_tof_UART.rules``` to ```/etc/udev/rules.d/```

```bash
cd Embedded-Programming-Bicycle-Radar/
sudo cp imu_tof_UART.rules /etc/udev/rules.d/
```

Then, reload and trigger.

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```



## Build and Run the Code

The following has been tested on Debian GNU/Linux 12 (bookworm). Hopefully it will work on other systems as well, or will at least be not too difficult to fix.

### Build

```bash
cd Embedded-Programming-Bicycle-Radar/
mkdir build
mkdir frame
cd build
cmake ..
make -j8
```

### Run

Please move the binary file from ```build``` to the folder ```Embedded-Programming-Bicycle-Radar```
```bash
mv bicycle_radar ..
cd ..
```
Then, run the program
```bash
./bicycle_radar
```



## Notes

### Code structure

```
Embedded-Programming-Bicycle-Radar/
├── frame/                     # where the crash images saved
├── include/
│   └──bicycle_rader/
│     ├── CallbackInterface.h  # father class of callback and event trigger
│     ├── Callbacks.h          
│     ├── camera_driver.h      
│     ├── httplib.h            
│     ├── imu_driver.h         
│     ├── led_driver.h         
│     ├── libcam2opencv.h      
│     └── tof_driver.h         
├── parameters/
│   └── led_freq_dist.json     # parameters of LED lights
├── src/
│   ├── Callbacks.cpp          # define callbacks
│   ├── camera_driver.cpp      # send callback to the camera
│   ├── imu_driver.cpp         # decode data from IMU
│   ├── led_driver.cpp         # using gpio to flash the LED lights
│   ├── libcam2opencv.cpp      # camera management
│   ├── main_loop.cpp          # enterence of the program
│   └── ToF_driver.cpp         # decode data from ToF sensor
├── www/                       # frontend
├── CMakeLists.txt             # compilation rules - includes, etc
├── LICENSE
└── README.md

```

### License

This code is licensed under MIT License. Copyright (c) 2025 Mengyang Dong.

You may obtain a copy of the License at: https://opensource.org/license/mit

Enjoy!



## 🖼️ Web Interface Preview

Visit **[G16-raspberrypi.local:8080](G16-raspberrypi.local:8080)**  
from your mobile browser to (connect to a same network as the device):

- ✅ Set LED distances & flash frequencies  
- 📷 View photos automatically taken after a crash  
- ⬇️ Download snapshots  

### 📋 LED Configuration Page

![LED Settings UI](https://github.com/Dennis-UoG/Embedded-Programming-Bicycle-Radar/blob/Discussion/9b778e2f8898382c3459d2d73b912c1.png?raw=true)

### 🖼️ Crash Image Viewer

![Crash Snapshot Viewer](https://github.com/Dennis-UoG/Embedded-Programming-Bicycle-Radar/blob/Discussion/8d53b3bb731af6d959f3bfb351efcc5.png?raw=true)





