# 🤖 Kobra-Guard AI: Spaghetti Detective

An ESP32-powered AI monitoring system for the **Anycubic Kobra 3**. Using a **HuskyLens 2**, this system detects print failures (spaghetti) in real-time and alerts you via **Discord**, **Google Home**, and a **Live Web Dashboard**.

## 🚀 Features
- **AI Failure Detection:** Uses HuskyLens 2 Object Classification to distinguish between a "Good Print" and "Spaghetti."
- **Discord Integration:** Sends rich embed notifications with confidence scores to your Discord server.
- **Smart Home Sync:** Uses SinricPro to dim/brighten Google Home lights based on print status.
- **Live Dashboard:** A mobile-friendly web interface with a real-time confidence bar.
- **Remote Emergency Stop:** Stop the printer directly from your phone if the AI catches a failure.
- **Sensitivity Slider:** Adjustable "Panic Level" via the web UI.

## 🛠️ Hardware Needed
- **ESP32** (NodeMCU or similar)
- **DFRobot HuskyLens 2**
- **Anycubic Kobra 3** (Connected to your local network)
- **Gravity I2C Cable**

## 📦 Libraries Required
Install these via the Arduino Library Manager:
1. `DFRobot_HuskylensV2`
2. `WiFiManager` (by tzapu)
3. `SinricPro`
4. `ArduinoJson`

## ⚙️ Setup
1. **Flash the Code:** Open `3dprintedbot.ino` in Arduino IDE. 
2. **Partition Scheme:** Set `Tools > Partition Scheme` to **"Huge APP"**.
3. **Configure:** Enter your SinricPro keys and Kobra 3 IP address at the top of the sketch.
4. **First Run:** Connect to the `PrintMind_Setup` WiFi hotspot on your phone to give the ESP32 your home WiFi password.
5. **Train the AI:** - **ID 1:** Train on a successful print or empty bed.
   - **ID 2:** Train on a pile of scrap filament (spaghetti).

## 🖥️ Web Dashboard
Access the dashboard by navigating to the IP address printed in the Serial Monitor (115200 baud).

---
*Created with ❤️ for the 3D Printing Community.*

## 🔌 Wiring Diagram

To connect the **HuskyLens 2** to your **ESP32**, use the Gravity I2C cable provided with the camera. Ensure the HuskyLens is set to **I2C Mode** in its settings menu.

| HuskyLens Wire | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **Red** | **5V / VIN** | Power (HuskyLens 2 needs 5V) |
| **Black** | **GND** | Ground |
| **Blue** | **GPIO 22** | SCL (Clock) |
| **Green** | **GPIO 21** | SDA (Data) |

> **Note:** If the HuskyLens screen flickers, plug a USB-C cable directly into the camera's port for extra power.

---

## 🏎️ Connecting Other Printers

This project is built for the **Anycubic Kobra 3**, but it can work with almost any 3D printer by changing the `kobraIP` and the API paths in the code.

### 1. Identify your Printer's API
Update the `checkPrinterStatus()` and `stopPrinter()` functions using the addresses below:

| System / Printer | Status URL (checkPrinterStatus) | Stop URL (stopPrinter) |
| :--- | :--- | :--- |
| **Anycubic Kobra 3** | `/api/v1/status` | `/api/v1/stop` |
| **OctoPrint** | `/api/job` (Requires API Key) | `/api/job` (POST "cancel") |
| **Klipper (Mainsail)** | `/printer/objects/query?print_stats` | `/printer/print/cancel` |
| **Bambu Lab** | *Uses MQTT (Requires different library)* | *Uses MQTT* |

### 2. How to Update the Code
If you are using **OctoPrint** or **Mainsail**, find this line in the code:
`http.begin("http://" + String(kobraIP) + "/api/v1/status");`

And replace the `/api/v1/status` part with the one from the table above. 

---

## 🛠️ Calibration Tips
For the best results with the "Spaghetti Detective":
1. **Lighting:** Mount an LED strip near your nozzle. If the HuskyLens can't see the plastic clearly, it can't save your print!
2. **Angle:** Mount the camera to the **X-axis** or the **Print Bed** so it always has a clear view of the nozzle tip.
3. **Threshold:** Start with a **Sensitivity** of 50%. If you get false alarms, move the slider down to 30%.