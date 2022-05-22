#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "nvs.h"
#include "nvs_flash.h"

#define TXD_PIN 1
#define RXD_PIN 3
#define SERIAL_SIZE_RX 130


// SOME HELPER FUNCS
static inline void delayMs(int dlyTime) {
    vTaskDelay(dlyTime / portTICK_PERIOD_MS);
}

static void blink(uint8_t pin, int dly) {
    gpio_set_level(pin, 1);
    delayMs(dly);
    gpio_set_level(pin, 0);
    delayMs(dly);
}
//

static uint8_t ledPin = 2;  
static const char* onCommand = "ledOn";
static const char* offCommand = "ledOff";
static const char* blinkCommand = "ledBlink";

enum State {
    ON = 1, OFF, BLINK
};

static int ledState = OFF;

static void receiveData(uint8_t* data) {
    int length = 0;

    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_0, data, SERIAL_SIZE_RX, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            printf("Received %d bytes: %s", rxBytes, (const char*)data);
            uart_flush(UART_NUM_0);
            return;
        }
    }
}

void setupNVS(nvs_handle_t* handle) {
    esp_err_t err = nvs_flash_init();
    err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        printf("Error opening NVS Handle!\n"); return -1;
    }

    err = nvs_get_i32(*handle, "ledState", &ledState);
    if (err == ESP_ERR_NVS_NOT_FOUND)
        ledState = OFF;
}

void app_main(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, SERIAL_SIZE_RX, 0, 0, NULL, 0);

    printf("Setup UART\n");

    uint8_t* data = (uint8_t*) malloc(SERIAL_SIZE_RX+1);
    receiveData(data);

    printf("Received: %s", data);

    free(data);


    // const char* msg = (const char*) data;

    // gpio_reset_pin(ledPin);
    // gpio_set_direction(ledPin, GPIO_MODE_OUTPUT);

    // nvs_handle_t nvsHandle;
    // setupNVS(&nvsHandle);

    // if (strcmp(msg, onCommand))
    //     ledState = ON;
    // else if (strcmp(msg, offCommand))
    //     ledState = OFF;
    // else if (strcmp(msg, blinkCommand))
    //     ledState = BLINK;
    // else {
    //     printf("Invalid message\n");
    //     return 1;
    // }

    // nvs_set_i32(nvsHandle, "ledState", ledState);
    // nvs_commit(nvsHandle);
    // nvs_close(nvsHandle);
}