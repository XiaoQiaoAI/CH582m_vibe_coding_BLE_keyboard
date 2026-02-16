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