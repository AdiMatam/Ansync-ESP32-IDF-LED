#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "esp_vfs_fat.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#define delayMs(x) vTaskDelay(x / portTICK_PERIOD_MS);

typedef esp_console_cmd_t ConsoleCommand;
typedef esp_console_cmd_func_t CommandFunction;

static void updateNVS(int newState);

enum State {
    ON = 1, OFF, BLINK
};

// GLOBALS
static uint8_t ledPin = 2;  
static int ledState = OFF;
nvs_handle_t nvsHandle;
esp_console_repl_t *REPL = NULL;
//

static int ledOn(int argc, char **argv) {
    gpio_set_level(ledPin, 1);
    printf("\n");
    updateNVS(ON);
    return 0;
}

static int ledOff(int argc, char **argv) {
    gpio_set_level(ledPin, 0);
    printf("\n");
    updateNVS(OFF);
    return 0;
}

static int exitRepl(int argc, char **argv) {
    REPL->del(REPL);
    printf("\nCLOSED REPL!\n");
    nvs_close(nvsHandle);
    return 0;
}


void _blinker() {
    for (int i = 0; i < 3; i++) {
        gpio_set_level(ledPin, 1);
        delayMs(500);
        gpio_set_level(ledPin, 0);
        delayMs(500);
    }
}

static int ledBlink(int argc, char **argv) {
    _blinker();
    printf("\n\n");
    updateNVS(BLINK);
    return 0;
}

static void setupNVS() {
    esp_err_t err = nvs_flash_init();
    err = nvs_open("storage", NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        printf("Error opening NVS nvsHandle!\n");
    }

    err = nvs_get_i32(nvsHandle, "ledState", &ledState);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        printf("\nNO NVS DATA\n");
        ledState = OFF;
    }
}

static void updateNVS(int newState) {
    printf("\n\nUpdating NVS\n");
    ledState = newState;
    nvs_set_i32(nvsHandle, "ledState", ledState);
    nvs_commit(nvsHandle);
}

void app_main(void) {

    gpio_reset_pin(ledPin);
    gpio_set_direction(ledPin, GPIO_MODE_OUTPUT);

    setupNVS();
    printf("\nOld state of LED was: %d\n", ledState);

    if (ledState == ON)
        gpio_set_level(ledPin, 1);
    else if (ledState == OFF)
        gpio_set_level(ledPin, 0);
    else
        _blinker();

    // COMMANDS
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();

    repl_config.prompt = "Enter command:";
    repl_config.max_cmdline_length = 1024;

    const ConsoleCommand onCommand = {
        .command = "ledOn",
        .help = "LED function",
        .hint = NULL,
        .func = &ledOn,
        .argtable = NULL
    };
    const ConsoleCommand offCommand = {
        .command = "ledOff",
        .help = "LED function",
        .hint = NULL,
        .func = &ledOff,
        .argtable = NULL
    };
    const ConsoleCommand blinkCommand = {
        .command = "ledBlink",
        .help = "LED function",
        .hint = NULL,
        .func = &ledBlink,
        .argtable = NULL
    };
     
    const ConsoleCommand exitCommand = {
        .command = "exit",
        .help = "Exits the repl",
        .hint = NULL,
        .func = &exitRepl,
        .argtable = NULL
    };

    esp_console_register_help_command();
    ESP_ERROR_CHECK(esp_console_cmd_register(&onCommand));
    ESP_ERROR_CHECK(esp_console_cmd_register(&offCommand));
    ESP_ERROR_CHECK(esp_console_cmd_register(&blinkCommand));
    ESP_ERROR_CHECK(esp_console_cmd_register(&exitCommand));

    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &REPL));
    ESP_ERROR_CHECK(esp_console_start_repl(REPL));
}