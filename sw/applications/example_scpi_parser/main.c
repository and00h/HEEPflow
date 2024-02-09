#include "scpi/scpi.h"
#include "uart.h"
#include "soc_ctrl.h"
#include "core_v_mini_mcu.h"
#include "x-heep.h"

#define ECHO 1

#define SCPI_IDN1 "MANUFACTURE"
#define SCPI_IDN2 "INSTR2013"
#define SCPI_IDN3 NULL
#define SCPI_IDN4 "01-02"

volatile soc_ctrl_t soc_ctrl;
volatile uart_t uart;
volatile scpi_t scpi_context;
volatile int exit_scpi = 0;

scpi_result_t __attribute__((noinline)) MeasureCazzo(scpi_t * context) {
    SCPI_ResultInt(context, 42);
    return SCPI_RES_OK;
}

scpi_result_t __attribute__((noinline))  Exit(scpi_t * context) {
    exit_scpi = 1;
    uart_write(&uart, (const uint8_t *) "Exiting...\n", 11);
    return SCPI_RES_OK;
}

volatile scpi_command_t scpi_commands[] = {
	{ "MEASure:CAZZo?", MeasureCazzo, 0},
    { "*EXT", Exit, 0},
	SCPI_CMD_LIST_END
};

size_t __attribute__((noinline)) scrivi(scpi_t * context, const char * data, size_t len) {
    (void) context;
    size_t a = uart_write(&uart, (const uint8_t *) data, len);
    return a + uart_write(&uart, (const uint8_t *) "\r\n", 2);
}

int __attribute__((noinline))  SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;
    uart_write(&uart, (const uint8_t *) "ERR!\n", 5);
    return 0;
}

scpi_result_t __attribute__((noinline)) SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    return SCPI_RES_OK;
}
scpi_result_t __attribute__((noinline)) SCPI_Reset(scpi_t * context) {
    return SCPI_RES_OK;
}
scpi_result_t __attribute__((noinline))  SCPI_Flush(scpi_t * context) {
    return SCPI_RES_OK;
}

volatile scpi_interface_t scpi_interface = {
	.write = scrivi,
	.error = SCPI_Error,
	.control = NULL,
    .flush = NULL,
    .reset = NULL
};

#define SCPI_INPUT_BUFFER_LENGTH 256
static char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];

#define SCPI_ERROR_QUEUE_SIZE 17
scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

size_t __attribute__((noinline)) uart_gets(uart_t *uart, char *buf, size_t len) {
    size_t i;
    for (i = 0; i < len - 1; i++) {
        uint8_t c;
        uart_getchar(uart, &c);
        #if ECHO
        uart_putchar(uart, c);
        #endif
        if (c == '\n' || c == '\r' || c == '\0') {
            buf[i] = '\0';
            break;
        } else {
            buf[i] = c;
        }
    }
    return i;
}

void __attribute__((noinline)) uart_scpi(scpi_t * context, uart_t * uart) {
    while (!exit_scpi) {
        char buffer[256];
        size_t len = uart_gets(uart, buffer, sizeof(buffer));
        if (len > 0) {
            SCPI_Input(context, buffer, len);
            SCPI_Input(context, "\r\n", 2);
        }
    }
}

int main() {
    soc_ctrl.base_addr = mmio_region_from_addr((uintptr_t)SOC_CTRL_START_ADDRESS);

    uart.base_addr   = mmio_region_from_addr((uintptr_t)UART_START_ADDRESS);
    uart.baudrate    = UART_BAUDRATE;
    uart.clk_freq_hz = soc_ctrl_get_frequency(&soc_ctrl);
    
    if (uart_init(&uart) != kErrorOk) {
        return;
    }

    SCPI_Init(&scpi_context, 
              scpi_commands, 
              &scpi_interface, 
              scpi_units_def, 
              SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4, 
              scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
              scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

    uart_scpi(&scpi_context, &uart);
    return 0;
}