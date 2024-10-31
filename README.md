# Matter-Enabled Patient Monitoring and Alert System for ICU Wards :

## Problem:  
Current ICU patient monitoring systems typically lack wireless connectivity and are meant to only track a patients vitals. This would require nurses or doctors to enter the ICU at regular intervals to check in on the patient, increasing the risk of contamination in a highly sterile environment.
 
## Solution: 
Our system combines vital signs monitoring with several other sensors in order to provide a complete patient monitoring system that works using matter over thread protocol, ensuring secure communication and remote monitoring without needing to enter the ICU repeatedly.  

Additionally, the system has smart sensing and actuation capabilities such as fall detection, smoke/gas alert system, intrusion detection, etc. 

This system is powered by the Arduino Nano Matter board and has extremely low power consumption. The overall system was designed to be low cost as well.
 
## Detailed description of system: 
<img src="https://github.com/user-attachments/assets/e8fa3933-af8d-4053-9d9e-22a3d39bf292" width="70%" height="70%"> 

We have developed a matter-based system that can track and monitor a patient's vitals as well as the room parameters and create alerts if any irregularities are detected. 

### Sensors Utilized:

- Heartbeat Detection Module
- BMP280 Temperature Sensor

These sensors are used to monitor the patient condition and trigger an alert if high temperature or irregular heartbeats are detected. 

- MICS-6814 Gas Detector
  
The air quality sensor will create an alert when any dangerous gases such as CO, NO2, etc. are detected in the room.  

- MPU6050 Accelerometer
  
The system also incorporates an accelerometer which will detect if the patient has fallen and will trigger an alert. 

- HC-SR501 PIR motion detection sensor
  
The motion detection sensor can be placed near the entrance and will trigger an alert if some entry is detected outside of visiting hours.

<img src="https://github.com/user-attachments/assets/216c9e84-3092-4b90-abc2-bbfeb518bd60" width="30%" height="30%">

### Thread Border Router:

We are using an Amazon Echo (Gen 4) as our thread border router. The Alexa app on our smartphone serves as the commissioner for new devices and is also the control center and dashboard for sensor data.  

Additionally, we can utilize Alexa routines in order to setup automatic actions whenever a trigger happens. For example, if the accelerometer detects a fall, then the buzzer alarm will automatically be triggered. 

Alexa voice commands can also be used to check for sensor readings and trigger actuation events.

### Actuation Events:
For actuation events, we are using a buzzer and an RGB LED. 

The LED will blink alternating red and green if any anomalies are detected and the buzzer will also ring. 

If all systems are ok, then the LED will glow green.
 
 
## Why Matter? 
- Matter over thread has lower energy consumption as compared to WiFi, thus enabling longer operation on battery power. 

- Additionally, wifi based systems are dependent on wifi signal strength and speeds, which can vary across hospital systems. 

- Matter over thread provides a system to have localized data using a thread border router, which ensures that alerts are propagated immediately. 

- Thread networks are self healing and will automatically fix any gaps that arise using the mesh network.
  
- Thread networks can support upto several hundred devices within a single network as compared to an IP based WiFi router.
 
## Software: 
We have written some custom functions for sensor control, in addition to using the matter examples provided in the Arduino IDE.
The complete arduino code can be found in the repository above.
