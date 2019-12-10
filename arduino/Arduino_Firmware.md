## Arduino_MIoT接入文档

- ***Arduino 开发环境安装：http://arduino.nxez.com/2016/12/17/arduino-development-environment-introduction.html***

- ***接入小米IoT开发者平台：https://iot.mi.com/***


#### 1 背景介绍

- 小米开发板旨在帮助您了解小米IoT开发者平台的开发模式，帮助您更快地进入产品开发阶段

#### 2 硬件框架

***开发板由一块Arduino开发板通过串口和一块嵌入了小米智能模组的配对板组成。开发前请将两块电路板通过插针连接起来***

 ![](https://github.com/MiEcosystem/miio_open/blob/master/arduino/pictures/arduino.jpg)

| Arduino | MIoT Broad | 功能 |
| - | - | - |
| USB线连接 | | 供电、烧写代码 |
| pin1/pin0/GND | UA1_RXD/UA1_TXD/GND | MCU MIIO模块串口通讯 |
| pin4 | DHT11 | 数字采集温湿度 |
| pin9/pin10/pin11 | RGB LED | 控制RGB灯 |
| pin7 | Button | 采集按键事件，上报云端 |

#### 3 烧写运行

拿到开发板后，您需要向Arduino开发板烧写程序，具体操作如下

- 下载[固件程序](：https://github.com/)

- 下载[Arduino软件](http://arduino.cc/en/Main/Software/)并安装

- 导入Arduino [DHT11库文件导入](https://www.arduino.cn/thread-31720-1-1.html)

- 将配对板右上角的拨码开关拨至左侧PROG，通过USB线连接Arduino板和电脑

- 打开miio_tc_demo.ino，编译后写入开发板。请注意在“工具”中选择正确的开发板类型（Arduino Uno）和正确的串口(可在电脑资源管理中看到对应串口，如没有驱动选装CP2102驱动)

- 将配对板右上角的拨码开关拨回右侧，回到普通工作状态，从电脑拔除开发板完成烧写。可参考(https://www.arduino.cn/thread-76154-1-1.html)


#### 4 设备配网

使用WiFi进行工作的智能设备，需要连接上路由器、并和用户账号建立绑定关系后才能工作。您可以通过小米开发板来体验这个过程

- 请首先使用手机下载 米家App

- 打开米家App，并使用您的开发者账号进行登录

- 打开米家App，点击右上角的“加号”，并按照提示连接小米开发板

- 在完成小米开发板的快速连接后，您的设备列表中会出现一个新的设备，您就可以通过手机对开发板做一些简单操作了。


#### 5 MIOT-SPEC设备功能定义

小米IoT有标准的设备功能描述方式，主要有以下几个主要概念组成

| 概念 | 解释 |
| - | - |
| 属性（Property） | 功能单元之一，单一的功能操作可以定义为属性，属性支持被查询、控制和订阅，如灯的开关 |
| 方法（Action） | 功能单元之一，主要用于向设备下发组合控制指令，该功能即可以定义为方法，方法可支持同时下发多个属性（参数），如电饭煲的开始煮饭 |
| 事件（Event） | 功能单元之一，主要用于设备向云端上报组合数据，该功能即可以定义为事件，事件可支持同时上报多个属性（参数），如门锁被打开 |
| 服务（Service） | 服务代表了一类互相有联系的功能单元的合集，如环境检测服务、电池管理服务，服务中包括属性、方法、事件，可以描述该服务中包括的具体功能 |
| 设备（Device） | 即产品本身，设备由多个服务聚合而成，如支持充电的风扇会有风扇服务、电池管理服务、环境检测服务等 |
 
 miot.light.demo1设备功能定义：
 
 ![](https://github.com/MiEcosystem/miio_open/blob/master/arduino/pictures/miot-device-spec.jpg)
 
在小米开发者平台可编辑产品功能，实现不同的功能，具体可参考[创建产品](https://iot.mi.com/new/doc/03-%E5%B9%B3%E5%8F%B0%E4%BD%BF%E7%94%A8%E6%8C%87%E5%8D%97/02-%E6%99%BA%E8%83%BD%E7%A1%AC%E4%BB%B6%E7%9B%B4%E8%BF%9E%E6%8E%A5%E5%85%A5/02-%E5%88%9B%E5%BB%BA%E4%BA%A7%E5%93%81.html)

#### 6 代码理解

当您在开发过程中还未开发手机控制插件时，可以通过开放平台进行云端调试：

##### 打开小米智能硬件平台，在“开发平台>开发板管理”中，找到您绑定的开发板，并点击“调试”进入云端调试页面

***以下siid表示功能定义中的Service id，piid表示功能定义中的Property id，aiid 表示功能定义中的action id，eiid 表示功能定义中的event id，命令格式可参考[通用模组miot-spec支持资料](https://iot.mi.com/new/doc/04-%E5%B5%8C%E5%85%A5%E5%BC%8F%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97/03-WiFi%E2%80%94%E9%80%9A%E7%94%A8%E6%A8%A1%E7%BB%84%E6%8E%A5%E5%85%A5/01-%E9%80%9A%E7%94%A8%E6%A8%A1%E7%BB%84%E6%A0%87%E5%87%86%E5%8D%8F%E8%AE%AE%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97.html)***

![](https://github.com/MiEcosystem/miio_open/blob/master/arduino/pictures/msg_flow.jpg)

##### 下行消息基本流程如下

整个过程为：Cloud->小米模组->Arduino->RGB灯

- 云端向小米模组下发命令来设置RGB为蓝色，其中value赋值遵循rgb(RGB24)格式：0x00 ff(red) ff(green) ff(blue)，所以255等效于0x00 00 00 ff即蓝色

```c
{"from":"","id":xxx,"method":"set_properties","params":[{"did":"xxx","piid":4,"siid":2,"value":255}]}
```

- 小米模组将云端发来的包含method的json串解析为文本命令，供Arduino通过串口获取

- Arduino 对小米模组通过串口每隔200ms发送文本命令，来获取转化后的云端命令

```c
get_down
```

- Arduino 获取到从小米模组通过串口发来的rgb文本命令

- Arduino执行命令，将RGB蓝灯点亮

```c
set_rgb(255)
```

##### 上行消息基本流程如下

整个过程为：门铃button->Arduino->小米模组->Cloud

- 门铃button被按下时触发Arduino响应门铃事件

- Arduino将获取到的门铃事件，通过串口文本形式通知小米模组

```c
event_occured 4 1 1 "button_pressed"
```

- 小米模组将反应门铃事件的字符串”button_pressed”以及该事件的siid、eiid、piid封装进上报云端的json包中，上报云端

```c
{"id":xxx,"method":"event_occured","params":{"did":"xxx","siid":4,"eiid":1,"arguments":[{"piid":1,"value":"button_pressed"}]},"retry":0,"timestamp":1574761781,"tick":22763290}
```

#### 7 硬件连线与开发拓展

这里提供了Arduino与外设及小米智能模组的连接方式，您可以研读Arduino程序并对程序进行修改

- 连接Arduino与PC：使用USB线连接(用来供电、烧写代码)

- 连接Arduino与MIIO芯片：Arduino的UART(pin 1 ,pin 0 ,pin GND)->MIIO 的UART1(UA1_RXD, UA1_TXD,GND)

- 连接Arduino与DHT11 ：Arduino的5V ,pin 4,GND->DHT11的V,D,G

- 连接Arduino与RGB LEDS: Arduino的pin 9,pin 10,pin 11,GND->RGB LEDS的R,G,B,V-

- 连接Arduino与Button:pin7-->button，给予高电平表示按下

- 当您希望对开发板的硬件进行更多拓展时，可以使用飞线连接小米模组和Arduino开发板，同时将GPIO空出以接入其他外部设备。连接方法如下图所示：

![](https://github.com/MiEcosystem/miio_open/blob/master/arduino/pictures/Connection.jpg)

***Arduino扩展学习可参考：https://www.arduino.cn/thread-1066-1-1.html***
