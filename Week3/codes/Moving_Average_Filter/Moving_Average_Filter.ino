#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define STEP_NUM 5

Adafruit_MPU6050 mpu;

float acc_value = 0.0;
float moving_average_acc_value = 0.0;
float acc[STEP_NUM] = {0.0};
int counter = 0;

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
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    acc_value = a.acceleration.x;
    moving_average_acc_value = mov_avg(counter, acc_value);

    timer = millis();
    Serial.print(acc_value, 6); Serial.print(","); Serial.println(moving_average_acc_value, 6);
  }


}

float mov_avg(int* counter, float input) {
  float avg = 0.0;
  if (*counter < STEP_NUM - 1) {
    acc[*counter] = input;
    *counter++;
    return input;
  }

  acc[STEP_NUM - 1] = input;
  avg = average(acc, STEP_NUM);
  leftRotate(acc, STEP_NUM);

  return avg;
}


float average(float* arr, int size) {
  float s = 0.0;
  for (int i = 0; i < size; i++) {
    s += arr[i];
  }

  return s / (float)size;
}

void leftRotate(float* arr, int n)
{
  int temp = arr[0], i;
  for (i = 0; i < n - 1; i++)
    arr[i] = arr[i + 1];

  arr[i] = temp;
}
