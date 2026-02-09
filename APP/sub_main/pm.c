/********************************** (C) COPYRIGHT
 ******************************** File Name          : main.c Author : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *low power, standby mode routine:
 *WKUP(PA0)
 *This routine demonstrates that WFI enters the standby mode, the rising edge of
 *the WKUP (PA0) pin exits the standby mode,Program reset after wake-up.
 *Note: In order to reduce power consumption as much as possible, it is
 *recommended to set the unused GPIO to pull-down mode.
 *
 */

#include "main.h"

/* Global define */

void power_reset(uint8_t data_to_save_in_reg)
{
    save_all_data_to_fram();
    SYS_ResetKeepBuf(data_to_save_in_reg);
    SYS_ResetExecute();
}
/* Global Variable */
// __HIGH_CODE
void power_deep_sleep(void)
{
    //    /* 配置唤醒源为 GPIO - PA5 */
    //    save_all_data_to_fram();
    //    GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_Floating);
    //    GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_Floating);
    //    tim5_ch3_ll_dma_deinit();
    //    deinit_all_pin();
    //    PWR_PeriphClkCfg(DISABLE, 0x0000FFFF);
    //    // GPIOB_ModeCfg(GPIO_Pin_3, GPIO_ModeOut_PP_5mA);
    //    // LL_GPIO_ResetOutputPin(GPIOB, GPIO_Pin_3);
    //    // DelayMs(20);
    //    GPIOB_ModeCfg(GPIO_Pin_3, GPIO_ModeIN_Floating);
    //    // SetSysClock(CLK_SOURCE_HSE_1MHz);
    //    DelayMs(20);
    //    // while (!GPIOB_ReadPortPin(GPIO_Pin_3))
    //    //     ;
    //    // GPIOB_ModeCfg(GPIO_Pin_3, GPIO_ITMode_FallEdge); // 下降沿唤醒
    //    // PFIC_EnableIRQ(GPIO_B_IRQn);
    //    // PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_GPIO_WAKE, Long_Delay);
    //    // PRINT("IDLE mode sleep \n");
    //    // DelayMs(1);
    //    // LowPower_Idle();
    //    // PRINT("wake.. \n");
    //    // DelayMs(20);
    //
    //    // SetSysClock(CLK_SOURCE_PLL_48MHz);
    //    while (GPIOB_ReadPortPin(GPIO_Pin_3))
    //        DelayMs(50);
    //    // ;
    //    // DelayMs(2);
    //    // DelayMs(2);
    //    // DelayMs(2);
    //    GPIOB_ITModeCfg(GPIO_Pin_3, GPIO_ITMode_RiseEdge);
    //    DelayMs(100);
    //
    //    PFIC_EnableIRQ(GPIO_B_IRQn);
    //    PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_GPIO_WAKE, Long_Delay);
    //
    //    // PRINT("shut down mode sleep \n           ");
    //    // DelayMs(2);
    //    LowPower_Shutdown(0); // 全部断电，唤醒后复位
    //    /*
    //     此模式唤醒后会执行复位，所以下面代码不会运行，
    //     注意要确保系统睡下去再唤醒才是唤醒复位，否则有可能变成IDLE等级唤醒
    //     */
    //    HSECFG_Current(HSE_RCur_100); //
    //    降为额定电流(低功耗函数中提升了HSE偏置电流)
    //    // PRINT("wake.. \n");
    //    // DelayMs(500);
    //    power_deep_sleep();
}
//__INTERRUPT
//__HIGH_CODE
// void GPIOB_IRQHandler(void)
//{
//    GPIOB_ClearITFlagBit(GPIO_Pin_3);
//}
