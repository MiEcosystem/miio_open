# Arduino DEMO

小米IoT开发者平台提供基于Arduino开发板，使用MIoT-Spec协议开发的固件及扩展程序Demo。

## 基础准备

为了您能更快理解相关内容，请提前阅读[小米IoT开发者平台](https://iot.mi.com/)中开发引导。

其中[直连接入快速引导](https://iot.mi.com/new/doc/02-%E4%BA%A7%E5%93%81%E6%8E%A5%E5%85%A5%E6%8C%87%E5%8D%97/01-%E5%BF%AB%E9%80%9F%E5%BC%95%E5%AF%BC%EF%BC%88%E7%9B%B4%E8%BF%9E%E6%8E%A5%E5%85%A5%EF%BC%89.html)、[名词解释](https://iot.mi.com/new/doc/07-%E5%B8%B8%E7%94%A8%E4%BF%A1%E6%81%AF/01-%E5%90%8D%E8%AF%8D%E8%A7%A3%E9%87%8A.html)、[小米IoT协议规范](https://iot.mi.com/new/doc/03-%E5%B0%8F%E7%B1%B3IoT%E5%8D%8F%E8%AE%AE%E8%A7%84%E8%8C%83/01-%E6%A6%82%E8%BF%B0.html)为必看内容。

## Demo内容

### 开发板DEMO

DEMO开发板model为miot.light.demo1，使用小米WiFi模组MCHW03P，Arduino部分作为mcu与模组串口通信实现功能。

基础配置及MIoT-Spec功能定义已配置完成，包含灯的开关颜色功能、温湿度传感器、按键门铃功能，小米账号添加相应权限后可在开发者平台查看。

DEMO仅开放固件本地开发、调试、烧录，扩展程序本地开发、调试，不支持固件/扩展程序上传及上线。

### 固件开发

使用小米通用模组接入设备，相关完整文档请查看[通用模组标准协议接入指南](https://iot.mi.com/new/doc/04-%E5%B5%8C%E5%85%A5%E5%BC%8F%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/03-WiFi%E2%80%94%E9%80%9A%E7%94%A8%E6%A8%A1%E7%BB%84%E6%8E%A5%E5%85%A5/01-%E9%80%9A%E7%94%A8%E6%A8%A1%E7%BB%84%E6%A0%87%E5%87%86%E5%8D%8F%E8%AE%AE%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97.html)。

DEMO提供基于Arduino开发板的mcu固件，详情请参考[Arduino_MIoT接入文档](https://github.com/MiEcosystem/miio_open/blob/master/arduino/Arduino_Firmware.md)。

### 扩展程序开发

可通过扩展程序在米家App中控制设备、调用设备功能，扩展程序完整文档请查看[米家扩展程序开发指南](https://iot.mi.com/new/doc/05-%E7%B1%B3%E5%AE%B6%E6%89%A9%E5%B1%95%E7%A8%8B%E5%BA%8F%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/01-%E5%89%8D%E8%A8%80.html)。

DEMO提供对应扩展程序，请查看[PluginDEMO](https://github.com/MiEcosystem/MHCommonPlugin/tree/master/com.miot.light.demo1)，需使用已开通权限github账号。

### 更多内容

绑定开发板后，可通过“小爱同学”尝试控制灯的开关，如：“小爱同学，开灯”。（此功能仅供尝试）
