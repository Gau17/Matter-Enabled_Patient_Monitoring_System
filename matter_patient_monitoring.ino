#include <Matter.h>
#include <Wire.h>

// #include <MatterHumidity.h>
#include <MatterTemperature.h>
#include <MatterPressure.h>
#include <MatterAirQuality.h>
#include <MatterOnOffPluginUnit.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_BMP280.h>


MatterTemperature matter_temp_sensor;
MatterTemperature matter_heartrate_sensor;
MatterPressure matter_pressure_sensor;
MatterOnOffPluginUnit matter_fall_detector;
MatterOnOffPluginUnit matter_PIR_sensor;
MatterAirQuality matter_air_quality_sensor;

Adafruit_MPU6050 mpu;
Adafruit_BMP280 bmp;

#define CO A0
#define NH3 A1
#define NO2 A3
#define HeartRate A7

#define LED_GREEN D2
#define LED_RED D3
#define BUZZER D4
#define PIR D5

void decommissionHandler() 
{
  if (digitalRead(BTN_BUILTIN) == LOW) {
    int startTime = millis();
    while (digitalRead(BTN_BUILTIN) == LOW) {

      int elapsedTime = (millis() - startTime) / 1000.0;

      if (elapsedTime > 10) {
        Serial.printf("Decommissioning!\n");
        for (int i = 0; i < 10; i++) {
          digitalWrite(LEDR, !(digitalRead(LEDR)));
          delay(100);
        }

        if (!Matter.isDeviceCommissioned()) {
          Serial.println("Decommission done!");
          digitalWrite(LEDR, LOW);
        } else {
          Serial.println("Matter device is commissioned-> Starting Decommission process");
          nvm3_eraseAll(nvm3_defaultHandle);  // Decomission command
          digitalWrite(LED_BUILTIN, LOW);
          Serial.println("Decommission done!");
        }
        break;
      }
    }
  }
}


void setup()
{

  pinMode(CO, INPUT);
  pinMode(NH3, INPUT);
  pinMode(NO2, INPUT);
  pinMode(HeartRate, INPUT);
  pinMode(PIR, INPUT);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(BUZZER, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);

  Serial.begin(115200);
  Wire.begin();
  Matter.begin();

  matter_temp_sensor.begin();
  matter_heartrate_sensor.begin();
  matter_fall_detector.begin();
  matter_PIR_sensor.begin();
  matter_air_quality_sensor.begin();

  decommissionHandler();
  delay(200);

  Serial.println("Matter Health Monitoring System");

  if (!Matter.isDeviceCommissioned()) {
    Serial.println("Matter device is not commissioned");
    Serial.println("Commission it to your Matter hub with the manual pairing code or QR code");
    Serial.printf("Manual pairing code: %s\n", Matter.getManualPairingCode().c_str());
    Serial.printf("QR code URL: %s\n", Matter.getOnboardingQRCodeUrl().c_str());
  }
  while (!Matter.isDeviceCommissioned()) {
    delay(200);
  }

  Serial.println("Waiting for Thread network...");
  while (!Matter.isDeviceThreadConnected()) {
    delay(200);
  }
  Serial.println("Connected to Thread network");
  
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 sensor!");
    while (1);
  }
  Serial.println("MPU6050 initialized successfully!");

  if (!bmp.begin(0x76)) { // Ensure 0x76 matches your BMP280 address
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
  Serial.println("BMP280 initialized successfully!");
  Serial.println("Waiting for Matter device discovery...");
  Serial.println("Matter device is now online");
}

void temperatureSensor()
{
  float currentTemp = bmp.readTemperature();

  Serial.println();

  matter_temp_sensor.set_measured_value_celsius(currentTemp); 
  if(currentTemp > 26)
  {
    digitalWrite(BUZZER, HIGH);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
  }
  else{
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }
  Serial.printf("Current  temperature: %.02f C\n", currentTemp);

}

void pressureSensor()
{
  float currentPressure = (bmp.readPressure()/100);
  matter_pressure_sensor.set_measured_value(currentPressure);
  Serial.printf("Current Pressure: %.01f\n", currentPressure);
}

void fallDetection()
{
  sensors_event_t accel, gyro, temp;
  float accXAxis, accYAxis, accZAxis;
  float gyroXAxis, gyroYAxis, gyroZAxis;
  mpu.getEvent(&accel, &gyro, &temp);
  
  accXAxis = accel.acceleration.x;
  accYAxis = accel.acceleration.y;
  accZAxis = accel.acceleration.z;
  if(accZAxis < 3)
  {
    matter_fall_detector.set_onoff((bool)1);
    digitalWrite(BUZZER, HIGH);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
  }
  else
  {
    matter_fall_detector.set_onoff((bool)0);
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }

  Serial.printf("Accelerometer\n X axis: %f, Y axis: %f, Z axis: %f", accXAxis, accYAxis, accZAxis);
}

void airQuality()
{
  int co = (analogRead(CO)*(5/1024));
  int nh3 = (analogRead(NH3)*(5/1024));
  int no2 = (analogRead(NO2)*(5/1024));

  Serial.print("\nCO (a0): ");
  Serial.println(co * (5 / 1024));
  Serial.print("NH3 (a1): ");
  Serial.println(nh3 * (5 / 1024));
  Serial.print("NO2 (a2): ");
  Serial.println(no2 * (5 / 1024));
  
  if ((co > 0) && (co < 50)) {
    matter_air_quality_sensor.set_air_quality(MatterAirQuality::AirQuality_t::GOOD);
    Serial.println("Current air quality: good");
  } else if (co < 100) {
    matter_air_quality_sensor.set_air_quality(MatterAirQuality::AirQuality_t::FAIR);
    Serial.println("Current air quality: fair");
  } else if (co < 200) {
    matter_air_quality_sensor.set_air_quality(MatterAirQuality::AirQuality_t::MODERATE);
    Serial.println("Current air quality: moderate");
  } else if (co < 300) {
    matter_air_quality_sensor.set_air_quality(MatterAirQuality::AirQuality_t::POOR);
    Serial.println("Current air quality: poor");
  } else if (co < 400) {
    matter_air_quality_sensor.set_air_quality(MatterAirQuality::AirQuality_t::VERY_POOR);
    Serial.println("Current air quality: very poor");
  } else if (co < 500) {
    matter_air_quality_sensor.set_air_quality(MatterAirQuality::AirQuality_t::EXTREMELY_POOR);
    Serial.println("Current air quality: extremely poor");
  }
}

void heartRate()
{
  int16_t heartrate = (analogRead(HeartRate)-3000);
  matter_heartrate_sensor.set_measured_value_raw(heartrate); 
  Serial.printf("HeartRate value %d", heartrate);
  if( heartrate < 300)
  {
    digitalWrite(BUZZER, HIGH);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
  }
  else{
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }
}

void pirSensor()
{
  bool pirValue = digitalRead(PIR);
  Serial.printf("\nPIR value: %d\n", pirValue);
  matter_PIR_sensor.set_onoff(pirValue);
}

void loop() 
{
  temperatureSensor();
  delay (200);
  pressureSensor();
  delay(200);
  fallDetection();
  delay(200);
  airQuality();
  delay(200);
  heartRate();
  delay(200);
  pirSensor();
  delay(200);
}
