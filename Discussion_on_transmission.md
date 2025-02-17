# Raspberry Pi Data Collection and Viewing in Mobile Browser

This guide introduces how to collect real-time data on a Raspberry Pi and view it in a mobile browser.

## Method 1: Using Flask to Set Up a Web Server (Recommended)

### Install Flask
```bash
pip install flask
```

### Write Flask Server Code
```python
from flask import Flask, jsonify
import random  # Simulating data
import time

app = Flask(__name__)

@app.route('/')
def home():
    return "Raspberry Pi Data Server Running"

@app.route('/data')
def get_data():
    data = {
        "temperature": round(random.uniform(20, 30), 2),
        "humidity": round(random.uniform(40, 60), 2),
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
    }
    return jsonify(data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
```

### Run Flask Server
```bash
python server.py
```

### Access via Mobile Browser
Run `ifconfig` or `ip a` on the Raspberry Pi to find its IP address, e.g., `192.168.1.100`, then enter the following in the mobile browser:
```
http://192.168.1.100:5000
```
Accessing `http://192.168.1.100:5000/data` retrieves real-time data in JSON format.

---

## Method 2: Using WebSocket for Real-Time Data Push (For Low Latency)

### Install Flask-SocketIO
```bash
pip install flask-socketio
```

### Write WebSocket Server Code
```python
from flask import Flask
from flask_socketio import SocketIO
import random
import time
import threading

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")

def send_data():
    while True:
        data = {
            "temperature": round(random.uniform(20, 30), 2),
            "humidity": round(random.uniform(40, 60), 2),
            "timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
        }
        socketio.emit('update', data)
        time.sleep(2)

@app.route('/')
def index():
    return "WebSocket Data Push Service Running"

if __name__ == '__main__':
    thread = threading.Thread(target=send_data)
    thread.start()
    socketio.run(app, host='0.0.0.0', port=5000)
```

### Receive Data in an HTML Page
Create `index.html` and use JavaScript to connect to WebSocket:
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Real-Time Data</title>
</head>
<body>
    <h1>Real-Time Data</h1>
    <p id="data"></p>
    <script>
        var socket = new WebSocket("ws://192.168.1.100:5000/socket.io/");
        socket.onmessage = function(event) {
            document.getElementById("data").innerText = event.data;
        };
    </script>
</body>
</html>
```

### Access via Mobile Browser
```
http://192.168.1.100:5000
```

---

## Method 3: Using MQTT for Data Transmission

If multiple clients need to receive data, MQTT can be used.

### Install MQTT Server
```bash
sudo apt install mosquitto mosquitto-clients
```

### Run MQTT Server
```bash
mosquitto -d
```

### Send Data Using Python
```python
import paho.mqtt.client as mqtt
import random
import time

client = mqtt.Client()
client.connect("192.168.1.100", 1883, 60)

while True:
    data = f"Temperature: {random.uniform(20, 30):.2f}, Humidity: {random.uniform(40, 60):.2f}"
    client.publish("sensor/data", data)
    print("Published:", data)
    time.sleep(2)
```

### Subscribe to Data on Mobile
Use an MQTT client on mobile (e.g., MQTT Dash) to subscribe to the `sensor/data` topic.

---

## Choosing the Right Method
- **Simple data viewing** → Flask Web Server (Method 1)
- **Low latency & auto-update** → WebSocket (Method 2)
- **Multiple device subscription** → MQTT (Method 3)

