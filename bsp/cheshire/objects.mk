# Copyright 2025 ETH Zurich and University of Bologna.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

bsp-objs-y += $(O)/bsp_init.o
bsp-objs-y += $(O)/bsp_irq.o
bsp-objs-y += $(O)/bsp_tohost.o
bsp-objs-y += $(O)/bsp_uart.o
bsp-objs-y += $(O)/bsp/shared/crt0.o
bsp-objs-y += $(O)/bsp/shared/bsp_start.o
bsp-objs-y += $(O)/drivers/clint/clint.o

ifdef M5
bsp-objs-y += $(O)/bsp/shared/m5/m5op.o
endif

VPATH += $(BSP)
VPATH += $(RVB_HOME)/bsp/shared
VPATH += $(RVB_HOME)/drivers
