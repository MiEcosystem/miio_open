

#### Q: 有问题怎么办？
A: 关于产品定义或[小米IoT开发者平台](https://iot.mi.com/new/index.html)的问题，请联系米家产品经理。技术问题请区分是芯片开发的问题还是米家接入的问题。如果是芯片开发的问题，请联系厂商 FAE，如果是米家接入的问题，请先搜索[米家 BLE 标准认证接入demo](https://github.com/MiEcosystem/mijia_ble_standard)相关 issue，看是否有类似的问题。如果没有，请按照模板创建新 issue（务必附带设备侧 log 信息）。

---

#### Q: 如何查看 log 信息？
A:设备端：请下载并安装 JLink，
- unix-like 平台
```bash
添加 JLinkExe 到 $PATH，后执行 JLinkExe
$ JLinkExe -device <your_soc_platform> -if swd -speed 1000 -RTTTelnetPort 2000 -autoconnect 1
新开一个 term，然后 telnet 本地 2000 端口：
$ telnet localhost 2000
```
- windows 平台：打开 Segger JLink RTT Viewer，选择对应芯片型号。连接成功可直接查看log。

App端：
- android ：安装 [debug 版本 APK](https://github.com/MiEcosystem/NewXmPluginSDK/blob/master/%E7%B1%B3%E5%AE%B6%E8%B0%83%E8%AF%95APK%E4%B8%8B%E8%BD%BD%E5%9C%B0%E5%9D%80.md)，然后查找文件管理 -> 手机 -> Android -> data -> com.xiaomi.smarthome -> files -> log -> miio-bluetooth log

---

#### Q: 产品的pid如何获取？

A: 产品的pid是在[小米IoT开发者平台](https://iot.mi.com/new/index.html)上注册产品时生成的，在demo中pid = 156，是一个弱绑定的蓝牙开发板产品，用于测试。 还有一个强绑定的蓝牙开发板产品pid = 930，此两个产品类型仅用于开发者做初期测试。在真正的产品开发中，开发者应需要pid及强弱绑定关系，与在[小米IoT开发者平台](https://iot.mi.com/new/index.html)上注册产品时的信息保持一致。强弱绑定的具体定义，可参考小米IoT开发者平台。

---

#### Q: 同时发现多个同类产品时(如多个蓝牙温湿度传感器)，用户如何确定绑定哪个产品？

A: 开发者需要在开发者平台选择蓝牙配对方式，目前支持三种配对选择方式：
* 通过 RSSI 判断：发现多个同类产品时，选择信号最强的产品进行绑定。
* 设备选择即配对：产品需具有按键。当用户按下按键后，产品发送带有 solicited bit 广播包，米家 App 收到该广播包后会进行绑定。
* App 选择即配对：产品需具有字符显示能力，能够显示 MAC 地址最后 2 字节。用户在米家 App 内选择相同 MAC 的产品进行绑定。

---
#### Q: 设备如何OTA？

A: 目前没有统一的米家OTA机制，目前该功能正在研发过程中，我们后续将提供主流芯片的OTA的统一实现，降低厂商的接入成本。

---

#### Q: 为什么改变 PRODUCT_ID 的值就认证失败了？

A: 每个安全芯片只能绑定唯一一个 PID 和 MAC，如需更改 PID 或 MAC 请更换安全芯片。

---
#### Q: 如何申请一个Object ID？

A: 目前不支持厂商自己申请Object ID, 如有特殊需求，请联系对接的产品经理。


#### Q: BLE设备，通过手机App直连,怎么上报数据(不通过蓝牙网关上报数据)

A: 插件开发者可以调用 `set_user_device_data` 这个接口上报数据。可见参考[这个文档](https://github.com/MiEcosystem/ios-rn-sdk/blob/master/MiHomePluginSDK/docs/callSmartHomeAPI.md)上接口的说明

