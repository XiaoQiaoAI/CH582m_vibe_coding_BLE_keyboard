#include "psk_multi_button.h"
#include "ips096_st7735.h"
#include "main.h"
#include "multi_button.h"
#include "psk_led.h"

#include "custom_control.h"
#include "keyboard.h"
#include "math.h"
#include "mouse.h"
#include "touch_screen.h"

uint8_t mode3_mode[TOTAL_KEY] = {0};
Button  keys__uz[TOTAL_KEY];
uint8_t key_scan_code = 0;

uint8_t key_read(para i);

uint8_t num_of_keys(void)
{
    uint8_t sum = 0;
    for (uint8_t i = 0; i < TOTAL_KEY; i++) {
        if (key_read(i) && i != 3)
            sum++;
    }
    return sum;
}

void shutdown(void)
{
    keyboard_press_sp_key(LEFT_GUI, 2);
    keyboard_press_char('x', 2);
    keyboard_delay(100);
    keyboard_press_sp_key(LEFT_GUI, 2);
    keyboard_press_char('x', 2);
    keyboard_delay(50);
    keyboard_press_str("uuuuuuuuuuuuuuuuuuu\n", 5);
}
void set_mode(uint8_t mode)
{
    __LimitValue(mode, 0, 4);
    running_data.mode_data = mode;
    LCD_CS_RESET;
    IPS_Clear(CYAN);
    char txt[15];
    sprintf(txt, "%1d.%02dV %d%%", running_data.v_bat / 100, running_data.v_bat % 100, running_data.power_persent);
    IPS_ShowString(8, 0, txt, MAGENTA);
    for (int i = 0; i < LED_NUM; i++) {
        ws2812_list[i].hex = 0;
    }
    ws2812_list[mode * 2].hex     = 0xFF2464ff;
    ws2812_list[mode * 2 + 1].hex = 0xFF2464ff;
    if (mode < 3)
        for (int i = 0; i < 4; i++) {
            if (key_bund.user_key_desc[mode][i][0])
                IPS_ShowString_len(8, 16 + i * 16, key_bund.user_key_desc[mode][i], RED, 20);
            else
                IPS_ShowString(0, 16 + i * 16, "N/A", BLUE);
        }
    LCD_CS_SET;
}
void key_scan(void)
{
    static uint16_t j;
    static uint32_t pin[] = {GPIO_Pin_8, GPIO_Pin_3, GPIO_Pin_2, GPIO_Pin_1, GPIO_Pin_0};
    // static GPIO_TypeDef *port[] = {KEY4_GPIO_Port, KEY3_GPIO_Port,
    // KEY2_GPIO_Port, KEY1_GPIO_Port, KEY0_GPIO_Port}; key_scan_code = 0;
    for (j = 0; j < 1; j++) {
        key_scan_code <<= 1;
        key_scan_code += !GPIOA_ReadPortPin(pin[j]);
    }
    for (j; j < TOTAL_KEY; j++) {
        key_scan_code <<= 1;
        key_scan_code += !GPIOB_ReadPortPin(pin[j]);
    }
}
uint8_t key_read(para i)
{
    return (key_scan_code >> i) & 0x0001;
}
uint8_t key_comm[]
    = {0x74, 8, DOWN_KEY, HID_KEYBOARD_X, UP_KEY, HID_KEYBOARD_X, DOWN_KEY, HID_KEYBOARD_X, UP_KEY, HID_KEYBOARD_X};
uint8_t key_comm_898[] = {0x73, 3, 0xe0, 0xe1, HID_KEYBOARD_ESCAPE};

void prase_user_key(uint8_t *data, uint8_t len, uint8_t if_press)
{
    // for (uint8_t i = 0; i < min(data[1], len) + 2; i++)
    // {
    // 	PRINT("%02x ", data[i]);
    // }
    if (data[0] == 0x73) {
        for (int i = 0; i < min(data[1], len); i++) {
            keyboard_press_key_code_1(data[2 + i], if_press);
        }
    } else if (data[0] == 0x74) {
        if (if_press) {
            kbd_write_command_list(data + 2, min(data[1], len));
        }
    }
}
void user_defined_mode(uint8_t key_index, uint8_t if_press, uint8_t mode)
{
    __LimitValue(mode, 0, 2);
    __LimitValue(key_index, 0, 3);
    prase_user_key(key_bund.user_key_bind[mode][key_index], sizeof(key_bund.user_key_bind[mode][key_index]), if_press);
}
/**
 *
 * @brief : 按键回调函数, index 按键下标
 * @note  :
 * @param {void} *button
 */
void key44callback(void *button)
{
    uint32_t        index;
    PressEvent      btn_event_val;
    static uint32_t count = 0;
    btn_event_val         = get_button_event((struct Button *) button);
    index                 = ((struct Button *) button)->func_para;
    // PRINT("%d , %d\n", index, btn_event_val);
    // if (running_data.mode_data == 3 && btn_event_val == PRESS_DOWN)
    //     running_data.ws2812_mode = index;
    if (running_data.edit_flag == 1) {
        running_data.have_edit = 1;
        if (btn_event_val == PRESS_DOWN) {
            set_mode(index);
            running_data.mode_data = index;
        }
    } else if (running_data.edit_flag == 0) {
        if (running_data.mode_data < 3)
            user_defined_mode(index, !btn_event_val, running_data.mode_data);
        else {
            // switch (index) {
            // case 0:
            //     keyboard_press_key_code_1(Keyboard_CapLock, !btn_event_val);
            //     break;
            // case 1:
            //     keyboard_press_sp_key_1(LEFT_CTRL, !btn_event_val);
            //     keyboard_press_char_1('a', !btn_event_val);
            //     break;
            // case 2:
            //     keyboard_press_char_1('x', !btn_event_val);
            //     break;
            // case 3:
            //     // custom_press_key_1(MUTE, !btn_event_val);
            //     keyboard_press_key_code_1(HID_KEYBPAD_ADD, !btn_event_val);
            //     // mouse_set_continue_click(KEY_LEFT, !btn_event_val);
            //     break;
            // }
        }
    }
    return;
}
void key44longcallback(void *button)
{
    uint32_t   index;
    PressEvent btn_event_val;
    btn_event_val = get_button_event((struct Button *) button);
    index         = ((struct Button *) button)->func_para;
    // PRINT("repeat:%d\n", ((struct Button *)button)->repeat);
    // PRINT("edit_flag: %d\n", running_data.edit_flag);

    switch (btn_event_val) {
    case PRESS_REPEAT:
        break;
    case LONG_PRESS_HOLD:
        // if (running_data.power_off_timeout > 5)
        // 	running_data.power_off_timeout = 3;
        // return;
        break;
    }
    if (((struct Button *) button)->repeat > 1)
        if (running_data.edit_flag == 1) {
            running_data.tmp_mac   = 0xff;
            running_data.edit_flag = 2;
            PRINT("SET:BTR\n");
        }
    if (((struct Button *) button)->repeat > 2)
    // if (running_data.edit_flag == 1)
    {
        // 	running_data.tmp_mac = 0xff;
        running_data.edit_flag = 3;
        PRINT("SET:CONT\n");
    }
    if (((struct Button *) button)->repeat > 3) {
        led_set_bk(80);
        tmos_stop_task(mTaskID, MCT_light_reset);
        tmos_start_task(mTaskID, MCT_light_reset, MS1_TO_SYSTEM_TIME(3 * 1000));

        // SYS_ResetKeepBuf(0x80 | (running_data.mode_data & 0x0f));
        // SYS_ResetExecute();
        // while (1)
        // 	;
    }
    if (((struct Button *) button)->repeat > 10) {
        // led_set_bk(80);
        // tmos_stop_task(mTaskID, MCT_light_reset);
        // tmos_start_task(mTaskID, MCT_light_reset, MS1_TO_SYSTEM_TIME(3 * 1000));

        // SYS_ResetKeepBuf(0x80 | (running_data.mode_data & 0x0f));
        // SYS_ResetExecute();
        power_reset(0x80 | (running_data.mode_data & 0x0f));
        while (1)
            ;
    }

    // 		break;
    // 	}
    // 	break;
    // }
}
void key_power_callback(void *button)
{
    uint32_t   index;
    PressEvent btn_event_val;
    btn_event_val = get_button_event((struct Button *) button);
    index         = ((struct Button *) button)->func_para;
    switch (btn_event_val) {
    case PRESS_DOWN: {
        running_data.edit_flag               = 1;
        running_data.have_edit               = 0;
        running_data.ws2812_mode_ignore_flag = 1;
        for (int i = 0; i < LED_NUM; i++) {
            ws2812_list[i].hex = 0;
        }
        set_mode(running_data.mode_data);
        break;
    }
    case PRESS_UP: {
        running_data.edit_flag               = 0;
        running_data.ws2812_mode_ignore_flag = 0;
        tmos_set_event(mTaskID, MCT_PIC_DISPLAY);
        break;
    }
    case LONG_PRESS_START: {
        if (running_data.have_edit == 0) {
            tmos_set_event(mTaskID, MCT_START_POWER_OFF);
        }
        break;
    }
    }
}
void my_button_init(void)
{
    int8_t i;
    memset(keys__uz, 0, sizeof(keys__uz));
    key_scan_code = 0;
    for (i = 0; i < TOTAL_KEY; i++) {
        button_init(keys__uz + i, key_read, i);
        button_attach(keys__uz + i, PRESS_DOWN, key44callback);
        button_attach(keys__uz + i, PRESS_UP, key44callback);
        if (i == 4) {
            button_attach(keys__uz + i, PRESS_DOWN, key_power_callback);
            button_attach(keys__uz + i, PRESS_UP, key_power_callback);
            // button_attach(keys__uz + i, PRESS_REPEAT, key44longcallback);
            // button_attach(keys__uz + i, SINGLE_CLICK, key44longcallback);
            // button_attach(keys__uz + i, DOUBLE_CLICK, key44longcallback);
            button_attach(keys__uz + i, LONG_PRESS_START, key_power_callback);
            // button_attach(keys__uz + i, LONG_PRESS_HOLD, key44longcallback);
        }
        button_start(keys__uz + i);
    }
}
