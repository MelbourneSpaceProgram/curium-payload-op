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

#endif /* PROD_CONSUMER_APP_A_H */