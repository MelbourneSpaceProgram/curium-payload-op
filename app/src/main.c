#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/libc-hooks.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#include "app_eddi.h"

#define LED0_NODE DT_ALIAS(led0)
#define I2C_DEV_NODE DT_NODELABEL(i2c1)

#define APP_EDDI_STACKSIZE	2048
#define I2C_EDDI_ADDR 0x05

LOG_MODULE_REGISTER(app_main);

/* Define a thread for the root of application A.
 */
struct k_thread eddi_thread;
K_THREAD_STACK_DEFINE(eddi_stack, APP_EDDI_STACKSIZE);

APP_EDDI_BSS static const struct device *const i2c_dev = DEVICE_DT_GET(I2C_DEV_NODE);
APP_EDDI_DATA uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_CONTROLLER;

int main(void)
{
	k_tid_t thread_eddi;
	uint32_t i2c_cfg_tmp;

	LOG_INF("APP A partition: %p %zu", (void *)app_eddi_partition.start,
		(size_t)app_eddi_partition.size);
#ifdef Z_LIBC_PARTITION_EXISTS
	LOG_INF("libc partition: %p %zu", (void *)z_libc_partition.start,
		(size_t)z_libc_partition.size);
#endif
	sys_heap_init(&shared_pool, shared_pool_mem, HEAP_BYTES);

	//set up I2C
	if (!device_is_ready(i2c_dev)) {
		printf("I2C device is not ready\n");
		return 0;
	}

	if (i2c_configure(i2c_dev, i2c_cfg)) {
		printf("I2C config failed\n");
		return 0;
	}

	if (i2c_get_config(i2c_dev, &i2c_cfg_tmp)) {
		printf("I2C get_config failed\n");
		return 0;
	}
	if (i2c_cfg != i2c_cfg_tmp) {
		printf("I2C get_config returned invalid config\n");
		return 0;
	}

	/* Spawn supervisor entry for application A */
	thread_a = k_thread_create(&eddi_thread, eddi_thread, APP_EDDI_STACKSIZE,
				   app_eddi, NULL, NULL, NULL,
				   -1, K_USER, K_NO_WAIT);

	k_thread_join(thread_a, K_FOREVER);
	return 0;
}