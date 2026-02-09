#include "main.h"
#include "zf_driver_soft_iic.h"

extern soft_iic_info_struct fram_iic;

void fram_init(void);
void fram_write(uint16_t addr, uint8_t *data, uint16_t len);
void fram_read(uint16_t addr, uint8_t *data, uint16_t len);
void save_all_data_to_fram(void);
