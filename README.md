# 🚴 PiBike Radar – Raspberry Pi-Based Smart Rearview System

> **Afraid of what’s behind you? Let your bike keep watch!**

Ever feel the urge to constantly glance behind you while cycling? Wondering if that whoosh was just the wind or a car tailing too close? Worry no more — **PiBike Radar** has your back (literally)! With this smart rearview radar system, your regular bicycle transforms into a futuristic riding companion.

Here's how the PiBike Radar looks when mounted on a bike seat post:

![Radar System Mounted on Seat Post](https://github.com/Dennis-UoG/Embedded-Programming-Bicycle-Radar/blob/Discussion/image2.png?raw=true)

---

## 🌟 Features That Make You Say “Whoa!”

- **Real-Time Rear Radar**  
  Using a ToF (Time-of-Flight) sensor, PiBike constantly tracks the distance of vehicles or objects approaching from behind.

- **Color-Coded LED Alerts**  
  Green means go, yellow means caution, and red means 🚨! LED flashes change based on how close the object is. Even better? You can set those thresholds in real-time from your **phone’s browser**.

- **Crash Detection with Auto Snapshot**  
  IMU (gyroscope + accelerometer) detects crashes or falls. When it senses impact, the system instantly snaps a photo from the rear camera and uploads it to the web interface.

- **Web-Based Control Panel**  
  No command-line black magic needed. Just open your phone and go to  
  **`G16-raspberrypi.local:8080`**  
  — change LED distance thresholds, check saved images, and even download them.

---

## 👨‍💻 Tech Stack & Developer Notes

### ⚙️ Core Technologies

- C++17
- `libcamera` + OpenCV for capturing images
- `cpp-httplib` for serving a lightweight HTTP server
- `nlohmann/json` for easy parameter storage
- IMU sensor (via serial port)
- ToF Lidar (UART serial)
- GPIO LED control (via `libgpiod`)

---

### 📦 Dependencies (Tested on Raspberry Pi OS)

```bash
sudo apt update
sudo apt install -y \
  libgpiod-dev \
  libopencv-dev \
  libcamera-dev \
  cmake \
  g++ \
  git
```
---

### 🖼️ Web Interface Preview

Visit **[G16-raspberrypi.local:8080](G16-raspberrypi.local:8080)**  
from your mobile browser to:

- ✅ Set LED distances & flash frequencies  
- 📷 View photos automatically taken after a crash  
- ⬇️ Download snapshots  

### 📋 LED Configuration Page

![LED Settings UI](https://github.com/Dennis-UoG/Embedded-Programming-Bicycle-Radar/blob/Discussion/9b778e2f8898382c3459d2d73b912c1.png?raw=true)

### 🖼️ Crash Image Viewer

![Crash Snapshot Viewer](https://github.com/Dennis-UoG/Embedded-Programming-Bicycle-Radar/blob/Discussion/8d53b3bb731af6d959f3bfb351efcc5.png?raw=true)

---

### ⭐ Like this project?

Give it a **star** 🌟 or submit a **pull request** to help us make cycling even safer!

