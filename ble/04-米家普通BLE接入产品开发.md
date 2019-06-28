# 米家普通BLE接入产品开发

*本文用于指导产品厂商利用已经支持米家BLE接入的芯片开发产品*

<br/>

## 接入米家

普通BLE接入方式的软件SDK，由三部分组成：芯片原厂SDK，米家标准认证库，米家示例Demo。具体步骤如下：

1. 根据芯片选型选择对应的分支，下载芯片厂商提供的SDK，各分支下有对应的下载地址链接或下载方式。
2. 下载[米家标准认证示例demo](https://github.com/MiEcosystem/mijia_ble)。**注意**:
   - 按照各分支下的README说明，进入到芯片原厂SDK的指定目录
   - 命令行执行`git clone -b xxx(对应分支) --recursive https://github.com/MiEcosystem/mijia_ble.git`。建议使用git clone --recursive命令，直接网站上下载会发现有些文件找不到，这是因为submodule没有下载成功
3. 联系[小米IoT开发者平台](https://iot.mi.com/)申请米家蓝牙标准认证库，详见[开发者反馈指引](https://iot.mi.com/guide.html#id=80)，将申请得到的`mijia_std_authen_keil.lib`拷贝到各分支下README说明中指定的文件夹下。
4. 联系小米产品经理申请开通“小米蓝牙开发板”白名单。
5. 按照各分支下README说明中的路径导入工程，编译固件，下载固件到测试开发板。
6. 打开米家APP，开启蓝牙，发现附近的设备，点击“小米蓝牙开发板”，可成功连接登陆使用，表示已完成米家认证流程。
7. 后续的产品开发中，可修改demo例程中dev_info结构体中的pid字段为自己实际的产品id（demo例程中pid为156，表示小米蓝牙开发板），进行实际产品的开发。

已支持芯片平台列表：

| 芯片品牌     | 芯片型号        | SDK                                                          |
|:------------ |:--------------- |:------------------------------------------------------------ |
| Nordic       | 51/52           | https://github.com/MiEcosystem/mijia_ble/tree/Nordic         |
| Nordic       | 52(SDK15.2)     | https://github.com/MiEcosystem/mijia_ble/tree/Nordic_SDK15.2 |
| Dialog       | DA14585         | https://github.com/MiEcosystem/mijia_ble/tree/Dialog         |
| NXP          | QN908x / QN902x | https://github.com/MiEcosystem/mijia_ble/tree/NXP            |
| Cypress      | 20706           | https://github.com/MiEcosystem/mijia_ble/tree/cypress        |
| ST           | BlueNRG-1/2     | https://github.com/MiEcosystem/mijia_ble/tree/ST             |
| Silicon Labs | BG13            | https://github.com/MiEcosystem/mijia_ble/tree/Silabs         |
| Telink       | 826x            | https://github.com/MiEcosystem/mijia_ble/tree/telink         |
| TI           | CC2640R2        | https://github.com/MiEcosystem/mijia_ble/tree/ti             |
| Chipsea      | CST92F30        | https://github.com/MiEcosystem/mijia_ble/tree/Chipsea        |

## 米家标准认证库说明

需要接入到米家系统的BLE设备，连接米家APP时需要先经过认证，mible_std_authen_xxxx.lib库会在蓝牙设备端建立MiService服务，用于认证交互。该库适用于普通安全等级的米家接入设备。

首先开发者需要完成必要的初始化工作，如蓝牙协议栈初始化、timer初始化等，使得米家通用蓝牙接口可以正常工作。

开发者需仔细研究BLE连接开始前MiBeacon是如何配置的。更多信息请参考[米家BLE MiBeacon协议](https://github.com/MiEcosystem/miio_open/blob/master/ble/02-%E7%B1%B3%E5%AE%B6BLE%20MiBeacon%E5%8D%8F%E8%AE%AE.md)。

开发产品需关注的函数接口和事件回调接口都定义在`mible_server.h`。

| 函数名                      | 功能                         | 说明                  |
|:--------------------------- |:---------------------------- | :------------------- |
| mible_server_info_init      | 初始化设备信息               | 初始化一下参数：<br />1、强/弱绑定关系，需与开发者平台上强弱绑定配置一致。强弱绑定概念请查询开发者平台，默认弱绑定<br />2、产品id，需与开发者平台上产品ID配置一致<br />3、版本号（四位数字字符如”1234”）<br />4、是否需在设备端确认以发起或拒绝绑定（如按键）<br />5、模式，如产品为遥控器则选择MODE_REMOTE，否则选MODE_STANDARD  |
| mible_server_miservice_init | 初始化MiService              | 用于初始化MiService(UUID 0xFE95)，在初始化阶段调用 |
| mible_std_auth_evt_register | 注册回调函数                | 具体请参考下面四行具体内容              |
| mible_service_init_cmp      | MiService初始化完成回调函数 | 当MiService初始化完成，会调用此函数。 |
| mible_connected             | 连接回调函数                 | 当设备与APP建立连接，会调用此函数。    |
| mible_disconnected          | 断开连接回调函数             | 当设备与APP断开连接，会调用此函数。     |
| mible_bonding_evt_callback  | 认证结果回调函数             | 设备第一次与米家APP连接时，进入**绑定**流程，此函数返回绑定成功或失败的结果；绑定成功后（设备输入手机app的账号名下），以后设备每次与APP建立连接，都进行**登录**流程，此函数返回登录成功或失败的结果。**建立连接后，在未绑定或登录成功前，设备端不能允许任何对设备的操作**，如读写Characteristic等。建立连接后，若未成功进行绑定或登陆，超时10s后会自动断开连接。 |

## 更多

`mible_server.h`还规定了另外几个接口。

| 函数名                      | 功能                         | 说明                  |
|:--------------------------- |:---------------------------- | :------------------- |
| mible_std_auth_permit_bind  | 允许设备绑定                 | 如果mible_server_info_init中`strict_bind_confirm == 1`，表示在绑定流程开始时，必须在设备端通过某种方式（如按键）确认才会真正的发起绑定流程。如需要确认，则必须配合mible_std_auth_permit_bind和mible_std_auth_forbid_bind使用；如不需要确认，则表示随时可进行绑定。例如可以在按键回调中调用mible_std_auth_permit_bind，在超时回调中调用mible_std_auth_forbid_bind。                 |
| mible_std_auth_forbid_bind  | 禁止设备绑定                 | 同上                 |
| mible_std_server_encrypt    | 数据加密                     | 开发者可以自定义BLE Service和Characteristic，进行BLE数据传输。传输的具体内容可以使用mible_std_server_encrypt和mible_std_server_decrypt进行加密和解密，APP端也有类似的接口                  |
| mible_std_server_decrypt    | 数据解密                     | 同上                  |
| mible_std_auth_factory_reset| 恢复至出厂状态               | 可以采用mible_std_auth_factory_reset删除米家认证相关数据，将设备恢复至出厂状态。                  |

`mible_server.h`还有一些参数可供应用层修改：

| 宏定义                              | 说明           |
| :---------------------------------- | :------------- |
| MIBLE_STD_SERVER_CONN_MAX_INTERVAL  | 最大连接间隔   |
| MIBLE_STD_SERVER_CONN_MIN_INTERVAL  | 最小连接间隔   |
| MIBLE_STD_SERVER_CONN_SLAVE_LATENCY | 从设备连接延迟 |
| MIBLE_STD_SERVER_CONN_SUP_TIMEOUT   | 连接超时时间   |

开发者可以操作MiBeacon上报事件或属性。米家所有BLE网关都可以接收MiBeacon并转发给米家服务器。后续可以根据存储在米家服务器的事件或属性信息在APP上显示或进行自动化配置。为了确保网关能够成功接收MiBeacon，同一事件或属性需重复多次，更多信息请参考[米家BLE MiBeacon协议](https://github.com/MiEcosystem/miio_open/blob/master/ble/02-%E7%B1%B3%E5%AE%B6BLE%20MiBeacon%E5%8D%8F%E8%AE%AE.md)。

固件OTA请联系芯片厂商。

## FAQ

#### Q: 有问题怎么办？

A: 关于产品定义或[小米IoT开发者平台](https://iot.mi.com/)的问题，请联系米家产品经理。技术问题请区分是芯片开发的问题还是米家接入的问题。如果是芯片开发的问题，请联系厂商，如果是米家接入的问题，请搜索[米家标准认证示例demo](https://github.com/MiEcosystem/mijia_ble)相关issue，看是否有类似的问题。如果没有，请提交新issue。

#### Q: 产品的pid如何获取？

A: 产品的pid是在[小米IoT开发者平台](https://iot.mi.com/)上注册产品时生成的，在demo中pid = 156，是一个弱绑定的蓝牙开发板产品，用于测试。 还有一个强绑定的蓝牙开发板产品pid = 930，此两个产品类型仅用于开发者做初期测试。在真正的产品开发中，开发者应需要pid及强弱绑定关系，与在[小米IoT开发者平台](https://iot.mi.com/)上注册产品时的信息保持一致。强弱绑定的具体定义，可参考小米IoT开发者平台。

#### Q: 当周边有多个相同设备时(如多个蓝牙温湿度传感器)，怎么指定哪个设备需要被绑定？

A: 开发者需要在开发者平台选择蓝牙配对方式，目前有三种配对方式：APP选择配对，RSSI符合配对，设备确认配对。当选择APP选择配对或RSSI符合配对时，solicited置0。当选择设备确认配对时，正常广播的MiBeacon中solicited位为0，当用户触发，如按键，MiBeacon中solicited位变为1，持续2~3秒后恢复为0。此时手机可以发现solicited为1的设备并连接开始认证流程。注意不要与`mible_server.h`文件中device_info结构体中的strict_bind_confirm混淆。solicited是为了确认周边相同设备时，绑定哪个设备，而strict_bind_confirm是为了在设备端上确认是否可以绑定这个设备。此位原名称位bindingcfm，重命名为solicited。
