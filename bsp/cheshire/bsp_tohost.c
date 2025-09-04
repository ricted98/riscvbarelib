// Copyright 2025 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include <stdlib.h>
#include <stdint.h>
#include "common/tohost.h"
#include "common/io.h"
#include "bsp/bsp_uart.h"
#include "bsp/bsp_params.h"
#include "bsp/bsp_regs.h"

void bsp_tohost_exit(int status)
{
    bsp_uart_write_flush(&__base_uart);
    uint32_t v = ((uint32_t)status << 1) | 1u;
    iowritel((uintptr_t)&__base_regs + CHESHIRE_SCRATCH_2_REG_OFFSET, v);
    while (1) cpu_wait_for_interrupt();
}
