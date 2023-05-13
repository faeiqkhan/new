#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "lcd_i2c.h"
#include <esp_err.h>
#include <dht/dht.h>

#define DHT_PIN 5
#define DHT_TYPE DHT_TYPE_11

TaskHandle_t lcdTask;

void lcdTaskFunction(void* pvParameters) {
    lcd_init();

    lcd_print("Humidity:      ");
    lcd_set_cursor(0, 1);
    lcd_print("Temperature:   ");

    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    i2c_set_pin(I2C_NUM_0, 4, 5, 0);

    dht_init(DHT_PIN, DHT_TYPE);

    while (true) {
        float humidity = dht_get_humidity();
        float temperature = dht_get_temperature();

        lcd_set_cursor(10, 0);
        lcd_print_float(humidity, 2);
        lcd_set_cursor(14, 1);
        lcd_print_float(temperature, 2);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main() {
    nvs_flash_init();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();

    xTaskCreate(
        lcdTaskFunction,
        "lcdTask",
        4096,
        NULL,
        1,
        &lcdTask
    );
}
