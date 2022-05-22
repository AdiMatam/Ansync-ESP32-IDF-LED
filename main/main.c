
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

#define PORT      UART_NUM_0
#define BUF_SIZE (256)
#define delayMs(x) vTaskDelay(x / portTICK_PERIOD_MS);

// GLOBALS
enum State { // possible LED 'states'
    ON = 1, OFF, BLINK
};

static uint8_t ledPin = 2;  
static int ledState = OFF;
static nvs_handle_t nvsHandle;

const char* onCommand = "ledOn";
const char* offCommand = "ledOff";
const char* blinkCommand = "ledBlink";
// GLOBALS

static void setupUARTandLED() {
    gpio_reset_pin(ledPin);
    gpio_set_direction(ledPin, GPIO_MODE_OUTPUT);

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    ESP_ERROR_CHECK(uart_driver_install(PORT, BUF_SIZE, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(PORT, -1, -1, -1, -1)); // -1 to use 'default UART' 
}

static void setupNVS() {
    esp_err_t err = nvs_flash_init();
    err = nvs_open("storage", NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        printf("Error opening NVS nvsHandle!\n");
    }

    err = nvs_get_i32(nvsHandle, "ledState", &ledState);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        printf("NO NVS DATA\n\n");
        ledState = OFF; // default state for LED (if no data found) is OFF!
    }
    else if (err == ESP_OK)
        printf("Last stored state of LED was: %d\n\n", ledState);
}

static void updateNVS() {
    printf("Updating NVS\n\n");
    nvs_set_i32(nvsHandle, "ledState", ledState);
    nvs_commit(nvsHandle);
}

static void processCommand(const char* string) {
    bool update = true;
    if (strcmp(string, onCommand) == 0) {
        ledState = ON; 
        printf("Turning On...\n\n");
    }
    else if (strcmp(string, offCommand) == 0) {
        ledState = OFF; 
        printf("Turning Off...\n\n");
    }
    else if (strcmp(string, blinkCommand) == 0) {
        ledState = BLINK; 
        printf("Blink mode...\n\n");
    }
    else {
        update = false;
        printf("\nCOMMAND UNRECOGNIZED\n\n");
    }
    if (update)
        updateNVS();
}

static void performCurrentState() {
    if (ledState == ON)
        gpio_set_level(ledPin, 1);
    else if (ledState == OFF)
        gpio_set_level(ledPin, 0);
    else if (ledState == BLINK) {
        gpio_set_level(ledPin, 1);
        delayMs(500);
        gpio_set_level(ledPin, 0);
        delayMs(500);
    }
}

void app_main(void) {
    printf("\n\nIGNORE\n");
    setupUARTandLED();
    setupNVS();
    
    char data[BUF_SIZE]; // stores input

    const char NEWLINE = (char)13;
    int idx = 0;
    int readLen = 0;

    while (1) {
        // Reads character-by-character from UART, writes into 'data' buffer. As buffer fills,
        // function writes to the next location in buffer, hence: 'data + idx'
        readLen = uart_read_bytes(PORT, data + idx, BUF_SIZE - idx, 30 / portTICK_PERIOD_MS);
        if (readLen != 0) {
            idx += readLen; // incrementing 'idx' to end of string

            // if the latest character received is a NEWLINE (enter key), command is processed.
            if (data[idx - 1] == NEWLINE) { 
                data[idx - 1] = '\0'; // null-terminate at the newline
                printf("Received: %s\n\n", data);

                if (strcmp(data, "exit") == 0) { // exit the program
                    printf("Exiting\n\n");
                    break;
                }

                processCommand(data);
                
                memset(data, '\0', BUF_SIZE); // clearing input buffer
                idx = 0;
            }
        }
        performCurrentState(); // perform current LED state while awaiting input
    }
    printf("Goodbye!\n\n");
    nvs_close(nvsHandle); // close out...
}

