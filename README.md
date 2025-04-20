# 🚴 My Vélo Mate – Real-Time Cycling Monitor

Commercial physiological monitoring systems for exercise testing are often **expensive, complex, or inaccessible for student research and DIY sports science**. In particular, **portable real-time VO₂ monitoring** is rare and typically requires proprietary equipment.

🧠 **My Vélo Mate** addresses this gap by providing a **low-cost, Raspberry Pi-based solution** capable of tracking:
- O₂ and CO₂ concentration
- Respiratory flow
- VO₂, VCO₂, RER, VE, and VO₂MAX in real-time

It combines **multithreaded Qt architecture**, **event-driven callbacks**, and **modular sensor integration** to create a fully functional, extensible monitoring system.

<img src="assets/rpi.jpg" width="200" align="right" />

**Key use cases include**:
- Lab-based aerobic capacity testing (VO₂MAX)
- DIY fitness and cycling experiments
- Academic/engineering teaching demonstrations

This project brings powerful physiological monitoring capabilities to **students, makers, and sports engineers**, with a fully open-source implementation.

![b2c25b8988cba07ec1ac36a61ceb0e3](https://github.com/user-attachments/assets/2457a850-d32a-4dd5-8b57-11033a4bb3f3)

![13248066fea9438e98f577409426173](https://github.com/user-attachments/assets/6e222c5f-bf83-4a59-8609-6934fb53f88c)

---

## 🧰 Hardware Setup

This project integrates **three sensors**, all connected to the Raspberry Pi via I²C interface:

| Sensor        | Model       | Description                                  | Manufacturer             |
|---------------|-------------|----------------------------------------------|--------------------------|
| O₂ Sensor     | **4MZ-HH**  | Measures oxygen concentration (%)         | 河南华深科智能科技有限公司  |
| CO₂ Sensor    | **Sensirion SCD4x** | Measures CO₂ concentration (%)| [[Sensirion SCD4x Datasheet](https://developer.sensirion.com/product-support/scd4x-co2-sensor)] |
| Flow Sensor   | **Sensirion SFM3300** | Measures respiratory gas flow in L/min     | [Sensirion SFM3300 Datasheet](https://sensirion.com/products/catalog/SFM3300-D)

> All sensors communicate over the Raspberry Pi’s I²C bus.

![5b8fe5eb91e8d202a55427b5660284c](https://github.com/user-attachments/assets/b1719a74-0406-4e49-b141-fbcf20c1b90c)

![37e830aed028ca7777fd10ad6dc38e7](https://github.com/user-attachments/assets/47ac5727-3773-4751-96b7-ded011ab91fc)

![cdd0b0c735c6dd39572c9b4d17a96bb](https://github.com/user-attachments/assets/c604281f-e51d-450f-a17e-a634e1ebed66)

> 🛠️ You’ll also need a **breathing mask with a one-way valve** to ensure proper gas flow measurement.  
> Since no suitable commercial product was available, **we designed and DIYed a custom mask** for this project.

![19b74c0cf433edaed9b7a299f5d20c8](https://github.com/user-attachments/assets/f57d0050-803b-434a-b251-3356e0b03ef4)


---

## 🚀 How to Reproduce This Project

You can easily run the full system by following these steps:

### 1. 🔌 Connect the sensors to your Raspberry Pi

![f545e347b242b5af3ac7fd1eb3bed17](https://github.com/user-attachments/assets/bc9e7403-7c98-409d-ae2a-43eb425400ee)


![b2b865c5b0cb84f2f6ba9c95fd3821a](https://github.com/user-attachments/assets/105d34fb-e505-444b-90ad-63f752851888)


![7024ef1db43f815bc1572d97ff75ab7](https://github.com/user-attachments/assets/0a5f9689-8ec7-4097-8189-d29543ad513f)


![image](https://github.com/user-attachments/assets/37526979-d9c2-42e2-b298-4c2354369a79)


![image](https://github.com/user-attachments/assets/83194ab2-2e28-49d3-b6ee-054669ce7fed)


![image](https://github.com/user-attachments/assets/a81637e7-9d2b-40fc-8b31-a9b5824e2108)


![image](https://github.com/user-attachments/assets/bf269b42-eb53-4ea3-88a6-65cb734ee47a)



### 2. 📦 Download the Source Code Package

We provide a `.zip` file which contains all the Qt source files. Download and unzip the package.

> The release `.zip` is available in the (https://github.com/WeiyuanWang0821/Real-Time-Cycling-Monitor/releases/tag/V1.0).

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

## 🖥️ User Interface & Usage Guide

Below is a screenshot of the real-time Qt interface running on Raspberry Pi:

![160d7a60719dbdf3553f368d3cecc29](https://github.com/user-attachments/assets/8d671009-a8da-4e40-9ab3-937bc7b98b62)


### 👣 How to Use

1. **Set basic information**: Input your weight, age, and sex, then click `SET`.
2. Wait for a few seconds for the system to initialize and start sampling.
3. **Select curves** on the left (O₂, CO₂, flow, VO₂...) to visualize in real time.
4. The lower table will dynamically update calculated values, including **VO₂**, **VCO₂**, **VE**, **RER**, and **VO₂MAX**.

This marks the start of your **VO₂MAX monitoring** during real-time exercise testing.

---

## 💸 Sensor Cost & Limitations

- Excluding the Raspberry Pi, the total hardware cost (3 sensors) is **around £200**.
- Due to budget constraints, the **CO₂ sensor** used in this project is an **electrochemical model**, which:
  - Has low precision
  - Responds slowly (theoretical delay ~60 seconds)
  - Is **not suitable** for real-time exercise CO₂ tracking

However, **CO₂ is not required for VO₂MAX calculation**, only for **VCO₂-related metrics** like RER.

⚠️ In future versions, we plan to upgrade to a better CO₂ sensor.  
Feel free to try replacing it if you're interested in exploring improved accuracy.

---

📹 For a detailed video walkthrough, see:  
▶️ [https://www.tiktok.com/@lidashan/video/7495379013582556438](https://www.tiktok.com/@lidashan/video/7495379013582556438)

