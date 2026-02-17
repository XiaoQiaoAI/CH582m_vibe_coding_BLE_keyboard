# 功能介绍

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

# 工作流程

键盘设备 <-> BLE <-> 电脑 (c开发, 是个蓝牙HID键盘, 具备自定义服务特征传输数据, `本仓库`)

BLE <-> BLE_tcp_driver.exe <-> TCP接口 (c#开发, 图形化界面, 只有windows, 需要常态开启)`试过python的bleak库, 无法使用, 因为这是一个hid设备, 连接电脑后没开广播, 而且和电脑绑定后电脑会主动连接设备, 导致bleak搜不到蓝牙, 直接根据mac连接失败`

TCP接口 <-> 上位机, 修改配置 (Python开发, 图形化界面)

TCP接口 <-> claude hook程序, 上报claude状态 (Python开发, 脚本)

# 该仓库软件说明

编译软件 `MounRiver Studio`

下载软件 `WCHISP Studio`

### HOW to build

1. 下载官方sdk: `https://www.wch.cn/downloads/CH583EVT_ZIP.html`
2. 把仓库里的代码放到 `EXAM\BLE` 目录下(参考其他例程放置)
3. 打开工程文件 `HID_Keyboard_582m_vibe_coding.wvproj`
4. 结果文件在 `obj\HID_Keyboard_582m_vibe_coding.hex`

### HOW to download

1. 安装wchisp, 控制芯片进boot模式, 通过usb进行程序下载
2. 暂时boot引脚还不方便手动搞

# 快速上手

### 1️⃣ 获取硬件
两种方式：
- **自己动手**：立创开源广场马上开源
- **购买套件**：我们提供完整的 DIY 硬件包（PCB + 所有元件 + USB 线 + 亚克力外壳+键帽键轴），也提供开箱即用的完整版 – [claude外接自定义键盘](https://e.tb.cn/h.7Ceul3q1qmKK51M?tk=S6nIUnuIiBV)。键盘开源讨论qq群（1082678922）

### 2️⃣ 获取固件，上位机等全套软件，教程
[点击下载最新版键盘固件包](https://github.com/XiaoQiaoAI/CH582m_vibe_coding_BLE_keyboard/releases/tag/v1.0)


