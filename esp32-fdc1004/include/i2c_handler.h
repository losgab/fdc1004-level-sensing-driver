#include <driver/i2c.h>

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_FREQ_HZ 100000 

esp_err_t i2c_master_init(void);