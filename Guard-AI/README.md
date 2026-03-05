# 🤖 Kobra-Guard AI: The Ultimate Spaghetti Detective

[![Hardware: ESP32](https://www.amazon.co.uk/Esp32-Development-Board-Microcontroller-Accessories/dp/B0DK6SZKPH/ref=asc_df_B0DK6SZKPH?mcid=0eb11f1a5f8a30289637932badd75d84&tag=googshopuk-21&linkCode=df0&hvadid=718978587341&hvpos=&hvnetw=g&hvrand=12703419080131930058&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=1006626&hvtargid=pla-2374493277495&hvocijid=12703419080131930058-B0DK6SZKPH-&hvexpln=0&gad_source=1&th=1
[![AI: HuskyLensV2]https://www.dfrobot.com/product-2995.html

**Kobra-Guard AI** is an autonomous failure-detection system for the **Anycubic Kobra 3**. 
Using a HuskyLens 2 AI camera and an ESP32, it monitors your prints for "spaghetti" or bed adhesion failures.
When things go wrong, it alerts you via Discord, dims your smart lights, and gives you the power to stop the print remotely.

---

## ✨ Key Features
- **Real-Time AI Vision:** Detects failures using localized object classification (no cloud needed!).
- **Rich Discord Alerts:** Sends formatted embeds with confidence scores directly to your server.
- **Smart Home Integration:** Syncs with Google Home/Alexa via SinricPro for visual light alerts.
- **Interactive Web Dashboard:** Live confidence tracking, sensitivity control, and manual overrides.
- **Remote Killswitch:** Trigger an emergency stop to save your printer and filament.
- **Zero-Code Wi-Fi Setup:** Includes a Captive Portal (WiFiManager) for easy network connection.
---
## 🔌 Hardware Setup
Connect the HuskyLens v2 to your ESP32 via the I2C port.

| HuskyLens Wire | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **Red** | **5V / VIN** | Power (Steady 5V Required) |
| **Black** | **GND** | Ground |
| **Blue** | **GPIO 22** | SCL (Clock) |
| **Green** | **GPIO 21** | SDA (Data) |

### 3D Printer Compatibility
While optimized for the **Kobra 3**, this can be adapted for any system with a web API:
* **Anycubic Kobra 3:** `/api/v1/status`
* **OctoPrint:** `/api/job`
* **Klipper/Mainsail:** `/printer/objects/query`
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
## 👁️ AI Training Guide
1. **Select Mode:** Set HuskyLens to `Object Classification`.
2. **Train ID 1 (The Good):** Point at a successful print or clean bed. Hold the "Learn" button.
3. **Train ID 2 (The Bad):** Point at a "spaghetti" mess or failed filament. Hold the "Learn" button.
4. **Logic:** The ESP32 tracks the presence of ID 2. If the "Confidence" drops below your set threshold, the alarm triggers.
---
## 🛠️ Software Installation

1. **Library Requirements:** Install `DFRobot_HuskylensV2`, `WiFiManager`, `SinricPro`, and `ArduinoJson` via the Library Manager.
2. **Partition Scheme:** In Arduino IDE, go to `Tools > Partition Scheme` and select **"Huge APP (3MB No OTA)"**.
3. **Flash:** Upload the code and open the Serial Monitor (115200 baud) to find your Dashboard IP.

---

## 📱 Web Dashboard Overview

| Feature | Description |
| :--- | :--- |
| **Confidence Bar** | Real-time AI certainty level (100% = Healthy). |
| **Sensitivity Slider** | Adjust the "Panic Level" (How fast it triggers the alarm). |
| **Reset Confidence** | Manually clear false alarms and restore 100% status. |
| **Stop Print** | Immediate remote emergency shutdown of the printer. |
---
## 🔍 Troubleshooting
- **Site can't be reached?** Ensure your phone is on the same 2.4GHz Wi-Fi and use `http://` (not https).
- **HuskyLens not found?** Check that the Protocol is set to `I2C` in the HuskyLens General Settings.
- **Sketch too big?** You **must** use the "Huge APP" partition scheme to fit the libraries.
---
## 🛠️ Calibration Tips
For the best results with the "Spaghetti Detective":
1. **Lighting:** Mount an LED strip near your nozzle. If the HuskyLens can't see the plastic clearly, it can't save your print!
2. **Angle:** Mount the camera to the **X-axis** or the **Print Bed** so it always has a clear view of the nozzle tip.
3. **Threshold:** Start with a **Sensitivity** of 50%. If you get false alarms, move the slider down to 30%.
---
## 📜 License
This project is licensed under the MIT License - see the `LICENSE` file for details.

*Project created by [ginges3Dprints]*