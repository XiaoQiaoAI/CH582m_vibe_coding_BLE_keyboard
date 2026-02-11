/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2022/01/25
 * Description        : 模拟USB复合设备，键鼠，支持类命令
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
// 庆历四年春，滕子京谪守巴陵郡。越明年，政通人和，百废具兴，乃重修岳阳楼，增其旧制，刻唐贤今人诗赋于其上，属予作文以记之。(具 通：俱)
// 予观夫巴陵胜状，在洞庭一湖。衔远山，吞长江，浩浩汤汤，横无际涯，朝晖夕阴，气象万千，此则岳阳楼之大观也，前人之述备矣。然则北通巫峡，南极潇湘，迁客骚人，多会于此，览物之情，得无异乎？若夫淫雨霏霏，连月不开，阴风怒号，浊浪排空，日星隐曜，山岳潜形，商旅不行，樯倾楫摧，薄暮冥冥，虎啸猿啼。登斯楼也，则有去国怀乡，忧谗畏讥，满目萧然，感极而悲者矣。(隐曜 一作：隐耀；淫雨 通：霪雨) 至若春和景明，波澜不惊，上下天光，一碧万顷，沙鸥翔集，锦鳞游泳，岸芷汀兰，郁郁青青。而或长烟一空，皓月千里，浮光跃金，静影沉璧，渔歌互答，此乐何极！登斯楼也，则有心旷神怡，宠辱偕忘，把酒临风，其喜洋洋者矣。嗟夫！予尝求古仁人之心，或异二者之为，何哉？不以物喜，不以己悲，居庙堂之高则忧其民，处江湖之远则忧其君。是进亦忧，退亦忧。然则何时而乐耶？其必曰“先天下之忧而忧，后天下之乐而乐”乎！噫！微斯人，吾谁与归？时六年九月十五日。
#include "CH58x_common.h"

#define DevEP0SIZE              0x40

// 支持的最大接口数量
#define USB_INTERFACE_MAX_NUM   2
// 接口号的最大值
#define USB_INTERFACE_MAX_INDEX 1
#define USBD_MAX_POWER          (300 / 2) /* 100 mA */
#define CUSTOM_HID_FS_BINTERVAL 0x1U
// clang-format off

// const uint8_t MouseRepDesc[] = {0x05, 0x01, 0x09, 0x02, 0xA1, 0x01, 0x09, 0x01, 0xA1, 0x00, 0x05, 0x09, 0x19,
//                                 0x01, 0x29, 0x03, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x03, 0x81, 0x02,
//                                 0x75, 0x05, 0x95, 0x01, 0x81, 0x01, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x09,
//                                 0x38, 0x15, 0x81, 0x25, 0x7f, 0x75, 0x08, 0x95, 0x03, 0x81, 0x06, 0xC0, 0xC0};
const uint8_t MouseRepDesc[] = {
    0x05, 1,  // Usage Page (Generic Desktop Ctrls)
    0x09, 0x00, // Usage (Undefined)
    0xA1, 0x00, // Collection (Physical)
    // 0x85, 10,   // Report ID (8)
    0x05, 0x01, //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x30, //   Usage (X)
    0x15, 0x00, //   Logical Minimum (0)
    0x25, 0xFF, //   Logical Maximum (-1)
    0x75, 0x08, //   Report Size (8)
    0x95, 64,   //   Report Count (64)
    0x81, 0x02, //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01, //   Usage Page (Generic Desktop Ctrls)
    0x09, 0x31, //   Usage (Y)
    0x15, 0x00, //   Logical Minimum (0)
    0x25, 0xFF, //   Logical Maximum (-1)
    0x75, 0x08, //   Report Size (8)
    0x95, 64,   //   Report Count (64)
    0x91, 0x02, //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,       // End Collection
};

// 设备描述符
// DevEP0SIZE next is vid pid lower byte first
const uint8_t MyDevDescr[]
    = {0x12, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, DevEP0SIZE, 0x3c, 0x41, 0x07, 0x21, 0x00, 0x00, 0x01, 0x02, 0x00, 0x01};
// 配置描述符

static uint8_t MyCfgDescr[] = {
    0x09, 0x02,           0x3b, 0x00, 0x02,       0x01, 0x00,
    0xA0, USBD_MAX_POWER,                                                        // 配置描述符
    0x09, 0x04,           0x00, 0x00, 0x01,       0x03, 0x01,
    0x01, 0x00,                                                                  // 接口描述符,键盘
    0x09, 0x21,           0x11, 0x01, 0x00,       0x01, 0x22,
    0x3e, 0x00,                                                                  // HID类描述符
    0x07, 0x05,           0x81, 0x03, DevEP0SIZE, 0x00, CUSTOM_HID_FS_BINTERVAL, // 端点描述符
    0x09, 0x04,           0x01, 0x00, 0x01,       0x03, 0x01,
    0x02, 0x00,                                                                  // 接口描述符,鼠标
    0x09, 0x21,           0x10, 0x01, 0x00,       0x01, 0x22,
    sizeof(MouseRepDesc),   0x00,                                                // HID类描述符
    0x07, 0x05,           0x82, 0x03, 0x04,       0x00, CUSTOM_HID_FS_BINTERVAL  // 端点描述符
};
// clang-format on

/* USB速度匹配描述符 */
const uint8_t My_QueDescr[] = {0x0A, 0x06, 0x00, 0x02, 0xFF, 0x00, 0xFF, 0x40, 0x01, 0x00};

/* USB全速模式,其他速度配置描述符 */
uint8_t USB_FS_OSC_DESC[sizeof(MyCfgDescr)] = {
    0x09, 0x07, /* 其他部分通过程序复制 */
};

// 语言描述符
const uint8_t MyLangDescr[] = {0x04, 0x03, 0x09, 0x04};
// 厂家信息
const uint8_t MyManuInfo[] = {22,   0x03, 0x84, 0x30, 0xFC, 0x30, 0x48, 0x51, 0x1F, 0x75, 0x01,
                              0x30, 0x4A, 0x30, 0xB2, 0x75, 0x8C, 0x30, 0xD8, 0x69, 0xFC, 0x30};
// 产品信息
uint8_t MyProdInfo[254] = {254,  0x03, 0xD8, 0x8F, 0xEF, 0x53, 0xE5, 0x4E, 0x85, 0x8D, 0x7F, 0x95, 0x0d, 0x00, 0x0a,
                           0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00,
                           0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d,
                           0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00,
                           0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a,
                           0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00,
                           0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d,
                           0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00,
                           0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a,
                           0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00,
                           0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, 0x00, 0x0d, 0x00, 0x0a, '6',  0x00};
/*HID类报表描述符*/
#define asdgbhbhijoasgd sizeof(MyProdInfo)

uint8_t *KeyRepDesc;
uint8_t  KeyRepDesc_length;

/**********************************************************/
uint8_t        DevConfig, Ready;
uint8_t        SetupReqCode;
uint16_t       SetupReqLen;
const uint8_t *pDescr;
uint8_t        Report_Value[USB_INTERFACE_MAX_INDEX + 1] = {0x00};
uint8_t        Idle_Value[USB_INTERFACE_MAX_INDEX + 1]   = {0x00};
uint8_t        USB_SleepStatus                           = 0x00; /* USB睡眠状态 */

/*鼠标键盘数据*/
// uint8_t HIDMouse[4] = {0x0, 0x0, 0x0, 0x0};
// uint8_t HIDKey[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
/******** 用户自定义分配端点RAM ****************************************/
__attribute__((aligned(4))) uint8_t EP0_Databuf[64 + 64 + 64]; // ep0(64)+ep4_out(64)+ep4_in(64)
__attribute__((aligned(4))) uint8_t EP1_Databuf[64 + 64];      // ep1_out(64)+ep1_in(64)
__attribute__((aligned(4))) uint8_t EP2_Databuf[64 + 64];      // ep2_out(64)+ep2_in(64)
__attribute__((aligned(4))) uint8_t EP3_Databuf[64 + 64];      // ep3_out(64)+ep3_in(64)

/*********************************************************************
 * @fn      USB_DevTransProcess
 *
 * @brief   USB 传输处理函数
 *
 * @return  none
 */
void USB_DevTransProcess(void)
{
    uint8_t len, chtype;
    uint8_t intflag, errflag = 0;

    intflag = R8_USB_INT_FG;
    if (0x6C == intflag)
        usb_disconnect();

    if (intflag & RB_UIF_TRANSFER) {
        if ((R8_USB_INT_ST & MASK_UIS_TOKEN) != MASK_UIS_TOKEN) // 非空闲
        {
            switch (R8_USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
            // 分析操作令牌和端点号
            {
            case UIS_TOKEN_IN: {
                switch (SetupReqCode) {
                case USB_GET_DESCRIPTOR:
                    len = SetupReqLen >= DevEP0SIZE ? DevEP0SIZE : SetupReqLen; // 本次传输长度
                    memcpy(pEP0_DataBuf, pDescr, len);                          /* 加载上传数据 */
                    SetupReqLen -= len;
                    pDescr += len;
                    R8_UEP0_T_LEN = len;
                    R8_UEP0_CTRL ^= RB_UEP_T_TOG; // 翻转
                    break;
                case USB_SET_ADDRESS:
                    R8_USB_DEV_AD = (R8_USB_DEV_AD & RB_UDA_GP_BIT) | SetupReqLen;
                    R8_UEP0_CTRL  = UEP_R_RES_ACK | UEP_T_RES_NAK;
                    break;

                case USB_SET_FEATURE:
                    break;

                default:
                    R8_UEP0_T_LEN = 0; // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                    R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                    break;
                }
            } break;

            case UIS_TOKEN_OUT: {
                len = R8_USB_RX_LEN;
                if (SetupReqCode == 0x09) {
                    PRINT("%x, len %d\n", R8_USB_INT_ST, len);
                    usb_data_received(pEP0_DataBuf, len);
                    PRINT("[%s] Num Lock\t", (pEP0_DataBuf[0] & (1 << 0)) ? "*" : " ");
                    PRINT("[%s] Caps Lock\t", (pEP0_DataBuf[0] & (1 << 1)) ? "*" : " ");
                    PRINT("[%s] Scroll Lock\n", (pEP0_DataBuf[0] & (1 << 2)) ? "*" : " ");
                }
            } break;

            case UIS_TOKEN_OUT | 1: {
                if (R8_USB_INT_ST & RB_UIS_TOG_OK) { // 不同步的数据包将丢弃
                    R8_UEP1_CTRL ^= RB_UEP_R_TOG;
                    len = R8_USB_RX_LEN;
                    DevEP1_OUT_Deal(len);
                }
            } break;

            case UIS_TOKEN_IN | 1:
                R8_UEP1_CTRL ^= RB_UEP_T_TOG;
                R8_UEP1_CTRL = (R8_UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                break;

            case UIS_TOKEN_OUT | 2: {
                if (R8_USB_INT_ST & RB_UIS_TOG_OK) { // 不同步的数据包将丢弃
                    R8_UEP2_CTRL ^= RB_UEP_R_TOG;
                    len = R8_USB_RX_LEN;
                    DevEP2_OUT_Deal(len);
                }
            } break;

            case UIS_TOKEN_IN | 2:
                R8_UEP2_CTRL ^= RB_UEP_T_TOG;
                R8_UEP2_CTRL = (R8_UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                break;

            case UIS_TOKEN_OUT | 3: {
                if (R8_USB_INT_ST & RB_UIS_TOG_OK) { // 不同步的数据包将丢弃
                    R8_UEP3_CTRL ^= RB_UEP_R_TOG;
                    len = R8_USB_RX_LEN;
                    DevEP3_OUT_Deal(len);
                }
            } break;

            case UIS_TOKEN_IN | 3:
                R8_UEP3_CTRL ^= RB_UEP_T_TOG;
                R8_UEP3_CTRL = (R8_UEP3_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                break;

            case UIS_TOKEN_OUT | 4: {
                if (R8_USB_INT_ST & RB_UIS_TOG_OK) {
                    R8_UEP4_CTRL ^= RB_UEP_R_TOG;
                    len = R8_USB_RX_LEN;
                    DevEP4_OUT_Deal(len);
                }
            } break;

            case UIS_TOKEN_IN | 4:
                R8_UEP4_CTRL ^= RB_UEP_T_TOG;
                R8_UEP4_CTRL = (R8_UEP4_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                break;

            default:
                break;
            }
            R8_USB_INT_FG = RB_UIF_TRANSFER;
        }
        if (R8_USB_INT_ST & RB_UIS_SETUP_ACT) // Setup包处理
        {
            R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
            SetupReqLen  = pSetupReqPak->wLength;
            SetupReqCode = pSetupReqPak->bRequest;
            chtype       = pSetupReqPak->bRequestType;

            len     = 0;
            errflag = 0;
            if ((pSetupReqPak->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) {
                /* 非标准请求 */
                /* 其它请求,如类请求，产商请求等 */
                if (pSetupReqPak->bRequestType & 0x40) {
                    /* 厂商请求 */
                } else if (pSetupReqPak->bRequestType & 0x20) {
                    switch (SetupReqCode) {
                    case DEF_USB_SET_IDLE: /* 0x0A: SET_IDLE */ // 主机想设置HID设备特定输入报表的空闲时间间隔
                        Idle_Value[pSetupReqPak->wIndex] = (uint8_t) (pSetupReqPak->wValue >> 8);
                        break;                                  // 这个一定要有

                    case DEF_USB_SET_REPORT: /* 0x09: SET_REPORT */ // 主机想设置HID设备的报表描述符
                        break;

                    case DEF_USB_SET_PROTOCOL: /* 0x0B: SET_PROTOCOL */ // 主机想设置HID设备当前所使用的协议
                        Report_Value[pSetupReqPak->wIndex] = (uint8_t) (pSetupReqPak->wValue);
                        break;

                    case DEF_USB_GET_IDLE: /* 0x02: GET_IDLE */ // 主机想读取HID设备特定输入报表的当前的空闲比率
                        EP0_Databuf[0] = Idle_Value[pSetupReqPak->wIndex];
                        len            = 1;
                        break;

                    case DEF_USB_GET_PROTOCOL: /* 0x03: GET_PROTOCOL */ // 主机想获得HID设备当前所使用的协议
                        EP0_Databuf[0] = Report_Value[pSetupReqPak->wIndex];
                        len            = 1;
                        break;

                    default:
                        errflag = 0xFF;
                    }
                }
            } else /* 标准请求 */
            {
                switch (SetupReqCode) {
                case USB_GET_DESCRIPTOR: {
                    switch (((pSetupReqPak->wValue) >> 8)) {
                    case USB_DESCR_TYP_DEVICE: {
                        pDescr = MyDevDescr;
                        len    = MyDevDescr[0];
                    } break;

                    case USB_DESCR_TYP_CONFIG: {
                        pDescr = MyCfgDescr;
                        len    = MyCfgDescr[2];
                    } break;

                    case USB_DESCR_TYP_HID:
                        switch ((pSetupReqPak->wIndex) & 0xff) {
                        /* 选择接口 */
                        case 0:
                            pDescr = (uint8_t *) (&MyCfgDescr[18]);
                            len    = 9;
                            break;

                        case 1:
                            pDescr = (uint8_t *) (&MyCfgDescr[43]);
                            len    = 9;
                            break;

                        default:
                            /* 不支持的字符串描述符 */
                            errflag = 0xff;
                            break;
                        }
                        break;

                    case USB_DESCR_TYP_REPORT: {
                        if (((pSetupReqPak->wIndex) & 0xff) == 0)        // 接口0报表描述符
                        {
                            pDescr = KeyRepDesc;                         // 数据准备上传
                            len    = KeyRepDesc_length;
                        } else if (((pSetupReqPak->wIndex) & 0xff) == 1) // 接口1报表描述符
                        {
                            pDescr = MouseRepDesc;                       // 数据准备上传
                            len    = sizeof(MouseRepDesc);
                            Ready = 1; // 如果有更多接口，该标准位应该在最后一个接口配置完成后有效
                        } else
                            len = 0xff; // 本程序只有2个接口，这句话正常不可能执行
                    } break;

                    case USB_DESCR_TYP_STRING: {
                        switch ((pSetupReqPak->wValue) & 0xff) {
                        case 1:
                            pDescr = MyManuInfo;
                            len    = MyManuInfo[0];
                            break;
                        case 2:
                            pDescr = MyProdInfo;
                            len    = MyProdInfo[0];
                            break;
                        case 0:
                            pDescr = MyLangDescr;
                            len    = MyLangDescr[0];
                            break;
                        default:
                            errflag = 0xFF; // 不支持的字符串描述符
                            break;
                        }
                    } break;

                    case 0x06:
                        pDescr = (uint8_t *) (&My_QueDescr[0]);
                        len    = sizeof(My_QueDescr);
                        break;

                    case 0x07:
                        memcpy(&USB_FS_OSC_DESC[2], &MyCfgDescr[2], sizeof(MyCfgDescr) - 2);
                        pDescr = (uint8_t *) (&USB_FS_OSC_DESC[0]);
                        len    = sizeof(USB_FS_OSC_DESC);
                        break;

                    default:
                        errflag = 0xff;
                        break;
                    }
                    if (SetupReqLen > len)
                        SetupReqLen = len; // 实际需上传总长度
                    len = (SetupReqLen >= DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                    memcpy(pEP0_DataBuf, pDescr, len);
                    pDescr += len;
                } break;

                case USB_SET_ADDRESS:
                    SetupReqLen = (pSetupReqPak->wValue) & 0xff;
                    break;

                case USB_GET_CONFIGURATION:
                    pEP0_DataBuf[0] = DevConfig;
                    if (SetupReqLen > 1)
                        SetupReqLen = 1;
                    break;

                case USB_SET_CONFIGURATION:
                    DevConfig = (pSetupReqPak->wValue) & 0xff;
                    break;

                case USB_CLEAR_FEATURE: {
                    if ((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) // 端点
                    {
                        switch ((pSetupReqPak->wIndex) & 0xff) {
                        case 0x83:
                            R8_UEP3_CTRL = (R8_UEP3_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                            break;
                        case 0x03:
                            R8_UEP3_CTRL = (R8_UEP3_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                            break;
                        case 0x82:
                            R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                            break;
                        case 0x02:
                            R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                            break;
                        case 0x81:
                            R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                            break;
                        case 0x01:
                            R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                            break;
                        default:
                            errflag = 0xFF; // 不支持的端点
                            break;
                        }
                    } else if ((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE) {
                        if (pSetupReqPak->wValue == 1) {
                            USB_SleepStatus &= ~0x01;
                        }
                    } else {
                        errflag = 0xFF;
                    }
                } break;

                case USB_SET_FEATURE:
                    if ((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) {
                        /* 端点 */
                        switch (pSetupReqPak->wIndex) {
                        case 0x83:
                            R8_UEP3_CTRL = (R8_UEP3_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_STALL;
                            break;
                        case 0x03:
                            R8_UEP3_CTRL = (R8_UEP3_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_STALL;
                            break;
                        case 0x82:
                            R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_STALL;
                            break;
                        case 0x02:
                            R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_STALL;
                            break;
                        case 0x81:
                            R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_STALL;
                            break;
                        case 0x01:
                            R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_STALL;
                            break;
                        default:
                            /* 不支持的端点 */
                            errflag = 0xFF; // 不支持的端点
                            break;
                        }
                    } else if ((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE) {
                        if (pSetupReqPak->wValue == 1) {
                            /* 设置睡眠 */
                            USB_SleepStatus |= 0x01;
                        }
                    } else {
                        errflag = 0xFF;
                    }
                    break;

                case USB_GET_INTERFACE:
                    pEP0_DataBuf[0] = 0x00;
                    if (SetupReqLen > 1)
                        SetupReqLen = 1;
                    break;

                case USB_SET_INTERFACE:
                    break;

                case USB_GET_STATUS:
                    if ((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) {
                        /* 端点 */
                        pEP0_DataBuf[0] = 0x00;
                        switch (pSetupReqPak->wIndex) {
                        case 0x83:
                            if ((R8_UEP3_CTRL & (RB_UEP_T_TOG | MASK_UEP_T_RES)) == UEP_T_RES_STALL) {
                                pEP0_DataBuf[0] = 0x01;
                            }
                            break;

                        case 0x03:
                            if ((R8_UEP3_CTRL & (RB_UEP_R_TOG | MASK_UEP_R_RES)) == UEP_R_RES_STALL) {
                                pEP0_DataBuf[0] = 0x01;
                            }
                            break;

                        case 0x82:
                            if ((R8_UEP2_CTRL & (RB_UEP_T_TOG | MASK_UEP_T_RES)) == UEP_T_RES_STALL) {
                                pEP0_DataBuf[0] = 0x01;
                            }
                            break;

                        case 0x02:
                            if ((R8_UEP2_CTRL & (RB_UEP_R_TOG | MASK_UEP_R_RES)) == UEP_R_RES_STALL) {
                                pEP0_DataBuf[0] = 0x01;
                            }
                            break;

                        case 0x81:
                            if ((R8_UEP1_CTRL & (RB_UEP_T_TOG | MASK_UEP_T_RES)) == UEP_T_RES_STALL) {
                                pEP0_DataBuf[0] = 0x01;
                            }
                            break;

                        case 0x01:
                            if ((R8_UEP1_CTRL & (RB_UEP_R_TOG | MASK_UEP_R_RES)) == UEP_R_RES_STALL) {
                                pEP0_DataBuf[0] = 0x01;
                            }
                            break;
                        }
                    } else if ((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE) {
                        pEP0_DataBuf[0] = 0x00;
                        if (USB_SleepStatus) {
                            pEP0_DataBuf[0] = 0x02;
                        } else {
                            pEP0_DataBuf[0] = 0x00;
                        }
                    }
                    pEP0_DataBuf[1] = 0;
                    if (SetupReqLen >= 2) {
                        SetupReqLen = 2;
                    }
                    break;

                default:
                    errflag = 0xff;
                    break;
                }
            }
            if (errflag == 0xff) // 错误或不支持
            {
                //                  SetupReqCode = 0xFF;
                R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL; // STALL
            } else {
                if (chtype & 0x80)                                                              // 上传
                {
                    len = (SetupReqLen > DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                    SetupReqLen -= len;
                } else
                    len = 0;                                                                 // 下传
                R8_UEP0_T_LEN = len;
                R8_UEP0_CTRL  = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // 默认数据包是DATA1
            }

            R8_USB_INT_FG = RB_UIF_TRANSFER;
        }
    } else if (intflag & RB_UIF_BUS_RST) {
        R8_USB_DEV_AD = 0;
        R8_UEP0_CTRL  = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_UEP1_CTRL  = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_UEP2_CTRL  = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_UEP3_CTRL  = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_USB_INT_FG = RB_UIF_BUS_RST;
    } else if (intflag & RB_UIF_SUSPEND) {
        if (R8_USB_MIS_ST & RB_UMS_SUSPEND) {
            ;
        } // 挂起
        else {
            ;
        } // 唤醒
        R8_USB_INT_FG = RB_UIF_SUSPEND;
    } else {
        R8_USB_INT_FG = intflag;
    }
}

/*********************************************************************
 * @fn      DevHIDMouseReport
 *
 * @brief   上报鼠标数据
 *
 * @return  none
 */
void DevHIDMouseReport(uint8_t mouse)
{
    // HIDMouse[0] = mouse;
    // memcpy(pEP2_IN_DataBuf, HIDMouse, sizeof(HIDMouse));
    // DevEP2_IN_Deal(sizeof(HIDMouse));
}

/*********************************************************************
 * @fn      DevHIDKeyReport
 *
 * @brief   上报键盘数据
 *
 * @return  none
 */
void DevHIDKeyReport(uint8_t key)
{
    // HIDKey[2] = key;
    // memcpy(pEP1_IN_DataBuf, HIDKey, sizeof(HIDKey));
    // DevEP1_IN_Deal(sizeof(HIDKey));
}

/*********************************************************************
 * @fn      DevWakeup
 *
 * @brief   设备模式唤醒主机
 *
 * @return  none
 */
void DevWakeup(void)
{
    R16_PIN_ANALOG_IE &= ~(RB_PIN_USB_DP_PU);
    R8_UDEV_CTRL |= RB_UD_LOW_SPEED;
    mDelaymS(2);
    R8_UDEV_CTRL &= ~RB_UD_LOW_SPEED;
    R16_PIN_ANALOG_IE |= RB_PIN_USB_DP_PU;
}

/*********************************************************************
 * @fn      DebugInit
 *
 * @brief   调试初始化
 *
 * @return  none
 */
void DebugInit(void)
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

/*********************************************************************
 * @fn      main
 *
 * @brief   主函数
 *
 * @return  none
 */

/*********************************************************************
 * @fn      DevEP1_OUT_Deal
 *
 * @brief   端点1数据处理
 *
 * @return  none
 */
void DevEP1_OUT_Deal(uint8_t l)
{ /* 用户可自定义 */
    uint8_t i;

    for (i = 0; i < l; i++) {
        pEP1_IN_DataBuf[i] = ~pEP1_OUT_DataBuf[i];
    }
    DevEP1_IN_Deal(l);
}

/*********************************************************************
 * @fn      DevEP2_OUT_Deal
 *
 * @brief   端点2数据处理
 *
 * @return  none
 */
void DevEP2_OUT_Deal(uint8_t l)
{ /* 用户可自定义 */
    uint8_t i;

    for (i = 0; i < l; i++) {
        pEP2_IN_DataBuf[i] = ~pEP2_OUT_DataBuf[i];
    }
    DevEP2_IN_Deal(l);
}

/*********************************************************************
 * @fn      DevEP3_OUT_Deal
 *
 * @brief   端点3数据处理
 *
 * @return  none
 */
void DevEP3_OUT_Deal(uint8_t l)
{ /* 用户可自定义 */
    uint8_t i;

    for (i = 0; i < l; i++) {
        pEP3_IN_DataBuf[i] = ~pEP3_OUT_DataBuf[i];
    }
    DevEP3_IN_Deal(l);
}

/*********************************************************************
 * @fn      DevEP4_OUT_Deal
 *
 * @brief   端点4数据处理
 *
 * @return  none
 */
void DevEP4_OUT_Deal(uint8_t l)
{ /* 用户可自定义 */
    uint8_t i;

    for (i = 0; i < l; i++) {
        pEP4_IN_DataBuf[i] = ~pEP4_OUT_DataBuf[i];
    }
    DevEP4_IN_Deal(l);
}

/*********************************************************************
 * @fn      USB_IRQHandler
 *
 * @brief   USB中断函数
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void USB_IRQHandler(void) /* USB中断服务程序,使用寄存器组1 */
{
    USB_DevTransProcess();
}

void usb_EP2_send_report(uint8_t *report, uint8_t report_len)
{
    memcpy(pEP2_IN_DataBuf, report, report_len);
    DevEP2_IN_Deal(report_len);
}
void usb_EP1_send_report(uint8_t *report, uint8_t report_len)
{
    memcpy(pEP1_IN_DataBuf, report, report_len);
    DevEP1_IN_Deal(report_len);
}

void usb_set_desc(uint8_t *desc, uint8_t length)
{
    MyCfgDescr[25]    = length;
    KeyRepDesc        = desc;
    KeyRepDesc_length = length;
}
void usb_set_name(uint8_t *desc, uint8_t length)
{
    if (length <= 252) {
        memcpy(MyProdInfo + 2, desc, length);
        MyProdInfo[0] = length + 2;
    }
}
void usb_hid_kbd_init(void)
{
    pEP0_RAM_Addr = EP0_Databuf;
    pEP1_RAM_Addr = EP1_Databuf;
    pEP2_RAM_Addr = EP2_Databuf;
    pEP3_RAM_Addr = EP3_Databuf;

    USB_DeviceInit();

    PFIC_EnableIRQ(USB_IRQn);
    if (KeyRepDesc == NULL) {
        PRINT("NO descript set!!\n");
    }
}
#pragma region
// const uint8_t report_map_all[127] = {
//     0x05, 0x01, // USAGE_PAGE (Generic Desktop)
//     0x09, 0x06, // USAGE (Keyboard)
//     0xa1, 0x01, // COLLECTION (Application)
//     0x85, 1,    // Report ID (8)
//     0x05, 0x07, //   USAGE_PAGE (Keyboard)
//     0x19, 0xe0, //   USAGE_MINIMUM (Keyboard LeftControl)
//     0x29, 0xe7, //   USAGE_MAXIMUM (Keyboard Right GUI)
//     0x15, 0x00, //   LOGICAL_MINIMUM (0)
//     0x25, 0x01, //   LOGICAL_MAXIMUM (1)
//     0x75, 0x01, //   REPORT_SIZE (1)
//     0x95, 0x08, //   REPORT_COUNT (8)
//     0x81, 0x02, //   INPUT (Data,Var,Abs)
//     0x95, 1,    //   REPORT_COUNT (1)
//     0x75, 8,    //   REPORT_SIZE (8)
//     0x81, 0x03, //   INPUT (Cnst,Var,Abs)

//     0x95, 0x05, //   REPORT_COUNT (5)
//     0x75, 0x01, //   REPORT_SIZE (1)
//     0x05, 0x08, //   USAGE_PAGE (LEDs)
//     0x19, 0x01, //   USAGE_MINIMUM (Num Lock)
//     0x29, 0x05, //   USAGE_MAXIMUM (Kana)
//     0x91, 0x02, //   OUTPUT (Data,Var,Abs)

//     0x95, 0x01, //   REPORT_COUNT (1)
//     0x75, 0x03, //   REPORT_SIZE (3)
//     0x91, 0x03, //   OUTPUT (Cnst,Var,Abs)

//     // 0x95, 0x06, //   REPORT_COUNT (6)
//     // 0x75, 0x08, //   REPORT_SIZE (8)
//     // 0x15, 0x00, //   LOGICAL_MINIMUM (0)
//     // 0x25, 0xFF, //   LOGICAL_MAXIMUM (255)
//     // 0x05, 0x07, //   USAGE_PAGE (Keyboard)
//     // 0x19, 0x00, //   USAGE_MINIMUM (Reserved (no event indicated))
//     // 0x29, 0x65, //   USAGE_MAXIMUM (Keyboard Application)
//     // 0x81, 0x00, //   INPUT (Data,Ary,Abs)

//     0x05, 0x07, //   Usage Page (Key Codes),
//     0x95, 0x78, //   Report Count (120),
//     0x75, 0x01, //   Report Size  (1),
//     0x15, 0x00, //   Logical Minimum (0),
//     0x25, 0x01, //   Logical Maximum (1),
//     0x19, 0x00, //   Usage Minimum (0),
//     0x29, 101,  //   Usage Maximum (101),
//     0x81, 0x02, //   Input (Data, Variable, Absolute),

//     0xc0, // END_COLLECTION

//     // 0x05, 0x01, // USAGE_PAGE (Generic Desktop)
//     // 0x09, 0x02, // USAGE (Mouse)
//     // 0xa1, 0x01, // COLLECTION (Application)
//     // 0x85, 0x02, // ??ID (2)
//     // 0x09, 0x01, //   USAGE (Pointer)
//     // 0xa1, 0x00, //   COLLECTION (Physical)
//     // 0x05, 0x09, //     USAGE_PAGE (Button)
//     // 0x19, 0x01, //     USAGE_MINIMUM (Button 1)
//     // 0x29, 0x03, //     USAGE_MAXIMUM (Button 3)
//     // 0x15, 0x00, //     LOGICAL_MINIMUM (0)
//     // 0x25, 0x01, //     LOGICAL_MAXIMUM (1)
//     // 0x95, 0x03, //     REPORT_COUNT (3)
//     // 0x75, 0x01, //     REPORT_SIZE (1)
//     // 0x81, 0x02, //     INPUT (Data,Var,Abs)
//     // 0x95, 0x01, //     REPORT_COUNT (1)
//     // 0x75, 0x05, //     REPORT_SIZE (5)
//     // 0x81, 0x03, //     INPUT (Cnst,Var,Abs)
//     // 0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
//     // 0x09, 0x30, //     USAGE (X)
//     // 0x09, 0x31, //     USAGE (Y)
//     // 0x09, 0x38, //     USAGE (Wheel)
//     // 0x15, 0x81, //     LOGICAL_MINIMUM (-127)
//     // 0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
//     // 0x75, 0x08, //     REPORT_SIZE (8)
//     // 0x95, 0x03, //     REPORT_COUNT (3)
//     // 0x81, 0x06, //     INPUT (Data,Var,Rel)
//     // 0xc0,       //   END_COLLECTION
//     // 0xc0,       // END_COLLECTION

//     // 0x05, 0x0d,       // USAGE_PAGE (Digitizers)
//     // 0x09, 0x04,       // USAGE (Touch Screen)
//     // 0xa1, 0x01,       // COLLECTION (Application)
//     // 0x85, 0x04,       // ??ID (4)
//     // 0x09, 0x22,       //   USAGE (Finger)
//     // 0xa1, 0x00,       //   COLLECTION (Physical)
//     // 0x09, 0x42,       //     USAGE (Tip Switch)
//     // 0x15, 0x00,       //     LOGICAL_MINIMUM (0)
//     // 0x25, 0x01,       //     LOGICAL_MAXIMUM (1)
//     // 0x75, 0x01,       //     REPORT_SIZE (1)
//     // 0x95, 0x01,       //     REPORT_COUNT (1)
//     // 0x81, 0x02,       //     INPUT (Data,Var,Abs)
//     // 0x75, 0x07,       //     REPORT_SIZE (7)
//     // 0x95, 0x01,       //     REPORT_COUNT (1)
//     // 0x81, 0x03,       //     INPUT (Cnst,Var,Abs)
//     // 0x05, 0x01,       //     USAGE_PAGE (Generic Desktop)
//     // 0x09, 0x30,       //     USAGE (X)
//     // 0x75, 0x10,       //     REPORT_SIZE (16)
//     // 0x95, 0x01,       //     REPORT_COUNT (1)
//     // 0x15, 0x00,       //     LOGICAL_MINIMUM (0)
//     // 0x26, 0xff, 0x0f, //     LOGICAL_MAXIMUM (4095)
//     // 0x35, 0x00,       //     PHYSICAL_MINIMUM (0)
//     // 0x46, 0xff, 0x0f, //     PHYSICAL_MAXIMUM (4095)
//     // 0x81, 0x02,       //     INPUT (Data,Var,Abs)
//     // 0x09, 0x31,       //     USAGE (Y)
//     // 0x95, 0x01,       //     REPORT_COUNT (1)
//     // 0x75, 0x10,       //     REPORT_SIZE (16)
//     // 0x15, 0x00,       //     LOGICAL_MINIMUM (0)
//     // 0x26, 0xff, 0x0f, //     LOGICAL_MAXIMUM (4095)
//     // 0x35, 0x00,       //     PHYSICAL_MINIMUM (0)
//     // 0x46, 0xff, 0x0f, //     PHYSICAL_MAXIMUM (4095)
//     // 0x81, 0x02,       //     INPUT (Data,Var,Abs)
//     // 0xc0,             //   END_COLLECTION
//     // 0xc0,             // END_COLLECTION

//     0x05, 0x0C, // 0   	GLOBAL_USAGE_PAGE(Consumer)
//     0x09, 0x01, // 2   	LOCAL_USAGE(	Consumer Control 	)
//     0xA1, 0x01, // 4   	MAIN_COLLECTION(Applicatior)
//     0x85, 0x05, // 6   	GLOBAL_REPORT_ID(5)
//     0xA1, 0x00, // 8   	MAIN_COLLECTION(Physical)
//     0x09, 0xE9, // 10  	LOCAL_USAGE(	Volume Increment 	)
//     0x09, 0xEA, // 12  	LOCAL_USAGE(	Volume Decrement 	)
//     0x09, 0xE2, // 14  	LOCAL_USAGE(	Mute 	)
//     0x09, 0xCD, // 16  	LOCAL_USAGE(	Play/Pause 	)
//     0x35, 0x00, // 18  	GLOBAL_PHYSICAL_MINIMUM(0)
//     0x45, 0x07, // 20  	GLOBAL_PHYSICAL_MAXIMUM(7)
//     0x15, 0x00, // 22  	GLOBAL_LOGICAL_MINIMUM(0)
//     0x25, 0x01, // 24  	GLOBAL_LOCAL_MAXIMUM(1)
//     0x75, 0x01, // 26  	GLOBAL_REPORT_SIZE(1)
//     0x95, 0x04, // 28  	GLOBAL_REPORT_COUNT(4)
//     0x81, 0x02, // 30  	MAIN_INPUT(data var absolute NoWrap linear PreferredState NoNullPosition NonVolatile )	Input 0.4
//     0x75, 0x01, // 32  	GLOBAL_REPORT_SIZE(1)
//     0x95, 0x04, // 34  	GLOBAL_REPORT_COUNT(4)
//     0x81, 0x01, // 36  	MAIN_INPUT(const array absolute NoWrap linear PreferredState NoNullPosition NonVolatile )	Input 1.0
//     0xC0,       // 38  	MAIN_COLLECTION_END
//     0xC0,       // 39  	MAIN_COLLECTION_END

//     // 0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
//     // 0x15, 0x00,       // LOGICAL_MINIMUM (0)
//     // 0x09, 0x04,       // USAGE (Joystick)
//     // 0xa1, 0x01,       // COLLECTION (Application)
//     // 0x85, 0x06,       // ??ID (6)
//     // 0x05, 0x02,       //   USAGE_PAGE (Simulation Controls)
//     // 0x09, 0xbb,       //   USAGE (Throttle)
//     // 0x15, 0x81,       //   LOGICAL_MINIMUM (-127)
//     // 0x25, 0x7f,       //   LOGICAL_MAXIMUM (127)
//     // 0x75, 0x08,       //   REPORT_SIZE (8)
//     // 0x95, 0x01,       //   REPORT_COUNT (1)
//     // 0x81, 0x02,       //   INPUT (Data,Var,Abs)
//     // 0x05, 0x01,       //   USAGE_PAGE (Generic Desktop)
//     // 0x09, 0x01,       //   USAGE (Pointer)
//     // 0xa1, 0x00,       //   COLLECTION (Physical)
//     // 0x09, 0x30,       //     USAGE (X)
//     // 0x09, 0x31,       //     USAGE (Y)
//     // 0x95, 0x02,       //     REPORT_COUNT (2)
//     // 0x81, 0x02,       //     INPUT (Data,Var,Abs)
//     // 0xc0,             //   END_COLLECTION
//     // 0x09, 0x39,       //   USAGE (Hat switch)
//     // 0x15, 0x00,       //   LOGICAL_MINIMUM (0)
//     // 0x25, 0x03,       //   LOGICAL_MAXIMUM (3)
//     // 0x35, 0x00,       //   PHYSICAL_MINIMUM (0)
//     // 0x46, 0x0e, 0x01, //   PHYSICAL_MAXIMUM (270)
//     // 0x65, 0x14,       //   UNIT (Eng Rot:Angular Pos)
//     // 0x75, 0x04,       //   REPORT_SIZE (4)
//     // 0x95, 0x01,       //   REPORT_COUNT (1)
//     // 0x81, 0x02,       //   INPUT (Data,Var,Abs)
//     // 0x05, 0x09,       //   USAGE_PAGE (Button)
//     // 0x19, 0x01,       //   USAGE_MINIMUM (Button 1)
//     // 0x29, 0x04,       //   USAGE_MAXIMUM (Button 4)
//     // 0x15, 0x00,       //   LOGICAL_MINIMUM (0)
//     // 0x25, 0x01,       //   LOGICAL_MAXIMUM (1)
//     // 0x75, 0x01,       //   REPORT_SIZE (1)
//     // 0x95, 0x04,       //   REPORT_COUNT (4)
//     // 0x55, 0x00,       //   UNIT_EXPONENT (0)
//     // 0x65, 0x00,       //   UNIT (None)
//     // 0x81, 0x02,       //   INPUT (Data,Var,Abs)
//     // 0xc0,             // END_COLLECTION

//     0x05, 0x8c, /* USAGE_PAGE (ST Page) */
//     0x09, 0x01, /* USAGE (Demo Kit) */
//     0xa1, 0x01, /* COLLECTION (Application) */
//     0x85, 0x07, // 6   	GLOBAL_REPORT_ID(7)

//     // The Input report
//     0x09, 0x03,       // USAGE ID - Vendor defined
//     0x15, 0x00,       // LOGICAL_MINIMUM (0)
//     0x26, 0x00, 0xFF, // LOGICAL_MAXIMUM (255)
//     0x75, 0x08,       // REPORT_SIZE (8bit)
//     0x95, 64,         // REPORT_COUNT (64Byte)
//     0x81, 0x02,       // INPUT (Data,Var,Abs)

//     // // The Output report
//     // 0x09, 0x04,                  // USAGE ID - Vendor defined
//     // 0x15, 0x00,                  // LOGICAL_MINIMUM (0)
//     // 0x26, 0x00, 0xFF,            // LOGICAL_MAXIMUM (255)
//     // 0x75, 0x08,                  // REPORT_SIZE (8bit)
//     // 0x95, CUSTOM_HID_EPOUT_SIZE, // REPORT_COUNT (64Byte)
//     // 0x91, 0x02,                  // OUTPUT (Data,Var,Abs)

//     // 0x05, 0x01, // USAGE_PAGE (Generic Desktop)
//     // 0x09, 0x06, // USAGE (Keyboard)
//     // 0xa1, 0x01, // COLLECTION (Application)
//     // 0x85, 1,    // Report ID (8)
//     // 0x05, 0x07, //   USAGE_PAGE (Keyboard)
//     // 0x19, 0xe0, //   USAGE_MINIMUM (Keyboard LeftControl)
//     // 0x29, 0xe7, //   USAGE_MAXIMUM (Keyboard Right GUI)
//     // 0x15, 0x00, //   LOGICAL_MINIMUM (0)
//     // 0x25, 0x01, //   LOGICAL_MAXIMUM (1)
//     // 0x75, 0x01, //   REPORT_SIZE (1)
//     // 0x95, 0x08, //   REPORT_COUNT (8)
//     // 0x81, 0x02, //   INPUT (Data,Var,Abs)
//     // 0x95, 0x01, //   REPORT_COUNT (1)
//     // 0x75, 0x08, //   REPORT_SIZE (8)
//     // 0x81, 0x03, //   INPUT (Cnst,Var,Abs)

//     // 0x95, 0x05, //   REPORT_COUNT (5)
//     // 0x75, 0x01, //   REPORT_SIZE (1)
//     // 0x05, 0x08, //   USAGE_PAGE (LEDs)
//     // 0x19, 0x01, //   USAGE_MINIMUM (Num Lock)
//     // 0x29, 0x05, //   USAGE_MAXIMUM (Kana)
//     // 0x91, 0x02, //   OUTPUT (Data,Var,Abs)

//     // 0x95, 0x01, //   REPORT_COUNT (1)
//     // 0x75, 0x03, //   REPORT_SIZE (3)
//     // 0x91, 0x03, //   OUTPUT (Cnst,Var,Abs)

//     // // 0x95, 0x06, //   REPORT_COUNT (6)
//     // // 0x75, 0x08, //   REPORT_SIZE (8)
//     // // 0x15, 0x00, //   LOGICAL_MINIMUM (0)
//     // // 0x25, 0xFF, //   LOGICAL_MAXIMUM (255)
//     // // 0x05, 0x07, //   USAGE_PAGE (Keyboard)
//     // // 0x19, 0x00, //   USAGE_MINIMUM (Reserved (no event indicated))
//     // // 0x29, 0x65, //   USAGE_MAXIMUM (Keyboard Application)
//     // // 0x81, 0x00, //   INPUT (Data,Ary,Abs)

//     // 0x05, 0x07, //   Usage Page (Key Codes),
//     // 0x95, 0x78, //   Report Count (120),
//     // 0x75, 0x01, //   Report Size  (1),
//     // 0x15, 0x00, //   Logical Minimum (0),
//     // 0x25, 0x01, //   Logical Maximum (1),
//     // 0x19, 0x00, //   Usage Minimum (0),
//     // 0x29, 120,  //   Usage Maximum (120),
//     // 0x81, 0x02, //   Input (Data, Variable, Absolute),
//     // 0xc0,       // END_COLLECTION

//     // /* USER CODE END 0 */
//     0xC0 /*     END_COLLECTION	             */

//     // 0x05, 0x01, // Usage Pg (Generic Desktop)
//     // 0x09, 0x06, // Usage (Keyboard)
//     // 0xA1, 0x01, // Collection: (Application)
//     //             //
//     // 0x05, 0x07, // Usage Pg (Key Codes)
//     // 0x19, 0xE0, // Usage Min (224)
//     // 0x29, 0xE7, // Usage Max (231)
//     // 0x15, 0x00, // Log Min (0)
//     // 0x25, 0x01, // Log Max (1)
//     //             //
//     //             // Modifier byte
//     // 0x75, 0x01, // Report Size (1)
//     // 0x95, 0x08, // Report Count (8)
//     // 0x81, 0x02, // Input: (Data, Variable, Absolute)
//     //             //
//     //             // Reserved byte
//     // 0x95, 0x01, // Report Count (1)
//     // 0x75, 0x08, // Report Size (8)
//     // 0x81, 0x01, // Input: (Constant)
//     //             //
//     //             // LED report
//     // 0x95, 0x05, // Report Count (5)
//     // 0x75, 0x01, // Report Size (1)
//     // 0x05, 0x08, // Usage Pg (LEDs)
//     // 0x19, 0x01, // Usage Min (1)
//     // 0x29, 0x05, // Usage Max (5)
//     // 0x91, 0x02, // Output: (Data, Variable, Absolute)
//     //             //
//     //             // LED report padding
//     // 0x95, 0x01, // Report Count (1)
//     // 0x75, 0x03, // Report Size (3)
//     // 0x91, 0x01, // Output: (Constant)
//     //             //
//     //             // Key arrays (6 bytes)
//     // 0x95, 0x06, // Report Count (6)
//     // 0x75, 0x08, // Report Size (8)
//     // 0x15, 0x00, // Log Min (0)
//     // 0x25, 0x65, // Log Max (101)
//     // 0x05, 0x07, // Usage Pg (Key Codes)
//     // 0x19, 0x00, // Usage Min (0)
//     // 0x29, 0x65, // Usage Max (101)
//     // 0x81, 0x00, // Input: (Data, Array)
//     //             //
//     // 0xC0        // End Collection
// };
#pragma endregion
// uint8_t usb_name[] = {
//     0x42, 0x30, 0x63, 0x30, 0x64, 0x30, 0x44, 0x30, 0x26, 0x20, 0x0D, 0x00, 0x0A, 0x00, 0x91, 0x66, 0x4F, 0x30, 0x66, 0x30, 0x72, 0x5E, 0x4B, 0x30, 0x89, 0x30, 0x73, 0x30, 0x5D, 0x30, 0x46, 0x30, 0x26, 0x20, 0x0D, 0x00, 0x0A, 0x00, 0xD5, 0x52, 0x44, 0x30, 0x66, 0x30, 0x6A, 0x30, 0x44, 0x30, 0x6E, 0x30, 0x6B, 0x30, 0x91, 0x66, 0x44, 0x30, 0x88, 0x30, 0x5E, 0xFF, 0x26, 0x20};

void test()
{
    //    usb_set_name(usb_name, sizeof(usb_name));
    // usb_set_desc(report_map_all, sizeof(report_map_all));
    //    usb_hid_kbd_init();
    while (1)
        ;
}
#include "main.h"
void usb_data_received(uint8_t *data, uint8_t len)
{
    hid_rx_iqr(data);
    running_data.usb_is_connected = 1;
    srand(SysTick->CNT);
    PRINT("%d\n", SysTick->CNT);
}
void usb_disconnect(void)
{
    running_data.usb_is_connected  = 0;
    running_data.power_off_timeout = 2 * 60;
}
