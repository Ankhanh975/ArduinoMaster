#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "thread1.h"
#include "thread2.h"

void task1(void *pvParameter)
{
  thread1_setup();
  Serial.println("Starting task 1");
  while (1)
  {
    thread1_loop();
    vTaskDelay(10 / portTICK_PERIOD_MS); // Add a small delay to yield control
    }
}
void task2(void *pvParameter)
{
  thread2_setup();
  while (1)
  {
    thread2_loop();
    vTaskDelay(10 / portTICK_PERIOD_MS); // Add a small delay to yield control
  }
}

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Starting setup, creating tasks");

  // Create tasks on different cores
  xTaskCreatePinnedToCore(
      task1,    // Task function
      "Task 1", // Task name
      2048,     // Stack size
      NULL,     // Task parameters
      1,        // Priority
      NULL,     // Task handle
      0         // Core 0
  );

  xTaskCreatePinnedToCore(
      task2,    // Task function
      "Task 2", // Task name
      2048,     // Stack size
      NULL,     // Task parameters
      1,        // Priority
      NULL,     // Task handle
      1         // Core 1
  );
}

void loop()
{
  // The loop function is not used in this case since tasks run on different cores.
  // You can leave it empty or use it for other tasks if needed.
}