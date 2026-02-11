#include "psk_ws2812.h"
// #include "tim.h"
#include "string.h"

__attribute__((aligned(4))) size_tmp ws2812_bittmp_all[24 * LED_NUM + RESET_PULSE] = {0};

// rgb
color_t ws2812_list[LED_NUM] = {
    // 0x0ff,     // 0
    // 0x0ff00,   // 1
    // 0x0ff0000, // 2
    // 0x0ff2452, // 3
    // 0xACFFE6,  // 4
    // 0x0,       // 5
    // 0x0,       // 6
    // 0x0ffffff, // 7
    0,
};
color_t        list_last[LED_NUM] = {0};
ws2812_state_t ws2812_state       = {0, 0, 0, 30};

// __HIGH_CODE
void update_bit(uint16_t index)
{
    static uint8_t last_global = 0;
    if (ws2812_list[index].hex == list_last[index].hex && last_global == ws2812_state.global_light) {
        // if (ws2812_list[index].rgb.alpha == 0)
        //     ws2812_state.zero_light_count++;
        return;
    } else {
        color_t   t = ws2812_list[index];
        uint16_t  tmp, tmpp_light;
        size_tmp *data_pos_ = ws2812_bittmp_all + 24 * index;

        list_last[index].hex = ws2812_list[index].hex;
        if (ws2812_state.cur_led == 0)
            last_global = ws2812_state.global_light;

        // PRINT("%d\n", index);

        // if (ws2812_state.global_light)
        // {
        //   tmpp_light = ws2812_state.global_light;
        // }
        // else
        if (t.rgb.alpha) {
            tmpp_light = (t.rgb.alpha * ws2812_state.global_light) >> 8;
        } else if (ws2812_state.global_light) {
            tmpp_light = ws2812_state.global_light;
        } else {
            goto no_set_light;
        }
        tmp = t.rgb.red;
        tmp = tmp * tmpp_light;
        tmp >>= 8;
        t.rgb.red = tmp;
        tmp       = t.rgb.blue;
        tmp       = tmp * tmpp_light;
        tmp >>= 8;
        t.rgb.blue = tmp;
        tmp        = t.rgb.green;
        tmp        = tmp * tmpp_light;
        tmp >>= 8;
        t.rgb.green = tmp;

    no_set_light:

        for (size_tmp *i = data_pos_ + 23; i >= data_pos_ + 16; i--) {
            *i = (t.hex) & (0x01) ? ONE_PULSE : ZERO_PULSE;
            t.hex >>= 1;
        }
        for (size_tmp *i = data_pos_ + 7; i >= data_pos_ + 0; i--) {
            *i = (t.hex) & (0x01) ? ONE_PULSE : ZERO_PULSE;
            t.hex >>= 1;
        }
        for (size_tmp *i = data_pos_ + 15; i >= data_pos_ + 8; i--) {
            *i = (t.hex) & (0x01) ? ONE_PULSE : ZERO_PULSE;
            t.hex >>= 1;
        }
    }
}
void force_update_bit(uint16_t index)
{
    color_t   t = ws2812_list[index];
    uint16_t  tmp, tmpp_light;
    size_tmp *data_pos_ = ws2812_bittmp_all + 24 * index;

    list_last[index].hex = ws2812_list[index].hex;

    if (ws2812_state.global_light) {
        tmpp_light = ws2812_state.global_light;
    } else if (t.rgb.alpha) {
        tmpp_light = t.rgb.alpha;
    } else {
        goto no_set_light;
    }
    tmp = t.rgb.red;
    tmp = tmp * tmpp_light;
    tmp >>= 8;
    t.rgb.red = tmp;
    tmp       = t.rgb.blue;
    tmp       = tmp * tmpp_light;
    tmp >>= 8;
    t.rgb.blue = tmp;
    tmp        = t.rgb.green;
    tmp        = tmp * tmpp_light;
    tmp >>= 8;
    t.rgb.green = tmp;

no_set_light:
    for (size_tmp *i = data_pos_ + 23; i >= data_pos_ + 16; i--) {
        *i = (t.hex) & (0x01) ? ONE_PULSE : ZERO_PULSE;
        t.hex >>= 1;
    }
    for (size_tmp *i = data_pos_ + 7; i >= data_pos_ + 0; i--) {
        *i = (t.hex) & (0x01) ? ONE_PULSE : ZERO_PULSE;
        t.hex >>= 1;
    }
    for (size_tmp *i = data_pos_ + 15; i >= data_pos_ + 8; i--) {
        *i = (t.hex) & (0x01) ? ONE_PULSE : ZERO_PULSE;
        t.hex >>= 1;
    }
}
#ifdef BIT_TRANSFER

void tim5_ch3_ll_init(void)
{
    LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH3);
    LL_TIM_EnableIT_UPDATE(TIM5);
    LL_TIM_EnableCounter(TIM5);
}
uint16_t get_current_bit(void)
{
    uint16_t tmp = 0;
    if (ws2812_state.reseting_time) {
        tmp = 0;
        ws2812_state.reseting_time--;
    }
    // else if (ws2812_state.cur_bit > 23)
    // {
    //   tmp = 0;
    // }
    else {
        // if (ws2812_state.cur_bit == 0 && ws2812_state.cur_led == 0)
        if (ws2812_state.cur_bit == 0) {
            TIM5->CCR3 = 0x0;
            LL_TIM_DisableCounter(TIM5);
            calc_bit(ws2812_state.cur_led);
            LL_TIM_EnableCounter(TIM5);
        }
        tmp = ws2812_bittmp[ws2812_state.cur_bit];
        ws2812_state.cur_bit++;
        if (ws2812_state.cur_bit == 24) {
            ws2812_state.cur_bit = 0;
            // ws2812_state.reseting_time = 2;
            ws2812_state.cur_led++;

            if (ws2812_state.cur_led >= LED_NUM) {
                ws2812_state.cur_led       = 0;
                ws2812_state.reseting_time = RESET_PULSE;
            }
            // else
            // {
            //   calc_bit(ws2812_state.cur_led);
            // }
        }
    }
    return tmp;
}

#endif
#ifdef HAL_DMA_PWM_transfer

void tim5_ch3_hal_dma_init(void)
{
    HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_3, (uint32_t *) ws2812_bittmp, sizeof(ws2812_bittmp) / sizeof(size_tmp));
}

// PWM DMA ?????????????
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim5) {
        TIM5->CCR3 = 0x0;
        HAL_TIM_PWM_Stop_DMA(&htim5, TIM_CHANNEL_3);
        // LL_TIM_DisableCounter(TIM5);
        if (ws2812_state.reseting_time) {
            if (ws2812_state.reseting_time > 0) {
                ws2812_state.reseting_time--;
                memset(ws2812_bittmp, 0x0, sizeof(ws2812_bittmp));
            }
        } else {
            calc_bit(ws2812_state.cur_led);
            ws2812_state.cur_led++;
            if (ws2812_state.cur_led >= LED_NUM) {
                ws2812_state.cur_led       = 0;
                ws2812_state.reseting_time = RESET_PULSE / 24 + 1;
            }
        }
        // LL_TIM_EnableCounter(TIM5);
        HAL_TIM_PWM_Start_DMA(&htim5,
                              TIM_CHANNEL_3,
                              (uint32_t *) ws2812_bittmp,
                              sizeof(ws2812_bittmp) / sizeof(size_tmp));
    }
}

#endif
#ifdef LL_DMA_PWM_transfer
void tim5_ch3_ll_dma_init(void)
{
    for (int i = 0; i < LED_NUM; i++) {
        force_update_bit(i);
    }
    // ws2812_state.global_light = 255;
    GPIOA_ModeCfg(GPIO_Pin_11, GPIO_ModeOut_PP_5mA);
    GPIOPinRemap(DISABLE, RB_PIN_TMR2);
    // PRINT("TMR2 DMA PWM\n");

    TMR2_PWMCycleCfg(75); // ?? 1.25us
                          // TMR2
                          // calc_bit(0);
    TMR2_PWMInit(High_Level, PWM_Times_1);

    TMR2_DMACfg(ENABLE,
                (uint16_t) (uint32_t) &ws2812_bittmp_all[0],
                (uint16_t) (uint32_t) &ws2812_bittmp_all[24 * LED_NUM + RESET_PULSE - 1],
                Mode_Single);
    TMR2_PWMEnable();
    TMR2_Enable();

    TMR2_ClearITFlag(TMR1_2_IT_DMA_END);
    // TMR2_ITCfg(ENABLE, TMR1_2_IT_DMA_END);
    // PFIC_EnableIRQ(TMR2_IRQn);

    // TMR2_ClearITFlag(TMR1_2_IT_DMA_END);

    // PFIC_EnableIRQ(TMR2_IRQn);

    // TMR2_ITCfg(ENABLE, TMR1_2_IT_DMA_END);

    // TMR3_TimerInit(360000 / 2);
    // TMR3_ITCfg(ENABLE, TMR0_3_IT_CYC_END);
    // PFIC_EnableIRQ(TMR3_IRQn);
}
void update_once(void)
{
    TMR2_ClearITFlag(TMR1_2_IT_DMA_END);
    TMR2_DMACfg(DISABLE,
                (uint16_t) (uint32_t) &ws2812_bittmp_all[0],
                (uint16_t) (uint32_t) &ws2812_bittmp_all[35],
                Mode_Single);

    TMR2_DMACfg(ENABLE,
                (uint16_t) (uint32_t) &ws2812_bittmp_all[0],
                (uint16_t) (uint32_t) &ws2812_bittmp_all[24 * LED_NUM + RESET_PULSE - 1],
                Mode_Single);
    TMR2_PWMEnable();
    TMR2_Enable();
}
__INTERRUPT
__HIGH_CODE
void TMR2_IRQHandler(void)
{
    if (TMR2_GetITFlag(TMR1_2_IT_DMA_END)) {
        TMR2_ClearITFlag(TMR1_2_IT_DMA_END);
        // // GPIOA_SetBits(GPIO_Pin_0);
        // // TMR2_Disable();
        // // TMR2_PWMActDataWidth(0); // ??? 50%, ??????????????
        // TMR2_DMACfg(DISABLE, (uint16_t)(uint32_t)&ws2812_bittmp[0],
        // (uint16_t)(uint32_t)&ws2812_bittmp[35], Mode_Single);
        // // TMR2_Enable();

        // // PRINT("int\n");
        // // LL_DMA_ClearFlag_TC0(DMA1);
        // // LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_0);
        // // LL_TIM_DisableCounter(TIM5);
        // if (ws2812_state.reseting_time)
        // {
        //   if (ws2812_state.reseting_time > 0)
        //   {
        //     ws2812_state.reseting_time--;
        //     if (ws2812_state.reseting_time == RESET_PULSE / 24)
        //       memset(ws2812_bittmp + 2, 0x0, sizeof(ws2812_bittmp));
        //     // memset(ws2812_bittmp + 2, 0x0, sizeof(ws2812_bittmp));

        //     // PRINT("Res\n");
        //   }
        // }
        // else
        // {
        //   calc_bit(ws2812_state.cur_led);
        //   ws2812_state.cur_led++;
        //   if (ws2812_state.cur_led >= LED_NUM)
        //   {
        //     ws2812_state.cur_led = 0;
        //     ws2812_state.reseting_time = RESET_PULSE / 24 + 1;
        //   }
        // }
        // // GPIOA_ResetBits(GPIO_Pin_0);

        // // TMR2_Enable();
        // TMR2_DMACfg(ENABLE, (uint16_t)(uint32_t)&ws2812_bittmp[0],
        // (uint16_t)(uint32_t)&ws2812_bittmp[31], Mode_LOOP);
        // // TMR2_Enable();
    }
    // LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_0, (uint32_t)ws2812_bittmp);
    // LL_TIM_EnableCounter(TIM5);
    // LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
}
void tim5_ch3_ll_dma_deinit(void)
{
    TMR2_DMACfg(DISABLE,
                (uint16_t) (uint32_t) &ws2812_bittmp_all[0],
                (uint16_t) (uint32_t) &ws2812_bittmp_all[35],
                Mode_Single);
    WS2812_OFF;

    // LL_TIM_DisableCounter(TIM5);
    // LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_0);
    // LL_TIM_DisableDMAReq_CC3(TIM5);
}

void change_2812_state(int x)
{
    if (ws2812_state.ws2812_power_state != x) {
        if (x == 0 || x == 1) {
            if (ws2812_state.ws2812_power_state == 2) {
                WS2812_POWER_OFF;
                // PRINT("L_OFF\n");
                tim5_ch3_ll_dma_deinit();
                TMR2_PWMDisable();
                TMR2_Disable();
                TMR2_PWMActDataWidth(0);
            }
        }
        if (x == 2) {
            WS2812_POWER_ON;
            TMR2_PWMEnable();
            TMR2_Enable();
            // PRINT("L_ON\n");
            // tim5_ch3_ll_dma_init();
            TMR2_DMACfg(ENABLE,
                        (uint16_t) (uint32_t) &ws2812_bittmp_all[0],
                        (uint16_t) (uint32_t) &ws2812_bittmp_all[24 * LED_NUM + RESET_PULSE - 1],
                        Mode_LOOP);
        }
    }
    ws2812_state.ws2812_power_state = x;
    // PRINT("ws%d\n", ws2812_state.ws2812_power_state);
}

__attribute__((interrupt("WCH-Interrupt-fast"))) __attribute__((section(".highcode"))) void TMR3_IRQHandler(
    void) // TMR0 ????
{
    if (TMR3_GetITFlag(TMR0_3_IT_CYC_END)) {
        TMR3_ClearITFlag(TMR0_3_IT_CYC_END); // ??????
        update_bit(ws2812_state.cur_led);
        ws2812_state.cur_led++;
        if (ws2812_state.cur_led >= LED_NUM) {
            if (ws2812_state.zero_light_count >= LED_NUM) {
                change_2812_state(1);
            } else {
                change_2812_state(2);
            }
            ws2812_state.zero_light_count = 0;
            ws2812_state.cur_led          = 0;
        }
    }
}

#endif
