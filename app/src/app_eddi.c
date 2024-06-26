/*
 * Copyright (c) 2019 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/libc-hooks.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/app_memory/mem_domain.h>

#include "app_eddi.h"

LOG_MODULE_REGISTER(app_eddi);

#define MAX_MSGS	8
#define EDDI_BUFF_DATA_SIZE 100


/* Resource pool for allocations made by the kernel on behalf of system
 * calls. Needed for k_queue_alloc_append()
 */
K_HEAP_DEFINE(app_eddi_resource_pool, 256 * 5 + 128);

/* Define app_a_partition, where all globals for this app will be routed.
 * The partition starting address and size are populated by build system
 * and linker magic.
 */
K_APPMEM_PARTITION_DEFINE(app_eddi_partition);

/* Memory domain for application A, set up and installed in app_a_entry() */
static struct k_mem_domain app_eddi_domain;

/* Message queue for IPC between the driver callback and the monitor thread.
 *
 * This message queue is being statically initialized, no need to call
 * k_msgq_init() on it.
 */
// K_MSGQ_DEFINE(mqueue, SAMPLE_DRIVER_MSG_SIZE, MAX_MSGS, 4);

/* Processing thread. This takes data that has been processed by application
 * B and writes it to the sample_driver, completing the control loop
 */
// struct k_thread writeback_thread;
// K_THREAD_STACK_DEFINE(writeback_stack, 2048);

/* Global data used by application A. By tagging with APP_A_BSS or APP_A_DATA,
 * we ensure all this gets linked into the continuous region denoted by
 * app_a_partition.
 */

APP_EDDI_DATA uint8_t eddi_state = EDDI_STATE_IDLE;
APP_EDDI_DATA uint8_t eddi_mode_flag = 5 << 5; //debris mode
APP_EDDI_DATA uint8_t eddi_gain_flag = 0 << 3; // x2 (6dB)
APP_EDDI_DATA uint8_t eddi_runtime_flag = 5; // 90min (5*18)

// const struct device *const i2c_dev = DEVICE_DT_GET(I2C_DEV_NODE);
// uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_CONTROLLER;

// static const struct device *const ant_pin = DEVICE_DT_GET(EDDI_ANT_PIN_NODE);

uint8_t meet_turnon_cond ()
{
    return 1;
}

uint8_t meet_exp_cond ()
{
    // conditions:
    // 1. in payload op mode
    // 2. health indicator checked

    // 3. antenna deployed
    // if (gpio_pin_get_dt(&ant_pin) == 0) {
    //     return 0;
    // }

    // all checked through
    return 1;
}

uint8_t has_complete_orbit ()
{

  return 0;
}

uint8_t has_complete_dl ()
{

  return 0;
}

void set_eddi_config ()
{

}

void turn_on_eddi () 
{
  // TODO tell EPS to provide power

  //set turn-on flags
  uint8_t set_flag = eddi_mode_flag | eddi_gain_flag |eddi_runtime_flag;

  if (i2c_write(i2c_dev, &set_flag, 1, I2C_EDDI_ADDR)) 
  {
    //TODO error handling
		printf("Fail to send i2c\n");
	}
}

void turn_off_eddi () 
{
  // TODO tell EPS to kill power
}

void eddi_exp_data_collect ()
{
  /**
   * 1. check the experiment mode in which EDDI ran the exp
   * 2. calculate data size based on the timer set and the experiment mode
   * 3. pull data from EDDI i2c_read()
   * 4. push data to COMM
  */

  // step 3
//   uint8_t databuf [EDDI_BUFF_DATA_SIZE];
//   i2c_read (i2c_dev, databuf, data_size, I2c_EDDI_ADDR);

}

uint8_t check_state ()
{
  uint8_t rt = 0;
  switch (eddi_state)
  {
  case EDDI_STATE_IDLE:
    rt = meet_turnon_cond ();
    break;
  case EDDI_STATE_ON:
    rt = meet_exp_cond ();
    break;
  case EDDI_STATE_EXP:
    rt = has_complete_orbit (); 
    break;
  case EDDI_STATE_DL:
    rt = has_complete_dl (); 
    break;
  default:
    //Unrecognized state
    //TODO add error handling
    break;
  }

  return rt;
}

void move_to_next_state ()
{
  switch (eddi_state)
  {
  case EDDI_STATE_IDLE:
    turn_on_eddi ();
    eddi_state = EDDI_STATE_ON;
    break;

  case EDDI_STATE_ON:
    set_eddi_config ();
    eddi_state = EDDI_STATE_EXP;
    break;

  case EDDI_STATE_EXP:
    eddi_exp_data_collect ();
    turn_off_eddi ();
    eddi_state = EDDI_STATE_DL;
    break;

  case EDDI_STATE_DL:
    eddi_state = EDDI_STATE_IDLE;
    break;
  
  default:
    // Unrecognized state
    //TODO add error handling
    break;
  }
}

/* Supervisor mode setup function for application A */
void app_eddi(void *p1, void *p2, void *p3)
{

    int ret;
	struct k_mem_partition *parts[] = {
#if Z_LIBC_PARTITION_EXISTS
		&z_libc_partition,
#endif
		&app_eddi_partition
	};

	/* Initialize a memory domain with the specified partitions
	 * and add ourself to this domain. We need access to our own
	 * partition, the shared partition, and any common libc partition
	 * if it exists.
	 */
	ret = k_mem_domain_init(&app_eddi_domain, ARRAY_SIZE(parts), parts);
	// ret = k_mem_domain_init(&app_eddi_domain, 1, &app_eddi_partition);
	__ASSERT(ret == 0, "k_mem_domain_init failed %d", ret);
	ARG_UNUSED(ret);

	k_mem_domain_add_thread(&app_eddi_domain, k_current_get());

	/* Assign a resource pool to serve for kernel-side allocations on
	 * behalf of application A. Needed for k_queue_alloc_append().
	 */
	k_thread_heap_assign(k_current_get(), &app_eddi_resource_pool);

    eddi_state = EDDI_STATE_IDLE;
    while (1) 
    {
      k_msleep (1000);
      if (check_state () == 1)
      {
        move_to_next_state ();
      }

    }
}

void setup_eddi ()
{

    // configure antenna pin
    // gpio_pin_configure_dt (&ant_pin, GPIO_INPUT);
}