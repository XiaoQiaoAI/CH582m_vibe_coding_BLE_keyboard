#ifndef __MAIN_H
#define __MAIN_H
// #define APPEARANCE GAP_APPEARE_HID_DIGITIZER_TYABLET
// #define NAME "Ptisak42 wow~"
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#include "psk_ps.h"

#define MCT_test_event     (1 << 0)
#define MCT_light_control  (1 << 1)
#define MCT_key_scan       (1 << 2)
#define MCT_event_update   (1 << 3)
#define MCT_adc_measure    (1 << 4)
#define MCT_light_reset    (1 << 5)
#define MCT_music_ticks    (1 << 6)
#define MCT_POWER_OFF      (1 << 7)
#define MCT_POWER_OFF_buzz (1 << 8)
#define MCT_WS2812_MODE    (1 << 9)

#include "CONFIG.h"
#include "HAL.h"
#include "adc.h"
#include "fram_RC16.h"
#include "gpio.h"
#include "hiddev.h"
#include "hidkbd.h"
#include "ips096_st7735.h"
#include "pm.h"
#include "usb1_hid.h"
#define POWER_ON GPIOA_SetBits(GPIO_Pin_12)
#define POWER_OFF                     \
    {                                 \
        save_all_data_to_fram();      \
        GPIOA_ResetBits(GPIO_Pin_12); \
        while (1) {                   \
        }                             \
    }

enum music_flag {
    MUS_STOP = 0x01,
    MUS_1    = 0x02,
    MUS_2    = 0x04,
    MUS_3    = 0x08,
    MUS_cus  = 0x10

};
enum ws2812_mode_e {
    WS2812_OFF = 0x0, // all led black (off)
    WS2812_SINGLE_MOVE,
    WS2812_RAINBOW_WAVE,
    WS2812_BREATHING,

};
typedef struct
{
    uint8_t mac_offset;
    uint8_t mode_data; // mode
    uint8_t edit_flag; // 0 no, 1 edit mode 2 mac
    uint8_t tmp_mac;
    int     v_bat;
    uint8_t power_persent;
    uint8_t bt_connect_stat;             // 0 no 1 connecting 2 ed

    uint8_t            usb_is_connected; // 0 ble, 1 usb
    enum ws2812_mode_e ws2812_mode;
    uint32_t           power_off_timeout;
} running_data_s;
// #pragma pack(1)
typedef struct
{
    uint8_t _mac_offset;
    uint8_t _mode_data;
    uint8_t _reserved[2];

    uint32_t key_count[7];
    uint32_t osu_mode_key_count[7];

    uint16_t GAP_APPEARE;

    uint8_t device_name[22]; // 21 in fact
    //----------------------------------------------------------------
    uint8_t user_key_bind[6][62];
    uint8_t user_key_desc[4][12];
} data_in_fram_s;
extern data_in_fram_s data_in_fram;
#define len_data_in_fram sizeof(data_in_fram_s)

void start_music(int x);
void power_deep_sleep(void);

// *notes; //
extern running_data_s running_data;
extern tmosTaskID     mTaskID;
// #include "ips096_st7735.h"
#include "buzzerDriver.h"
#include "psk_hid.h"
#include "psk_led.h"
#include "psk_multi_button.h"
#include "psk_ps.h"
#include "psk_ws2812.h"
void usb_desc_set(void);
void init_desp(void);

void sub_main(void);
void sub_main_1(void);
#endif
