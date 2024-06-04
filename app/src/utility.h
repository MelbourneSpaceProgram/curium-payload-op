
#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>

#define I2C_DEV_NODE DT_NODELABEL(i2c1)

extern const struct device *const i2c_dev;
extern uint32_t i2c_cfg;