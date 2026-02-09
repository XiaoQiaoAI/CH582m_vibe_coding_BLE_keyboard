#include "main.h"
// #include "zf_driver_soft_iic.h"
#include "ch_flash.h"
// soft_iic_info_struct fram_iic;
#define MB85RC16_BASE_ADDR 0x50
void fram_init(void)
{
    // soft_iic_init(&fram_iic, MB85RC16_BASE_ADDR, 0, GPIOB, GPIO_Pin_18, GPIOB,
    // GPIO_Pin_19); fram_iic
}
void fram_read(uint16_t addr, uint8_t *data, uint16_t len)
{
    // fram_iic.addr = MB85RC16_BASE_ADDR | ((addr >> 8) & 0x07);
    // soft_iic_read_8bit_registers(&fram_iic, addr & 0x0ff, data, len);
    // PRINT("read %x, len %d\n", addr, len);
    eeprom_read_data(addr, data, len);
}
void fram_write(uint16_t addr, uint8_t *data, uint16_t len)
{
    // fram_iic.addr = MB85RC16_BASE_ADDR | ((addr >> 8) & 0x07);
    // soft_iic_write_8bit_registers(&fram_iic, addr & 0x0ff, data, len);
    // PRINT("write %x, len %d\n", addr, len);
    // for (int i = 0; i < len; i++)
    //     PRINT("%02x ", data[i]);
    eeprom_write_data(addr, data, len);
}
void save_all_data_to_fram(void)
{
    data_in_fram._mac_offset = running_data.mac_offset;
    data_in_fram._mode_data  = running_data.mode_data;
    PRINT("GAP_APPEARE = %04x\n", data_in_fram.GAP_APPEARE);

    fram_write(0, &data_in_fram, sizeof(data_in_fram));
}
