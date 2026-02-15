#include "ch_flash.h"
#include "main.h"
#define MB85RC16_BASE_ADDR 0x50
void fram_init(void) {}
void fram_read(uint16_t addr, uint8_t *data, uint16_t len)
{
    eeprom_read_data(addr, data, len);
}
void fram_write(uint16_t addr, uint8_t *data, uint16_t len)
{
    eeprom_write_data(addr, data, len);
}
void save_all_data_to_fram(void)
{
    data_in_fram._mac_offset = running_data.mac_offset;
    data_in_fram._mode_data  = running_data.mode_data;
    PRINT("GAP_APPEARE = %04x\n", data_in_fram.GAP_APPEARE);

    fram_write(0, &data_in_fram, sizeof(data_in_fram));
}
