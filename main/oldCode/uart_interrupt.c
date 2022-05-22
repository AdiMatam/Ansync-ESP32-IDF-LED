/*
UART Interrupt Example
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_intr_alloc.h"

#define EX_UART_NUM UART_NUM_0

#define BUF_SIZE (256)
static QueueHandle_t uartQueue;

static void IRAM_ATTR interruptRoutine(void *p) {
    printf("\nI'm in the interrupt\n");
}

void app_main()
{

	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};

    // ESP_ERROR_CHECK(uart_enable_intr_mask(EX_UART_NUM, 0xffffffff));

	ESP_ERROR_CHECK(uart_param_config(EX_UART_NUM, &uart_config));

	//Set UART pins (using UART0 default pins ie no changes.)
	ESP_ERROR_CHECK(uart_set_pin(EX_UART_NUM, -1, -1, -1, -1));

	//Install UART driver, and get the queue.
	ESP_ERROR_CHECK(uart_driver_install(EX_UART_NUM, BUF_SIZE, 0, 20, &uartQueue, 0));

	// // release the pre registered UART handler/subroutine
	ESP_ERROR_CHECK(uart_isr_free(EX_UART_NUM));

	// // register new UART subroutine
    printf("\nbefore register\n");
	ESP_ERROR_CHECK(uart_isr_register(EX_UART_NUM, interruptRoutine, NULL, ESP_INTR_FLAG_IRAM, NULL));
    printf("\nafter register\n");

	if (uart_enable_rx_intr(EX_UART_NUM) == 0) printf("\neabled rx\n");
	// enable RX interrupt

    // xTaskCreate(interruptRoutine, "UART_ISR_ROUTINE", 2048, NULL, 12, NULL);

    while (1) {

    }
}
