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
    running_data.mode_data = mode;
    // IPS_Clear(CYAN);
    switch (mode) {
    case 0:
        IPS_ShowString(16, 16, "1 click  ESC", RED);
        IPS_ShowString(16, 32, "         z  ", RED);
        IPS_ShowString(16, 48, "         x  ", RED);
        IPS_ShowString(16, 64, "vol  - '+' +", RED);
        break;
    case 1:
        // IPS_ShowString(16, 16, "Shft + F1   ", RED);
        // IPS_ShowString(16, 32, "long 2 click", RED);
        // IPS_ShowString(16, 48, "ESC   !     ", RED);
        // IPS_ShowString(16, 64, "vol  - p/s +", RED);
        IPS_ShowString_len(16, 16, data_in_fram.user_key_desc[0], RED, 12);
        IPS_ShowString_len(16, 32, data_in_fram.user_key_desc[1], RED, 12);
        IPS_ShowString_len(16, 48, data_in_fram.user_key_desc[2], RED, 12);
        IPS_ShowString_len(16, 64, data_in_fram.user_key_desc[3], RED, 12);
        break;
    case 2:
        IPS_ShowString(16, 16, "Ctrl + s    ", RED);
        IPS_ShowString(16, 32, "Ctrl + c    ", RED);
        IPS_ShowString(16, 48, "Ctrl + v    ", RED);
        IPS_ShowString(16, 64, "   <- ESC ->", RED);
        break;
    case 3:
        IPS_ShowString(16, 16, "pause  space", RED);
        IPS_ShowString(16, 32, "vol - mute +", RED);
        IPS_ShowString(16, 48, "  a -time+ s", RED);
        IPS_ShowString(16, 64, "nor    x z c", RED);

        break;
    }
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
        // PRINT("ASFKHJ\n");
        if (if_press) {
            kbd_write_command_list(data + 2, min(data[1], len));
        }
    }
}
void user_defined_mode(uint8_t key_index, uint8_t if_press)
{
    prase_user_key(data_in_fram.user_key_bind[key_index], 50, if_press);
    // prase_user_key(key_comm_898, 20, if_press);
    // switch (key_index)
    // {
    // case 0:
    // 	keyboard_press_sp_key_1(LEFT_SHIFT, if_press);
    // 	keyboard_press_key_code_1(Keyboard_F1_112____4_101_104, if_press);
    // 	break;
    // case 1:
    // 	// mouse_set_continue_click(KEY_LEFT, if_press);

    // 	kbd_write_command_list(key_comm, sizeof(key_comm));

    // 	// touch_set_continue_function(if _press);
    // 	break;
    // case 2:
    // 	// keyboard_press_sp_key_1(LEFT_SHIFT, if_press);
    // 	keyboard_press_key_code_1(Keyboard_ESCAPE, if_press);
    // 	break;
    // case 3:
    // 	// custom_press_key_1(MUTE, if_press);
    // 	custom_press_key(PAUSE_PLAY, if_press);
    // 	break;
    // }
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
    PRINT("%d , %d\n", index, btn_event_val);
    return;

    if (btn_event_val == PRESS_DOWN) {
        data_in_fram.key_count[index]++;
    }
    if (running_data.bt_connect_stat == 2) {
        running_data.power_off_timeout = 60 * 60;
    }
    if (index == 4) // 按下开始设置
    {
        // led_set_bk(80);
        tmos_stop_task(mTaskID, MCT_light_reset);
        tmos_start_task(mTaskID, MCT_light_reset, MS1_TO_SYSTEM_TIME(22 * 100));

        switch (btn_event_val) {
        case PRESS_DOWN:
            ps("Nor:\n");
            for (int i = 0; i < 4; i++) {
                ps("%d ", data_in_fram.key_count[i]);
            }
            ps("\nSp:");
            for (int i = 4; i < 5; i++) {
                ps("%d ", data_in_fram.key_count[i]);
            }
            ps("\nEnc:");
            for (int i = 5; i < 7; i++) {
                ps("%d ", data_in_fram.key_count[i]);
            }
            ps("\n");

            running_data.edit_flag = 1;
            break;
        case PRESS_UP:
            if (running_data.edit_flag == 2 && running_data.tmp_mac < 3) {
                if (running_data.tmp_mac != running_data.mac_offset) {
                    running_data.mac_offset = running_data.tmp_mac;
                    power_reset(0x80 | (running_data.mode_data & 0x0f));

                    while (1)
                        ;
                }
            }
            running_data.edit_flag = 0x0;
            // PRINT("edit_flag: %d\n",running_data.edit_flag);

            // running_data.is_setting_mac = 0;
            break;
        }
    } else if (running_data.edit_flag == 2) {
        // led_set_bk(80);
        tmos_stop_task(mTaskID, MCT_light_reset);
        tmos_start_task(mTaskID, MCT_light_reset, MS1_TO_SYSTEM_TIME(22 * 100));

        switch (btn_event_val) {
        case PRESS_DOWN:
            running_data.tmp_mac = index;
            PRINT("INDEX=%d\n", index);
        }
        if (num_of_keys() >= 3) {
            running_data.tmp_mac = 0xff;
        }
        if (num_of_keys() >= 4) {
            POWER_OFF;
            running_data.tmp_mac = 0xff;
        }
    }

    else if (running_data.edit_flag == 1) {
        // led_set_bk(80);
        tmos_stop_task(mTaskID, MCT_light_reset);
        tmos_start_task(mTaskID, MCT_light_reset, MS1_TO_SYSTEM_TIME(22 * 100));

        switch (btn_event_val) {
        case PRESS_DOWN:
            set_mode(index);
        }
        if (num_of_keys() >= 3) {
            set_mode(3);
        }
    } else if (running_data.mode_data == 0) {
        if (!btn_event_val) {
            data_in_fram.osu_mode_key_count[index]++;
        }
        switch (index) {
        case 0:
            keyboard_press_key_code_1(Keyboard_ESCAPE, !btn_event_val);
            break;
        case 1:
            keyboard_press_char_1('z', !btn_event_val);
            break;
        case 2:
            keyboard_press_char_1('x', !btn_event_val);
            break;
        case 3:
            // custom_press_key_1(MUTE, !btn_event_val);
            keyboard_press_key_code_1(HID_KEYBPAD_ADD, !btn_event_val);
            // mouse_set_continue_click(KEY_LEFT, !btn_event_val);
            break;
        }
    } else if (running_data.mode_data == 1) {
        user_defined_mode(index, !btn_event_val);
    } else if (running_data.mode_data == 2) {
        switch (index) {
        case 0:
            keyboard_press_sp_key_1(LEFT_CTRL, !btn_event_val);
            keyboard_press_char_1('s', !btn_event_val);
            // keyboard_press_char_1(' ', !btn_event_val);
            // keyboard_press_key_code_1(Keyboard_ESCAPE, !btn_event_val);
            break;
        case 1:
            keyboard_press_sp_key_1(LEFT_CTRL, !btn_event_val);
            keyboard_press_char_1('c', !btn_event_val);
            break;
        case 2:
            keyboard_press_sp_key_1(LEFT_CTRL, !btn_event_val);
            keyboard_press_char_1('v', !btn_event_val);
            break;
        case 3:
            keyboard_press_key_code_1(Keyboard_ESCAPE, !btn_event_val);
            // keyboard_press_char_1('z', !btn_event_val);
            break;
        }
        // if (num_of_keys() > 1)
        // {
        // 	// touch_set_continue_function(1);
        // 	mouse_set_continue_click(KEY_LEFT, 20);
        // 	keyboard_press_char_1('z', 0);
        // 	keyboard_press_char_1('x', 0);
        // 	keyboard_press_key_code_1(Keyboard_ESCAPE, 0);
        // }
        // else
        // {
        // 	// touch_set_continue_function(0);
        // 	mouse_set_continue_click(KEY_LEFT, 0);
        // }
    } else if (running_data.mode_data == 3) {
        mode3_mode[index] = !btn_event_val;
        switch (index) {
        case 0:
            keyboard_press_char_1(' ', !btn_event_val);
            break;
        case 1:
            // keyboard_press_char_1('a', !btn_event_val);
            break;
        case 2:
            // keyboard_press_char_1('s', !btn_event_val);
            break;
        case 3:
            if (btn_event_val != PRESS_DOWN)
                break;
            int have = 0;
            for (int i = 1; i < 3 && have == 0; i++) {
                if (mode3_mode[i]) {
                    have = 1;
                    switch (i) {
                    // case 0:
                    //   break;
                    case 1:
                        custom_press_key(MUTE, 1);
                        break;
                    case 2:
                        custom_press_key(PAUSE_PLAY, 1);
                        break;
                    }
                }
            }
            if (have == 0)
                keyboard_press_char('z', 1);

            break;
        }
        if (num_of_keys() > 1) {
            keyboard_press_key_code_1(Keyboard_ESCAPE, 1);
        } else {
            keyboard_press_key_code_1(Keyboard_ESCAPE, 0);
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
            // button_attach(keys__uz + i, PRESS_REPEAT, key44longcallback);
            // button_attach(keys__uz + i, SINGLE_CLICK, key44longcallback);
            // button_attach(keys__uz + i, DOUBLE_CLICK, key44longcallback);
            // button_attach(keys__uz + i, LONG_PRESS_START, key44longcallback);
            // button_attach(keys__uz + i, LONG_PRESS_HOLD, key44longcallback);
        }
        button_start(keys__uz + i);
    }
}
