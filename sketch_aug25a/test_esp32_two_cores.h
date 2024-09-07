void task1(void *pvParameter) {
    // Task running on Core 0
    while (1) {
        // Control GPIO pins or perform actions
    }
}

void task2(void *pvParameter) {
    // Task running on Core 1
    while (1) {
        // Control GPIO pins or perform actions
    }
}

void app_main() {
    xTaskCreatePinnedToCore(task1, "Task 1", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(task2, "Task 2", 2048, NULL, 1, NULL, 1);
}