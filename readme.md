# Claude外接控制键盘

**为 Claude Code 打造的物理控制键盘 – 带实体按键、OLED 屏幕、RGB LED 和语音输入。完全开源，无限自定义。**

![产品演示](images/demo.gif) <!-- 请替换为实际图片/动图 -->

---

## ✨ 功能亮点

- **四个实体按键**
  - ✅ **绿色**：批准命令
  - ❌ **红色**：拒绝命令
  - 🎤 **蓝色**：按住说话，松开发送文字给 Claude（语音转文字）
  - ⚙️ **自定义键**：默认为 回车，可随意更改功能

- **拨杆开关** – 轻轻一拨，在**自动批准**和**手动模式**之间切换

- **0.96英寸 OLED 屏幕**
  - 显示当前状态、可爱像素图，或者你上传的自定义图片、动图（动漫角色、表情包……）
  - 内置小游戏正在开发中

- **8颗 RGB LED（WS2812B）**
  - 每种状态都有对应的灯光反馈：空闲、工作中、等待批准、自动模式、语音输入、ultra‑think……
  - 灯光颜色和效果完全可自定义

- **平台兼容** – 目前只支持Windows , macOS / Linux 正在路上，挖坑ing

- **完全开源** – 硬件原理图、PCB、固件、上位机工具全部开放


## 🚀 快速上手

### 1️⃣ 获取硬件
两种方式：
- **自己动手**：立创开源广场链接  
- **购买套件**：我们提供完整的 DIY 硬件包（PCB + 所有元件 + USB 线 + 亚克力外壳+键帽键轴），也提供开箱即用的完整版 – [点击查看淘宝店铺](#)。

### 2️⃣ 烧录固件
-下载软件 `WCHISP Studio`（烧录软件工具）
https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html
- 按住 `蓝色` 按钮的同时通过 Type-C连接开发板。按照图片中选择，下载。  
 
下载成功长这样
 

### 3️⃣ 安装上位机工具
运行install_hook.py 给claude code安装钩子
语音输入我用的是https://github.com/HaujetZhao/CapsWriter-Offline

### 4️⃣ 开始自定义！
- 让claud来按照你喜欢的，更改代码吧
- 将按键映射为任何快捷键或宏。  
- 上传你喜欢的动漫角色到 OLED 屏幕。  
- 设置专属 LED 配色方案。  


## 🔧 深度自定义
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

使用vs .NetFramework开发的 BLE - TCP 桥接
给python写的vibe code 上位机和设备间通信用的
https://github.com/PtilopsisSak/BLE_tcp_bridge_for_vibe_code

