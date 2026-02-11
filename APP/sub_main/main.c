#include "main.h"

// const uint8_t  usb_name[]   = {0x42, 0x30, 0x63, 0x30, 0x64, 0x30, 0x44, 0x30, 0x26, 0x20, 0x0D, 0x00, 0x0A,
//                                0x00, 0x91, 0x66, 0x4F, 0x30, 0x66, 0x30, 0x72, 0x5E, 0x4B, 0x30, 0x89, 0x30,
//                                0x73, 0x30, 0x5D, 0x30, 0x46, 0x30, 0x26, 0x20, 0x0D, 0x00, 0x0A, 0x00, 0xD5,
//                                0x52, 0x44, 0x30, 0x66, 0x30, 0x6A, 0x30, 0x44, 0x30, 0x6E, 0x30, 0x6B, 0x30,
//                                0x91, 0x66, 0x44, 0x30, 0x88, 0x30, 0x5E, 0xFF, 0x26, 0x20};
const uint8_t  usb_name[]   = {0x4B, 0x6D, 0xD5, 0x8B};
tmosTaskID     mTaskID      = INVALID_TASK_ID;
data_in_fram_s data_in_fram = {0};
running_data_s running_data;

tmosEvents MCT_ProcessEvent(tmosTaskID task_id, tmosEvents events);

void sub_main_1(void)
{
    //  ! read last shutdown data
    //  --------------------------------------------------------------------
    // ps("\nR8_RESET_STATUS 0x%02X\n", R8_RESET_STATUS);

    // if ((R8_RESET_STATUS & RB_RESET_FLAG) == RST_FLAG_RPOR) // �ϵ縴λ, �ػ�
    // {
    //     ps("\nRST_FLAG_RPOR, POWER OFF");

    //     // POWER_OFF;
    // }
    //  ! power en
    //  --------------------------------------------------------------------
    GPIOA_SetBits(GPIO_Pin_11);
    GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeOut_PP_5mA);
    fram_init();
    fram_read(0, &data_in_fram, sizeof(data_in_fram));
    running_data.mac_offset = data_in_fram._mac_offset;
    running_data.mode_data  = data_in_fram._mode_data;
    //  ! devices_init
    //  --------------------------------------------------------------------
    // mouse_init();
    keyboard_init();
    // touch_init();
    custom_init();
    void init_desp(void);
    init_desp();
    usb_set_name(usb_name, sizeof(usb_name));
    usb_set_desc(keyboard_state.report_pointer, keyboard_state.desp_lenth);
    usb_hid_kbd_init();

    // ! charging_detect
    // --------------------------------------------------------------------
    GPIOA_SetBits(GPIO_Pin_13);
    GPIOA_ModeCfg(GPIO_Pin_13, GPIO_ModeIN_PU);
#define IS_CHAEGING !GPIOA_ReadPortPin(GPIO_Pin_13)
    // ! usb_detect
    // --------------------------------------------------------------------
    GPIOA_SetBits(GPIO_Pin_14);
    GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeIN_PD);
#define HAVE_VUSB !!GPIOA_ReadPortPin(GPIO_Pin_14)
    running_data.power_off_timeout = 60 * 60;
    // ! BACK_LIGHT
    // --------------------------------------------------------------------
    GPIOPinRemap(DISABLE, RB_PIN_TMR0);
    GPIOA_ResetBits(GPIO_Pin_9); // ����PWM��
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    TMR0_PWMInit(High_Level, PWM_Times_1);
    TMR0_PWMCycleCfg(PWMCycleCfg); // ���� 100us

    TMR0_Disable();
    TMR0_PWMActDataWidth(PWMCycleCfg - 1); // �޸�ռ�ձȱ�����ʱ�رն�ʱ��
    TMR0_Enable();
    TMR0_PWMEnable();
    led_set_mode(0, SMOOTH_ALL, 0);
    led_set_bk(20);
    // ! ips --------------------------------------------------------------------
    char txts[10];
    GPIOB_SetBits(GPIO_Pin_17); // cs
    GPIOB_ModeCfg(GPIO_Pin_17, GPIO_ModeOut_PP_5mA);
    GPIOB_SetBits(GPIO_Pin_9);  // RST
    GPIOB_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    GPIOB_SetBits(GPIO_Pin_16); // RST
    GPIOB_ModeCfg(GPIO_Pin_16, GPIO_ModeOut_PP_5mA);

    GPIOPinRemap(ENABLE, RB_PIN_SPI0);
    GPIOB_ModeCfg(GPIO_Pin_15 | GPIO_Pin_13 | GPIO_Pin_14, GPIO_ModeOut_PP_5mA);
    SPI0_MasterDefInit();

    RST_RESET;
    DelayMs(5);
    RST_SET;
    DelayMs(1);
    LCD_CS_RESET;
    IPS_Write_Reg(0x11);
    IPS_Init();
    IPS_Clear(CYAN);
    set_mode(running_data.mode_data);
    sprintf(txts, "%d", running_data.mac_offset);
    IPS_ShowString(159 - 8 * 1, 0, txts, MAGENTA);
    IPS_ShowString(48, 0, "v", MAGENTA);
    LCD_CS_SET;
}
void sub_main(void)
{
    mTaskID = TMOS_ProcessEventRegister(MCT_ProcessEvent);
    tmos_start_task(mTaskID, MCT_light_control, MS1_TO_SYSTEM_TIME(50));
    tmos_start_task(mTaskID, MCT_test_event, MS1_TO_SYSTEM_TIME(100));
    tmos_start_task(mTaskID, MCT_POWER_OFF, MS1_TO_SYSTEM_TIME(999));

    // ! POWER ON --------------------------------------------------------------------
    LL_GPIO_SetOutputPin(GPIOA, GPIO_Pin_12);
    GPIO_SINGLE_INIT(GPIOA, GPIO_Pin_12, GPIO_ModeOut_PP_5mA);
    // ! GD25Q256 --------------------------------------------------------------------
    GPIOB_SetBits(GPIO_Pin_12); // cs
    GPIOB_ModeCfg(GPIO_Pin_12, GPIO_ModeOut_PP_5mA);
    // ! buzz --------------------------------------------------------------------
    buzzerDriverInit();
    buzzerSetNewFrequency(0);
    tmos_start_task(mTaskID, MCT_music_ticks, MS1_TO_SYSTEM_TIME(20));
    // start_music(0);
    // ! SW --------------------------------------------------------------------
    GPIOB_ModeCfg(GPIO_Pin_5, GPIO_ModeOut_PP_5mA);
    GPIOB_ModeCfg(GPIO_Pin_4, GPIO_Mode_IPU);
    // ! key --------------------------------------------------------------------
    GPIOB_ModeCfg(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    key_scan();
    my_button_init();
    tmos_start_task(mTaskID, MCT_key_scan, MS1_TO_SYSTEM_TIME(98));
    // ! hid_dev
    // --------------------------------------------------------------------
    tmos_start_task(mTaskID, MCT_event_update, MS1_TO_SYSTEM_TIME(56));
    // ps("%s, %d\n", __FILE__, __LINE__);
    // ! adc --------------------------------------------------------------------
    BP_ADC_Init();
    tmos_start_task(mTaskID, MCT_adc_measure, MS1_TO_SYSTEM_TIME(50));
    // ! tim5_ch3_ll_dma_init  --------------------------------------------------------------------
    // ws2812_power_pin
    // GPIO_SINGLE_INIT(GPIOB, NO_PIN, GPIO_ModeOut_PP_5mA);
    WS2812_POWER_OFF;
    GPIOAGPPCfg(DISABLE, RB_PIN_XT32K_IE);
    tim5_ch3_ll_dma_init();
    change_2812_state(2);
    tmos_start_task(mTaskID, MCT_WS2812_MODE, 100);
}
tmosEvents MCT_ProcessEvent(tmosTaskID task_id, tmosEvents events)
{
    uint8_t *msgPtr;
    char     txt[18];
    if (events
        & SYS_EVENT_MSG) { // ����HAL����Ϣ������tmos_msg_receive��ȡ��Ϣ��������ɺ�ɾ����Ϣ��?
        msgPtr = tmos_msg_receive(task_id);
        if (msgPtr) {
            /* De-allocate */
            tmos_msg_deallocate(msgPtr);
        }
        return events ^ SYS_EVENT_MSG;
    }
    if (events & MCT_test_event) {
        static int index_t_ = 0;
        // sprintf(txt, "%d", running_data.bt_connect_stat);
        // ws2812_list[0].rgb.red -= 10;
        // ws2812_list[0].rgb.green -= 5;

        // IPS_ShowString(48, 0, running_data.bt_connect_stat == 2 ? "OK " : "ing", MAGENTA);
        // if (running_data.mode_data == 0)
        //     sprintf(txt, "%5d", data_in_fram.osu_mode_key_count[index_t_]);
        // else
        //     sprintf(txt, "%5d", data_in_fram.key_count[index_t_]);
        // IPS_ShowString(112 + 4, 16 + index_t_ * 16, txt, MAGENTA);
        // index_t_++;
        // if (index_t_ >= 3)
        //     index_t_ = 0;
        // ps("key0 %d", data_in_fram.key_count[0]);
        tmos_start_task(mTaskID, MCT_test_event, MS1_TO_SYSTEM_TIME(10));
        return events ^ MCT_test_event;
    }
    if (events & MCT_light_control) {
        tmos_start_task(mTaskID, MCT_light_control, MS1_TO_SYSTEM_TIME(10));
        led_smooth_change_handle();
        return events ^ MCT_light_control;
    }
    if (events & MCT_key_scan) {
        static int t = 0;
        key_scan();
        // send_keyboard_data();
        // tmos_set_event(mTaskID, MCT_event_update);
        button_ticks();
        // t = !t;
        // send_custom_control_data();
        // if (t)
        //     encode_get();

        // tmos_set_event(mTaskID, MCT_event_update);
        tmos_start_task(mTaskID, MCT_key_scan, MS1_TO_SYSTEM_TIME(1));

        return events ^ MCT_key_scan;
    }
    if (events & MCT_event_update) {
        static int i = 0;
        static int t = 0;
        i++;
        switch (i) {
        case 1:
            send_keyboard_data();
            break;
        case 2:
            t = !t;
            send_custom_control_data();
            // if (t)
            //     encode_get();
            i = 0;

            break;
        case 3:
            // send_mouse_data();
            // ps("Zero \n");
            break;
            // case 4:
            //     break;
        }
        tmos_start_task(mTaskID, MCT_event_update, 3);
        return events ^ MCT_event_update;
    }
    if (events & MCT_adc_measure) {
        // tmos_start_task(mTaskID, MCT_adc_measure, MS1_TO_SYSTEM_TIME(2 * 60 *
        // 1000));
        tmos_start_task(mTaskID, MCT_adc_measure, MS1_TO_SYSTEM_TIME(60 * 1000));
        uint16_t rawa_adc          = read_vbat_adc();
        running_data.v_bat         = __Map(rawa_adc, 0, 2888, 0, 329);
        running_data.power_persent = __Map(running_data.v_bat, 320, 415, 0, 100);
        __LimitValue(running_data.power_persent, 0, 100);
        ps("adc %d\n", rawa_adc);
        ps("%% %d\n", running_data.power_persent);
        // if (tx_tmp <= (int)(__Map(3.4f, 0, 3.07f, 0, 3033)))
        //     running_data.low_power_flag = 1;
        if (running_data.v_bat > 450)
            ws2812_state.global_light = 90;
        else
            ws2812_state.global_light = 70;
        if (running_data.v_bat < 350 && running_data.v_bat > 325) {
            start_music(2);
        }
        if (running_data.v_bat < 325 && running_data.v_bat > 50) {
            ps("low_power\n");
            // POWER_OFF;
        }
        if (running_data.v_bat > 440) {
            running_data.power_off_timeout += 60;
        }
        sprintf(txt, "%1d.%02dV", running_data.v_bat / 100, running_data.v_bat % 100);
        ps("vbat = ");
        ps("%1d.%02dV", running_data.v_bat / 100, running_data.v_bat % 100);
        ps("\n");
        IPS_ShowString(0, 0, txt, MAGENTA);

        return events ^ MCT_adc_measure;
    }
    if (events & MCT_light_reset) {
        led_set_bk(1);
        return events ^ MCT_light_reset;
    }
    if (events & MCT_music_ticks) {
        tmos_start_task(mTaskID, MCT_music_ticks, MS1_TO_SYSTEM_TIME(20));
        music_ticks();
        return events ^ MCT_music_ticks;
    }
    if (events & MCT_POWER_OFF) {
        tmos_start_task(mTaskID, MCT_POWER_OFF, MS1_TO_SYSTEM_TIME(1000));
        ps("pfc %2d:%02ds\n", running_data.power_off_timeout / 60, running_data.power_off_timeout % 60);
        if (running_data.usb_is_connected == 0) {
            if (running_data.power_off_timeout) {
                // if (!IS_CHAEGING)
                running_data.power_off_timeout--;
                // fram_read(0, &data_in_fram, sizeof(data_in_fram));
                // for (int i = 0; i < 128; i++)
                // {
                //     ps("%02x ", *(((uint8_t *)&data_in_fram) + i));
                // }
                // ps("\n");
            } else {
                start_music(3);
                tmos_start_task(mTaskID, MCT_POWER_OFF_buzz, MS1_TO_SYSTEM_TIME(500));
                tmos_stop_task(mTaskID, MCT_POWER_OFF);
            }
        }
        return events ^ MCT_POWER_OFF;
    }
    if (events & MCT_POWER_OFF_buzz) {
        buzzerStop();
        POWER_OFF;
        return events ^ MCT_POWER_OFF_buzz;
    }
    if (events & MCT_WS2812_MODE) {
        tmos_start_task(mTaskID, MCT_WS2812_MODE, MS1_TO_SYSTEM_TIME(50));

        static uint8_t led_position = 0;
        static int8_t  direction    = 1; // 1 = forward, -1 = backward

        switch (running_data.ws2812_mode) {
        case WS2812_OFF:
            // Turn off all LEDs
            for (int i = 0; i < LED_NUM; i++) {
                ws2812_list[i].hex = 0;
            }
            break;

        case WS2812_SINGLE_MOVE:
            // Clear all LEDs first
            for (int i = 0; i < LED_NUM; i++) {
                ws2812_list[i].hex = 0;
            }

// Set main LED at full brightness
#define COLOR_HEX 0xff3333
            ws2812_list[led_position].hex       = COLOR_HEX;
            ws2812_list[led_position].rgb.alpha = 255;

            // Set adjacent LEDs at 60% brightness
            if (led_position > 0) {
                ws2812_list[led_position - 1].hex       = COLOR_HEX;
                ws2812_list[led_position - 1].rgb.alpha = 100; // 60% of 255
            }
            if (led_position < LED_NUM - 1) {
                ws2812_list[led_position + 1].hex       = COLOR_HEX;
                ws2812_list[led_position + 1].rgb.alpha = 100; // 60% of 255
            }

            // Set LEDs 2 positions away at 30% brightness
            if (led_position > 1) {
                ws2812_list[led_position - 2].hex       = COLOR_HEX;
                ws2812_list[led_position - 2].rgb.alpha = 30; // 30% of 255
            }
            if (led_position < LED_NUM - 2) {
                ws2812_list[led_position + 2].hex       = COLOR_HEX;
                ws2812_list[led_position + 2].rgb.alpha = 30; // 30% of 255
            }

            // Update position for next iteration
            led_position += direction;

            // Bounce back when reaching the end
            if (led_position >= LED_NUM - 1) {
                led_position = LED_NUM - 1;
                direction    = -1;
            } else if (led_position <= 0) {
                led_position = 0;
                direction    = 1;
            }
            break;

        case WS2812_RAINBOW_WAVE: {
            static uint8_t hue_offset = 0;

            // Create rainbow wave across all LEDs
            for (int i = 0; i < LED_NUM; i++) {
                // Calculate hue for this LED (0-255 range)
                // Spread rainbow across LEDs and add time-based offset for animation
                uint8_t hue = (i * 256 / LED_NUM + hue_offset) & 0xFF;

                // Simple HSV to RGB conversion (S=255, V=255)
                uint8_t region    = hue / 43; // 0-5
                uint8_t remainder = (hue - (region * 43)) * 6;

                uint8_t r = 0, g = 0, b = 0;

                switch (region) {
                case 0:
                    r = 255;
                    g = remainder;
                    b = 0;
                    break;
                case 1:
                    r = 255 - remainder;
                    g = 255;
                    b = 0;
                    break;
                case 2:
                    r = 0;
                    g = 255;
                    b = remainder;
                    break;
                case 3:
                    r = 0;
                    g = 255 - remainder;
                    b = 255;
                    break;
                case 4:
                    r = remainder;
                    g = 0;
                    b = 255;
                    break;
                default:
                    r = 255;
                    g = 0;
                    b = 255 - remainder;
                    break;
                }

                ws2812_list[i].rgb.red   = r;
                ws2812_list[i].rgb.green = g;
                ws2812_list[i].rgb.blue  = b;
                ws2812_list[i].rgb.alpha = 255; // Slightly dimmed for smoother look
            }

            // Advance the wave animation
            hue_offset -= 10;
        } break;

        case WS2812_BREATHING: {
            static uint8_t brightness = 0;
            static int8_t  fade_dir   = 1; // 1 = fade in, -1 = fade out

// Define breathing color (soft blue)
#define BREATH_COLOR_R 50
#define BREATH_COLOR_G 100
#define BREATH_COLOR_B 255

            // Set all LEDs to the same color with current brightness
            for (int i = 0; i < LED_NUM; i++) {
                ws2812_list[i].rgb.red   = BREATH_COLOR_R;
                ws2812_list[i].rgb.green = BREATH_COLOR_G;
                ws2812_list[i].rgb.blue  = BREATH_COLOR_B;
                ws2812_list[i].rgb.alpha = brightness;
            }

            // Update brightness for breathing effect
            brightness += fade_dir * 8;

            // Reverse direction at min/max brightness
            if (brightness >= 250) {
                brightness = 250;
                fade_dir   = -1;
            } else if (brightness <= 50) {
                brightness = 50;
                fade_dir   = 1;
            }
        } break;

        default:
            break;
        }
        for (int i = 0; i < LED_NUM; i++)
            update_bit(i);
        return events ^ MCT_WS2812_MODE;
    }
    return 0;
}
