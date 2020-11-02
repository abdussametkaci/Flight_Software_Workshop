#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

float acc_value = 0.0;
float kalman_value = 0.0;
float kalman_old = 0.0;
float cov_old = 0.0;

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
    mpu.read();
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    acc_value = a.acceleration.x;
    kalman_value = kalman_filter(acc_value);

    timer = millis();
    Serial.print(acc_value, 6); Serial.print(","); Serial.println(kalman_value, 6);
  }


}

float kalman_filter (float input)
{
    
  float kalman_new = kalman_old; // eski değer alınır
  float cov_new = cov_old + 0.50; //yeni kovaryans değeri belirlenir. Q=0.50 alınmıştır
  
  float kalman_gain = cov_new / (cov_new + 0.9); //kalman kazancı hesaplanır. R=0.9 alınmıştır
  float kalman_calculated = kalman_new + (kalman_gain * (input - kalman_new)); //kalman değeri hesaplanır
  
  cov_new = (1 - kalman_gain) * cov_old; //yeni kovaryans değeri hesaplanır
  cov_old = cov_new; //yeni değerler bir sonraki döngüde kullanılmak üzere kaydedilir
  
  kalman_old = kalman_calculated;

  return kalman_calculated; //hesaplanan kalman değeri çıktı olarak verilir
}
