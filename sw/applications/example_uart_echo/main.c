#include <stdio.h>
#include "uart.h"
#include "soc_ctrl.h"
#include "core_v_mini_mcu.h"
#include "x-heep.h"

int main() {
  char s1[] = "Write: ";

  soc_ctrl_t soc_ctrl;
  soc_ctrl.base_addr = mmio_region_from_addr((uintptr_t)SOC_CTRL_START_ADDRESS);

  uart_t uart;
  uart.base_addr   = mmio_region_from_addr((uintptr_t)UART_START_ADDRESS);
  uart.baudrate    = UART_BAUDRATE;
  uart.clk_freq_hz = soc_ctrl_get_frequency(&soc_ctrl);

  if (uart_init(&uart) != kErrorOk) {
      return -1;
  }

  uart_write(&uart, (uint8_t *)s1, sizeof(s1));
  
  uint8_t rd;
  while (1) {
    uart_getchar(&uart, &rd);
    uart_putchar(&uart, rd);
    if (rd == '\r' || rd == '\n') {
      break;
    }
  }
  printf("\r\n");
  return 0;
}