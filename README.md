# 🚴 My Vélo Mate – Real-Time Cycling Monitor

**My Vélo Mate** is a real-time physiological monitoring system built on Raspberry Pi using Qt. It continuously tracks O₂ concentration, CO₂ levels, and respiratory flow during cycling. Featuring multithreaded architecture and event-driven callbacks, it supports live charting, metric calculations (VO₂, VCO₂, RER, VE), and dynamic table updates in an intuitive UI.
![b2c25b8988cba07ec1ac36a61ceb0e3](https://github.com/user-attachments/assets/2457a850-d32a-4dd5-8b57-11033a4bb3f3)

![13248066fea9438e98f577409426173](https://github.com/user-attachments/assets/6e222c5f-bf83-4a59-8609-6934fb53f88c)

---

## 🧰 Hardware Setup

This project integrates **three sensors**, all connected to the Raspberry Pi via I²C interface:

| Sensor        | Model       | Description                                  | Manufacturer             |
|---------------|-------------|----------------------------------------------|--------------------------|
| O₂ Sensor     | **4MZ-HH**  | Measures oxygen concentration (%)         | 河南华深科智能科技有限公司  |
| CO₂ Sensor    | **Sensirion SCD4x** | Measures CO₂ concentration (%)| [Sensirion SCD4x Datasheet](https://www.sensirion.com/en/environmental-sensors/gas-sensors/carbon-dioxide-sensors-co2/scd4x/) |
| Flow Sensor   | **Sensirion SFM3300** | Measures respiratory gas flow in L/min     | [Sensirion SFM3300 Datasheet](https://www.sensirion.com/en/flow-sensors/flow-sensors-for-gas/sfm3300/)

> All sensors communicate over the Raspberry Pi’s I²C bus.

![5b8fe5eb91e8d202a55427b5660284c](https://github.com/user-attachments/assets/b1719a74-0406-4e49-b141-fbcf20c1b90c)

![37e830aed028ca7777fd10ad6dc38e7](https://github.com/user-attachments/assets/47ac5727-3773-4751-96b7-ded011ab91fc)

![cdd0b0c735c6dd39572c9b4d17a96bb](https://github.com/user-attachments/assets/c604281f-e51d-450f-a17e-a634e1ebed66)

> 🛠️ You’ll also need a **breathing mask with a one-way valve** to ensure proper gas flow measurement.  
> Since no suitable commercial product was available, **we designed and DIYed a custom mask** for this project.

> ![Uploading 19b74c0cf433edaed9b7a299f5d20c8.png…]()

---

## 🚀 How to Reproduce This Project

You can easily run the full system by following these steps:

### 1. 🔌 Connect the sensors to your Raspberry Pi

We will provide a visual pinout or wiring diagram (见下图/附件)，确保以下连接完成：

- **SCD4x / SFM3300 / O₂ sensor** all share the I²C SDA/SCL pins (e.g., GPIO 2/3)
- Power via 3.3V or 5V depending on sensor spec
- Check datasheets for exact voltage compatibility

### 2. 📦 Download the Source Code Package

We provide a `.zip` file which contains all the Qt source files. Download and unzip the package.

> The release `.zip` is available in the [Releases page](https://github.com/你的仓库地址/releases).

### 3. 🧱 Open in Qt Creator

1. Launch Qt Creator on your Raspberry Pi
2. Click **File → Open File or Project**
3. Select the `exp3.pro` file inside the extracted folder

### 4. ⚙️ Build and Run

- Make sure your Raspberry Pi has `wiringPi` and I²C enabled (`sudo raspi-config`)
- Click **Build** then **Run** in Qt
- The application window will open and start collecting real-time data

> If I²C devices are not detected, try `i2cdetect -y 1` to verify connections.

---

*(后续我可以帮你补充：运行截图、演示视频链接、I²C 连接示意图、附加脚本等)*


