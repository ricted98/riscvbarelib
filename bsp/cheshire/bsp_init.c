// Copyright 2025 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <string.h>
#include <stdio.h>
#include "common/cpu.h"
#include "common/cpu_defs.h"
#include "common/tohost.h"
#include "common/mp.h"
#include "common/io.h"
#include "bsp/bsp_uart.h"
#include "bsp/bsp_params.h"

void bsp_mp_init();
void bsp_irq_init();

void bsp_init()
{
    extern void (*_putchar)(char c);
    extern int  (*_getchar)();
    extern void (*_tohost_exit)(int status);

    bsp_uart_init(&__base_uart, UART_REFCLK, UART_BAUDRATE);
    _putchar = bsp_uart_putchar;
    _getchar = bsp_uart_getchar;

    _tohost_exit = bsp_tohost_exit;

    printf("Executing the riscvbarelib env (compiled: %s | %s)\n",
            __DATE__, __TIME__);

    write_csr(mhpmevent3, 1); // select Icache Miss Event
    write_csr(mhpmevent4, 2); // select Dcache Miss Event

    bsp_mp_init();
    bsp_irq_init();
}

void bsp_mp_init()
{
    memset((void*)cpu_hid2sid, 0xff, sizeof(cpu_hid2sid));
    memset((void*)cpu_sid2hid, 0xff, sizeof(cpu_sid2hid));

    for (int i = 0; i < BSP_CONFIG_NCPUS; i++) {
        //  In this platform. the hart and logical IDs are the same.
        cpu_hid2sid[i] = i;

        //  In this platform. the hart and logical IDs are the same.
        cpu_sid2hid[i] = i;

        //  By default consider that all CPUs are in the IDLE state
        cpu_set_state(i, CPU_IDLE);
    }

    cpu_dfence();
}
