#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define STEP_NUM 5

Adafruit_MPU6050 mpu;

float acc_value = 0.0;
float median_acc_value = 0.0;
float acc[STEP_NUM] = {0.0};
float acc_copy[STEP_NUM] = {0.0};
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
    median_acc_value = mov_median(counter, acc_value);

    timer = millis();
    Serial.print(acc_value, 6); Serial.print(","); Serial.println(median_acc_value, 6);
  }


}

float mov_median(int* counter, float input) {
  float med = 0.0;
  if (*counter < STEP_NUM - 1) {
    acc[*counter] = input;
    acc_copy[*counter] = input;
    *counter++;
    return input;
  }

  acc[STEP_NUM - 1] = input;
  acc_copy[STEP_NUM - 1] = input;
  
  med = median(acc_copy, STEP_NUM);
  
  copy_arr(acc, acc_copy, STEP_NUM);
  
  leftRotate(acc, STEP_NUM);
  leftRotate(acc_copy, STEP_NUM);

  return med;
}

float median(float* a, int n) 
{ 
    // First we sort the array 
    quickSort(a, 0, n-1); 
  
    // check for even case 
    if (n % 2 != 0) 
        return (float)a[n / 2]; 
  
    return (float)(a[(n - 1) / 2] + a[n / 2]) / 2.0; 
} 
// A utility function to swap two elements  
void swap(float* a, float* b)  
{  
    float t = *a;  
    *a = *b;  
    *b = t;  
}  
  
/* This function takes last element as pivot, places  
the pivot element at its correct position in sorted  
array, and places all smaller (smaller than pivot)  
to left of pivot and all greater elements to right  
of pivot */
int partition (float* arr, int low, int high)  
{  
    int pivot = arr[high]; // pivot  
    int i = (low - 1); // Index of smaller element  
  
    for (int j = low; j <= high - 1; j++)  
    {  
        // If current element is smaller than the pivot  
        if (arr[j] < pivot)  
        {  
            i++; // increment index of smaller element  
            swap(&arr[i], &arr[j]);  
        }  
    }  
    swap(&arr[i + 1], &arr[high]);  
    return (i + 1);  
}  
  
/* The main function that implements QuickSort  
arr[] --> Array to be sorted,  
low --> Starting index,  
high --> Ending index */
void quickSort(float arr[], int low, int high)  
{  
    if (low < high)  
    {  
        /* pi is partitioning index, arr[p] is now  
        at right place */
        int pi = partition(arr, low, high);  
  
        // Separately sort elements before  
        // partition and after partition  
        quickSort(arr, low, pi - 1);  
        quickSort(arr, pi + 1, high);  
    }  
}

void copy_arr(float* arr1, float* arr2, int n){
  for(int i = 0; i < n; i++){
    arr2[i] = arr1[i];
  }
  
}

void leftRotate(float* arr, int n)
{
  int temp = arr[0], i;
  for (i = 0; i < n - 1; i++)
    arr[i] = arr[i + 1];

  arr[i] = temp;
}
