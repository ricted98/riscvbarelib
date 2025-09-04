// Copyright 2025 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
//
// Nils Wistoff <nwistoff@iis.ee.ethz.ch>
// Paul Scheffler <paulsc@iis.ee.ethz.ch>

#include "bsp/bsp_uart.h"
#include "bsp/bsp_util.h"
#include "bsp/bsp_params.h"

void bsp_uart_init(void *uart_base, uint64_t freq, uint64_t baud) {
    uint64_t divisor = freq / (baud << 4);
    uint8_t dlo = (uint8_t)(divisor);
    uint8_t dhi = (uint8_t)(divisor >> 8);
    *reg8(uart_base, UART_INTR_ENABLE_REG_OFFSET) = 0x00;   // Disable all interrupts
    *reg8(uart_base, UART_LINE_CONTROL_REG_OFFSET) = 0x80;  // Enable DLAB (set baud rate divisor)
    *reg8(uart_base, UART_DLAB_LSB_REG_OFFSET) = dlo;       // divisor (lo byte)
    *reg8(uart_base, UART_DLAB_MSB_REG_OFFSET) = dhi;       // divisor (hi byte)
    *reg8(uart_base, UART_LINE_CONTROL_REG_OFFSET) = 0x03;  // 8 bits, no parity, one stop bit
    *reg8(uart_base, UART_FIFO_CONTROL_REG_OFFSET) = 0xC7;  // Enable & clear FIFO, 14B threshold
    *reg8(uart_base, UART_MODEM_CONTROL_REG_OFFSET) = 0x20; // Autoflow mode
}

int bsp_uart_read_ready(void *uart_base) {
    return *reg8(uart_base, UART_LINE_STATUS_REG_OFFSET) & (1 << UART_LINE_STATUS_DATA_READY_BIT);
}

static inline int __uart_write_ready(void *uart_base) {
    return *reg8(uart_base, UART_LINE_STATUS_REG_OFFSET) & (1 << UART_LINE_STATUS_THR_EMPTY_BIT);
}

static inline int __uart_write_idle(void *uart_base) {
    return __uart_write_ready(uart_base) &&
           *reg8(uart_base, UART_LINE_STATUS_REG_OFFSET) & (1 << UART_LINE_STATUS_TMIT_EMPTY_BIT);
}

void bsp_uart_write(void *uart_base, uint8_t byte) {
    while (!__uart_write_ready(uart_base))
        ;
    *reg8(uart_base, UART_THR_REG_OFFSET) = byte;
}

void bsp_uart_write_flush(void *uart_base) {
    // Ensure our read comes after any prior writes only
    // TODO: CVA6 likely violates inter-read-write ordering; double-check!
    fence();
    while (!__uart_write_idle(uart_base))
        ;
}

uint8_t bsp_uart_read(void *uart_base) {
    while (!bsp_uart_read_ready(uart_base))
        ;
    return *reg8(uart_base, UART_RBR_REG_OFFSET);
}

// Default UART provides console
void bsp_uart_putchar(char byte) {
    bsp_uart_write(&__base_uart, byte);
}

int bsp_uart_getchar() {
    return bsp_uart_read(&__base_uart);
}
