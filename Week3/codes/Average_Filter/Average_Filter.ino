#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define STEP_NUM 5.0

Adafruit_MPU6050 mpu;

float acc_value = 0.0;
float average_acc_value = 0.0;


void setup(void) {
  Serial.begin(115200);

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1);
  }
  // Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
}
long unsigned int timer = 0;
void loop() {

  if (millis() - timer >= 100) {
    for (int i = 0; i < STEP_NUM; i++) {
      mpu.read();
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      acc_value = a.acceleration.x;
      average_acc_value += acc_value;
    }

    average_acc_value /= STEP_NUM;
    timer = millis();
    Serial.print(acc_value, 6);Serial.print(",");Serial.println(average_acc_value, 6);
  }


}
