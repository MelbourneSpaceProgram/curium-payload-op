/*
 * Copyright (c) 2019 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PROD_CONSUMER_APP_A_H
#define PROD_CONSUMER_APP_A_H

#include <zephyr/kernel.h>
#include <zephyr/app_memory/app_memdomain.h>

void app_eddi_entry(void *p1, void *p2, void *p3);

extern struct k_mem_partition app_eddi_partition;
#define APP_EDDI_DATA	K_APP_DMEM(app_eddi_partition)
#define APP_EDDI_BSS	K_APP_BMEM(app_eddi_partition)

#define EDDI_STATE_IDLE 0
#define EDDI_STATE_ON 1
#define EDDI_STATE_EXP 2
#define EDDI_STATE_DL 3

#define EDDI_ANT_PIN_NODE DT_ALIAS (ant0)

APP_EDDI_DATA uint8_t eddi_state = EDDI_STATE_IDLE;
APP_EDDI_DATA uint8_t eddi_mode_flag = 5 << 5; //debris mode
APP_EDDI_DATA uint8_t eddit_gain_flag = 0 << 3; // x2 (6dB)
APP_EDDI_DATA uint8_t eddit_runtime_flag = 5; // 90min (5*18)

static const struct device *const ant_pin = DEVICE_DT_GET(EDDI_ANT_PIN_NODE);

#endif /* PROD_CONSUMER_APP_A_H */