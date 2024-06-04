/*
 * Copyright (c) 2019 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PROD_CONSUMER_APP_A_H
#define PROD_CONSUMER_APP_A_H

#include <zephyr/kernel.h>
#include <zephyr/app_memory/app_memdomain.h>
#include "utility.h"

void app_eddi(void *p1, void *p2, void *p3);
void setup_eddi ();

extern struct k_mem_partition app_eddi_partition;
#define APP_EDDI_DATA	K_APP_DMEM(app_eddi_partition)
#define APP_EDDI_BSS	K_APP_BMEM(app_eddi_partition)

#define EDDI_STATE_IDLE 0
#define EDDI_STATE_ON 1
#define EDDI_STATE_EXP 2
#define EDDI_STATE_DL 3

#define EDDI_ANT_PIN_NODE DT_ALIAS (ant0)
#define I2C_EDDI_ADDR 0x05


#endif /* PROD_CONSUMER_APP_A_H */