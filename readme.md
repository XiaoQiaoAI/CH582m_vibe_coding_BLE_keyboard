# Vibe coding Keyboard
> The four-key numeric keypad (voice input, YES, NO, automatic approval, dual-mode input, RGB color lights, animated display, etc.) is compatible with Claude. / 四键小键盘（语音输入，YES，NO，自动批准，双模输入，RGB彩灯，显示动图等）适配claude

## Language / 语言选择
- [English](#english)
- [中文](#中文)

---

## English
### 1. Introduction
[Keyboard video display](https://x.com/zhngxnyng199073/status/2024706634443084056?s=20)

**A physical control keyboard built for Claude Code – with physical buttons, OLED screen, RGB LED, and voice input. Fully open source, infinitely customizable.**

- **Four physical buttons**
  - ✅ **Green**: Approve command
  - ❌ **Red**: Reject command
  - 🎤 **Blue**: Press and hold to speak, release to send text to Claude (voice-to-text)
  - ⚙️ **Custom key**: Default is Enter, can be freely customized
    
- **Toggle switch** – Flip gently to switch between **auto-approval** and **manual mode**

- **Dual-mode** – Both **Type-C** and **Bluetooth** connections, charge via Type-C when battery is low

- **0.96-inch OLED screen**
  - Displays current status, cute pixel art, or your uploaded custom images/animations (anime characters, memes, etc.)
  - Built-in mini-games are under development

- **8 RGB LEDs (WS2812B)**
  - Corresponding light feedback for each status: Idle, Working, Waiting for approval, Auto mode, Voice input, etc.
  - Light colors and effects are fully customizable

- **Platform compatibility** – Currently only supports Windows; macOS / Linux support is coming soon

### 2. Workflow
Keyboard device <-> BLE <-> Computer (Developed in C, a Bluetooth HID keyboard with custom service characteristics for data transmission, `this repository`)

BLE <-> BLE_tcp_driver.exe <-> TCP interface (Developed in C#, GUI, Windows only, needs to run in the background) `Tried Python's bleak library, but it didn't work because this is an HID device. It doesn't broadcast after connecting to the computer, and the computer will actively connect to the device after pairing, making bleak unable to find the Bluetooth device, and direct connection via MAC address fails.`

TCP interface <-> Host software (for configuration modification, developed in Python with GUI)

TCP interface <-> Claude hook program (reports Claude status, Python script)

### 3. Software Description for This Repository
- Compilation software: `MounRiver Studio`
- Download software: `WCHISP Studio`

### 4. How to Build
1. Download the official SDK: `https://www.wch.cn/downloads/CH583EVT_ZIP.html`
2. Place the code from this repository into the `EXAM\BLE` directory (refer to other example projects for placement)
3. Open the project file `HID_Keyboard_582m_vibe_coding.wvproj`
4. The output file is located at `obj\HID_Keyboard_582m_vibe_coding.hex`

### 5. How to Download
1. Install wchisp, control the chip to enter boot mode, and download the program via USB
2. The boot pin is not convenient to manually operate for now

### 6. Quick Start
#### 1️⃣ Get the Hardware
Two options:
- **DIY**: Will be open sourced on Lichuang Open Source Plaza soon
- **Purchase a kit**: We provide complete DIY hardware packages (PCB + all components + USB cable + acrylic case + keycaps and switches), as well as ready-to-use versions – [Claude external custom keyboard](https://e.tb.cn/h.7Ceul3q1qmKK51M?tk=S6nIUnuIiBV). Join the keyboard open source discussion QQ group: 1082678922

#### 2️⃣ Get Firmware, Host Software, and Full Tutorials
[Click to download the latest keyboard firmware package](https://github.com/XiaoQiaoAI/CH582m_vibe_coding_BLE_keyboard/releases/tag/v1.0)

### 7. Edit the Whole Project
- [Keyboard firmware code](https://github.com/XiaoQiaoAI/CH582m_vibe_coding_BLE_keyboard)
- [Keyboard-computer bridge software](https://github.com/XiaoQiaoAI/BLE_tcp_bridge_for_vibe_code)
- [Computer host software - Hook and customization tool](https://github.com/XiaoQiaoAI/vibe_code_config_tool)

GIF author [x: @downvote_me](https://x.com/downvote_me)

---

## 中文
### 1. 功能介绍
[键盘展示视频](https://www.bilibili.com/video/BV1C6ZMBgEcM/?spm_id_from=333.1387.0.0&vd_source=df25740f1d3732101203e58b2e2a6026)

**为 Claude Code 打造的物理控制键盘 – 带实体按键、OLED 屏幕、RGB LED 和语音输入。完全开源，无限自定义。**

- **四个实体按键**
  - ✅ **绿色**：批准命令
  - ❌ **红色**：拒绝命令
  - 🎤 **蓝色**：按住说话，松开发送文字给 Claude（语音转文字）
  - ⚙️ **自定义键**：默认为 回车，可随意更改功能
    
- **拨杆开关** – 轻轻一拨，在**自动批准**和**手动模式**之间切换

- **双模** - **type-C**连接和**蓝牙**连接两种方式，没电了type-C充电即可

- **0.96英寸 OLED 屏幕**
  - 显示当前状态、可爱像素图，或者你上传的自定义图片、动图（动漫角色、表情包……）
  - 内置小游戏正在开发中

- **8颗 RGB LED（WS2812B）**
  - 每种状态都有对应的灯光反馈：空闲、工作中、等待批准、自动模式、语音输入……
  - 灯光颜色和效果完全可自定义

- **平台兼容** – 目前只支持Windows , macOS / Linux 正在路上，挖坑ing

### 2. 工作流程
键盘设备 <-> BLE <-> 电脑 (c开发, 是个蓝牙HID键盘, 具备自定义服务特征传输数据, `本仓库`)

BLE <-> BLE_tcp_driver.exe <-> TCP接口 (c#开发, 图形化界面, 只有windows, 需要常态开启)`试过python的bleak库, 无法使用, 因为这是一个hid设备, 连接电脑后没开广播, 而且和电脑绑定后电脑会主动连接设备, 导致bleak搜不到蓝牙, 直接根据mac连接失败`

TCP接口 <-> 上位机, 修改配置 (Python开发, 图形化界面)

TCP接口 <-> claude hook程序, 上报claude状态 (Python开发, 脚本)

### 3. 该仓库软件说明
编译软件 `MounRiver Studio`

下载软件 `WCHISP Studio`

### 4. How to Build
1. 下载官方sdk: `https://www.wch.cn/downloads/CH583EVT_ZIP.html`
2. 把仓库里的代码放到 `EXAM\BLE` 目录下(参考其他例程放置)
3. 打开工程文件 `HID_Keyboard_582m_vibe_coding.wvproj`
4. 结果文件在 `obj\HID_Keyboard_582m_vibe_coding.hex`

### 5. How to Download
1. 安装wchisp, 控制芯片进boot模式, 通过usb进行程序下载
2. 暂时boot引脚还不方便手动搞

### 6. 快速上手
#### 1️⃣ 获取硬件
两种方式：
- **自己动手**：板子的原理图在hard_ware里。
- **购买套件**：我们提供完整的 DIY 硬件包（PCB + 所有元件 + USB 线 + 亚克力外壳+键帽键轴），也提供开箱即用的完整版 – [claude外接自定义键盘](https://e.tb.cn/h.7Ceul3q1qmKK51M?tk=S6nIUnuIiBV)。键盘开源讨论qq群（1082678922）

#### 2️⃣ 获取固件，上位机等全套封装好的软件，教程
[点击下载最新版键盘固件包](https://github.com/XiaoQiaoAI/CH582m_vibe_coding_BLE_keyboard/releases/tag/v1.0)

### 7. 编辑整个工程，修改⚙️
[键盘固件代码](https://github.com/XiaoQiaoAI/CH582m_vibe_coding_BLE_keyboard)

展示里面的动图作者:[x: @downvote_me](https://x.com/downvote_me)
























# 非商业用途开源许可协议 (Non-Commercial Open Source License)

版权所有 (c) [2026] [南京市锦心湾科技有限责任公司]

## 1. 权限授予
被许可人（以下简称“您”）可以在满足以下条件的前提下使用、复制、修改、分发本软件及其衍生作品：

- **仅限非商业用途**：本软件仅可用于个人学习、学术研究或非盈利项目。
- **保留署名信息**：在任何复制、修改或分发的软件中，必须明确保留本协议及原作者署名。
- **源代码开放**：如果您对本软件进行了修改并进行分发，应将修改后的源代码一并公开。

## 2. 禁止条款
您 **不得**：

- 将本软件或其衍生作品用于任何商业活动，包括但不限于销售、租赁、广告、商业服务或盈利性部署；
- 去除或修改版权声明、许可信息；
- 将本软件用于违反法律法规或侵害他人权益的用途。

## 3. 免责声明
本软件按“现状”提供，作者对本软件的适用性、稳定性、功能性 **不作任何明示或暗示的保证**。  
使用本软件所产生的任何风险由您自行承担，作者不对由此造成的任何直接或间接损害承担责任。

## 4. 其他说明
- 您可以在非商业的前提下自由分发本软件；
- 如需商业授权，请联系作者以获得单独书面许可；
- 本协议的解释权归原作者所有。
