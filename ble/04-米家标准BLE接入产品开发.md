# 米家标准BLE接入产品开发

*本文用于指导产品开发者实现标准BLE产品接入*

## 0.准备工作

### 软件环境：
* IDE/toolchains (根据芯片 SDK 要求进行安装，如遇问题可咨询芯片原厂 FAE)
* [JLink](https://www.segger.com/downloads/jlink/)
* [Git](https://git-scm.com/downloads)

### 硬件环境：
硬件平台可以选用芯片原厂的开发板，或使用在研产品 PCBA。推荐使用原厂开发板环境进行功能验证。完成功能验证后再移植到在研产品 PCBA。米家提供基于开发板实现的标准接入 Demo Project。<br>
Demo Project GitHub 地址如下：（请按照各分支下 README ，完成工程导入）

| 芯片厂商      | 芯片平台       | Demo Project                                                       |
| :----------- | :-------       | :----------------------------------------------------------------- |
| Nordic       | 51 Series      | https://github.com/MiEcosystem/mijia_ble_standard/tree/nordic_legacy|
| Nordic       | 52 Series      | https://github.com/MiEcosystem/mijia_ble_standard/tree/nordic      |
| Silicon Labs | BG13           | https://github.com/MiEcosystem/mijia_ble_standard/tree/silabs      |
| Telink       | TLSR8253       | https://github.com/MiEcosystem/mijia_ble_standard/tree/telink      |


## 1.代码集成
(*如果开发者在 Demo Project 基础上进行开发，可以跳过本节 1~5 步*)

Demo Project 代码结构：

![高安全级 BLE 产品软件框架](./pics/secure-auth.png)

如上图所示，标准 BLE 产品接入需要用到以下代码：
* SoC SDK & 协议栈
* [mijia ble api](https://github.com/MiEcosystem/mijia_ble_api)
* [mijia ble libs](https://github.com/MiEcosystem/mijia_ble_libs.git)

其中，SoC stack & SDK 由芯片原厂提供，米家会推荐已验证的 SDK 版本；
mijia ble api & libs 由米家提供，以源码形式托管在 GitHub 上。推荐使用 git submodule 方式将米家代码集成到自己仓库中，以便后续通过 submodule update 方式进行更新。mijia ble api 为开源仓库，无需申请权限即可访问。mijia ble libs 为私有仓库，需申请访问权限。（请联系产品接口人）

### 集成方法如下：
（将以 nordic 为例，示例如何具体操作）

1.下载芯片原厂 SDK。(SDK 版本根据 mijia ble api 要求进行选择)
```
示例：从 nordic 官网下载 [SDK 15.2.0](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK/Download#infotabs)
```

2.按照芯片原厂 SDK 说明文档，新建 BLE 空工程。(如果已创建工程，可以跳过此步)
```
示例：nordic BLE 空工程在 `SDK\examples\ble_peripheral\ble_app_template` 目录下，复制一份 ble_app_template 并重命名，比如叫 stand_demo
```

3.在新建的 BLE 工程目录下创建 [Git](https://git-scm.com/) 本地仓库。(如果已有本地仓库，可以跳过此步)
```
示例：
$ git init
```

4.添加 mijia ble api 和 mijia ble libs 到本地仓库。添加 mijia ble api 时需选择对应芯片平台的分支。
```
示例：
$ git submodule add -b nordic https://github.com/MiEcosystem/mijia_ble_api.git
$ git submodule add -b master https://github.com/MiEcosystem/mijia_ble_libs.git
$ git commit -am "add submodules mijia ble api & libs"
```

5.添加 mijia ble api 和 mijia ble libs 路径下源码到 BLE 工程中，并将路径加入到工程的 `include path`。mijia ble api 会使用到 SoC SDK 中的 BLE，TIMER，FLASH 等功能，需要开发者将这些功能模块添加到工程中。 （详细资源占用情况及配置方法，请参考芯片对应分支下的 README）。添加下列路径下代码和库。（根据编译器和芯片 CPU 型号选择对应库文件）
```
mijia_ble_libs
├── common
│   ├── mible_beacon.c
│   ├── mible_crypto.c
│   ├── mible_rxfer.c
│   └── queue.c
├── cryptography
│   ├── mi_crypto.c
│   ├── mi_crypto_backend_mbedtls.c
│   └── mi_crypto_backend_uECC.c
├── mijia_profiles
│   ├── mi_service_server.c
│   └── stdio_service_server.c
├── standard_auth
│   ├── stand-auth-cortex-?.a (搭配 gcc 使用)
│   └── stand-auth-cortex-?.lib (搭配 armcc/keil 使用)
└── third_party
    ├── SEGGER_RTT
    │   ├── SEGGER_RTT.c
    │   └── SEGGER_RTT_printf.c
    ├── mbedtls
    │   ├── asn1parse.c
    │   ├── ccm.c
    │   └── sha256_hkdf.c
    ├── micro-ecc
    │   ├── micro_ecc_?.a
    │   ├── micro_ecc_?.lib
    │   └── uECC.c（如果缺少对应平台库文件，可使用源码代替）
    └── pt
        └── pt_misc.c
```

>[注意] libs 引用的 `./third_party/` 路径下的第三方开源代码（例如， mbedtls，RTT），可能在 SoC SDK 中已经包含。当工程编译出现重复定义冲突时，开发者需要去掉 libs 中重复的文件。（比如，原厂工程中已经包含 SEGGER RTT 文件，就无需再添加 `./third_party/SEGGER_RTT` 路径下源码）

6.创建自定义配置文件，在文件中添加下列宏定义，然后将 `CUSTOMIZED_MI_CONFIG_FILE=<your_config.h>` 添加到工程 `Preprocesser symbols`中；

| 宏名                       | 作用                                         |
| :------------------------ | :------------------------------------------- |
| CUSTOMIZED_MI_CONFIG_FILE | 自定义的 config 头文件                         |
| DEVELOPER_VERSION         | 开发者固件版本号                               |
| PRODUCT_ID                | 必须与在小米IoT开发者平台创建的产品 ID 相同       |
| MI_BLE_ENABLED            | 采用 BLE 协议接入                             |
| HAVE_MSC                  | 使用标准认证，无须外接认证芯片，定义为 0 即可      |
| HAVE_RESET_BUTTON         | 具有重置按键                                  |
| OOB_USE_NUMPAD_INPUT      | 绑定时，通过数字键盘确认，增强安全性              |
| OOB_USE_QR_CODE_OUT       | 绑定时，通过二维码确认，增强安全性                |
| OOB_USE_DISPLAT_OUT       | 绑定时，通过显示屏提示，增强安全性                |
| OBJ_QUEUE_SIZE            | mibeacon object 发送队列大小                  |
| OBJ_ADV_INTERVAL_MS       | 每条 mibeacon object 发送间隔                 |
| OBJ_ADV_TIMEOUT_MS        | 每条 mibeacon object 发送时长                 |

*<注1> 如果缺省宏定义，会采用 mi_config.h 中默认值。例如，不定义 PRODUCT_ID，则默认为 463 小米蓝牙开发板，此产品为白名单用户可见。（如需使用此 PID 进行功能验证，请申请白名单权限）*

7.如需查看 log 信息，下列宏定义添加到工程 `Preprocesser symbols` 中；

| 宏名                       | 作用                                         |
| :------------------------ | :------------------------------------------- |
| MI_LOG_ENABLED            | 打开调试log，仅在调试阶段使用，量产时应去除        |
| MI_ASSERT                 | MI_LOG_ENABLED 打开时才有效，进行 assert 检查   |


## 2.米家认证
(*进行以下步骤前请**先阅读** mijia ble libs [**使用手册**](https://github.com/MiEcosystem/mijia_ble_libs/blob/master/readme.md)*)

标准 BLE 接入设备需支持 **米家标准认证协议** `standard auth`，该功能已在 `mijia ble libs` 中集成，使用方法如下：

1.芯片运行环境初始化（包括芯片电源，时钟，GPIO，TIMER，BLE stack 等功能模块）。

2.实现认证事件处理函数 `mi_schd_event_handler()`，进行 `mi_scheduler_init()` 初始化。（`mi_schd_event_handler()` 在收到 SCHD_EVT_OOB_REQUEST 事件的 30 秒内，产品端根据自身 IO 能力获取 OOB 数据，再调用 `mi_schd_oob_rsp()` 向 `mi_schduler` 提供所需 OOB 数据。）
[示例代码](https://github.com/MiEcosystem/mijia_ble_standard/blob/65f0093ebaa19a58a0e43f108be321f675a8ce99/main.c#L644-L670)

3.初始化后，调用 `mi_scheduler_start(SYS_KEY_RESTORE)` 尝试读取绑定信息，执行结果会通过 `mi_schd_event_handler()` 传参返回。如果已重置，将收到 SCHD_EVT_KEY_NOT_FOUND 事件。
[示例代码](https://github.com/MiEcosystem/mijia_ble_standard/blob/65f0093ebaa19a58a0e43f108be321f675a8ce99/main.c#L703-L705)

4.调用 `mi_service_init()` 创建米家服务。米家服务提供认证数据传输通道，服务缺失将导致认证失败。

5.使用 `mibeacon_adv_data_set()` 设置广播数据：第一个参数 is_solicited 用于请求手机进行绑定，比如，当特定按键按下后 5 秒内设置为 1，请求手机进行绑定。第二个参数 采用 BLE 接入无须填写。如果用户要发送自定义数据，需先按照标准蓝牙广播格式编码，再通过 user data 参数传入。
**注意** 请勿使用 SDK 中 ADV API 直接修改广播数据。以免 mibeacon 广播数据被覆盖，造成产品无法被米家 App 发现。
[示例代码](https://github.com/MiEcosystem/mijia_ble_standard/blob/65f0093ebaa19a58a0e43f108be321f675a8ce99/main.c#L533-L545)

6.将 `mi_schd_process()` 添加到 main loop 中。

7.米家认证相关的事件，会通过 `mi_schd_event_handler()` 通知应用层：

| 事件名称                          | 含义                                         |
| :------------------------------- | :------------------------------------------ |
| SCHD_EVT_REG_SUCCESS             | 绑定注册成功                                  |
| SCHD_EVT_REG_FAILED              | 绑定注册失败                                  |
| SCHD_EVT_ADMIN_LOGIN_SUCCESS     | 登录成功                                     |
| SCHD_EVT_ADMIN_LOGIN_FAILED      | 登录失败                                     |
| SCHD_EVT_TIMEOUT                 | 操作超时                                     |
| SCHD_EVT_KEY_NOT_FOUND           | 未绑定                                       |
| SCHD_EVT_KEY_FOUND               | 已绑定                                       |
| SCHD_EVT_KEY_DEL_SUCC            | 重置成功                                     |
| SCHD_EVT_OOB_REQUEST             | 请求输入 OOB                                 |

此外，开发者还可以使用 `mi_scheduler_start()` 发送以下命令，进行本地系统管理：

| 命令名称                     | 含义                                        |
| :-------------------------- | :----------------------------------------- |
| SYS_KEY_RESTORE             | 读取绑定信息                                 |
| SYS_KEY_DELETE              | 删除绑定信息                                 |

经过以上步骤，产品侧已支持米家标准认证功能，可以使用米家 App 进行功能验证。window 平台具体操作如下：
* 打开 JLink RTT Viewer，选择对应芯片型号，连接待测产品。连接成功后，Terminal 0 下会显示产品侧 log 信息；
* 开启蓝牙，打开米家 App，点击右上角 ➕ 发现附近的设备；（如果没有出现待测产品，请检查 mibeacon 广播格式以及产品 PID 是否正确。）
* 点击待测产品，开始注册绑定。（产品侧 log 会提示连接状态等信息）
* 注册绑定成功后，即进入产品插件页（如果 App 插件暂未上传，可以跳过此步骤。）
* 退出插件，再次进入，米家 App 将进行登录操作。
* 重置方法，目前需要两步
    - 米家 App 里删除设备
    - 设备上长按重置按钮

## 3.应用层开发
认证完成后，产品便具备安全通信的能力。产品可以通过发送广播或建立连接进行加密数据传输。

### 广播方式
BLE 是一种近场通信方式，如果要实现消息的远程上报，则需要搭配一个能够连接互联网的桥接设备，这个设备称为蓝牙网关。

米家定义了两类广播消息 [object](https://github.com/MiEcosystem/miio_open/blob/master/ble/03-米家BLE%20Object协议.md)：事件和状态。**事件**上报不存在频率限制，具有高实时性，可以用来触发自动化场景，比如开门事件触发开灯；**状态**上报存在频率限制，具有高延迟，适用于周期或缓慢变化的数据，比如环境温度。使用 `mibeacon_obj_enque()` 将待发送消息放入发送队列，设备将按照设置的间隔进行多次广播。每条消息的发送间隔和总时长可以通过宏定义修改，具体参数参考 之前章节1.6 。
[示例代码](https://github.com/MiEcosystem/mijia_ble_standard/blob/65f0093ebaa19a58a0e43f108be321f675a8ce99/main.c#L478-L484)

### 连接方式
建立连接后，双方可以通过 GATT 服务进行数据传输。米家透传服务作为一个示例，演示如何在一个自定义服务上实现加密传输。

1.参考 `stdio_service_init()` 创建数据传输示例服务，注册回调函数，以接收 App 数据。(示例服务创建时使用了 mijia ble api, 开发者可直接使用原厂 SDK API)
[示例代码](https://github.com/MiEcosystem/mijia_ble_libs/blob/3b733870ca186662761f153f850537bae022fb5d/mijia_profiles/stdio_service_server.c#L168-L234)

2.建立连接并完成米家登陆后，双方会生成会话密钥。在发送数据前，先使用 `mi_session_encrypt()` 加密数据，再发送。参考 `stdio_tx()` 实现；
[示例代码](https://github.com/MiEcosystem/mijia_ble_libs/blob/3b733870ca186662761f153f850537bae022fb5d/mijia_profiles/stdio_service_server.c#L237-L261)

3.接收加密数据后，调用 `get_mi_authorization()` 判断设备当前登录状态，若已登录使用 `mi_session_decrypt()` 解密数据，再上报应用层处理。参考 stdio 服务 `on_write_permit()` 实现：
[示例代码](https://github.com/MiEcosystem/mijia_ble_libs/blob/3b733870ca186662761f153f850537bae022fb5d/mijia_profiles/stdio_service_server.c#L76-L96)

## 4.FAQ

#### Q: 有问题怎么办？
A: 关于产品定义或[小米IoT开发者平台](https://iot.mi.com/)的问题，请联系米家产品经理。技术问题请区分是芯片开发的问题还是米家接入的问题。如果是芯片开发的问题，请联系厂商 FAE，如果是米家接入的问题，请先搜索[米家 BLE 标准认证接入demo](https://github.com/MiEcosystem/mijia_ble_standard)相关 issue，看是否有类似的问题。如果没有，请按照模板创建新 issue（务必附带设备侧 log 信息）。

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

#### Q: 产品的pid如何获取？

A: 产品的pid是在[小米IoT开发者平台](https://iot.mi.com/)上注册产品时生成的，在demo中pid = 156，是一个弱绑定的蓝牙开发板产品，用于测试。 还有一个强绑定的蓝牙开发板产品pid = 930，此两个产品类型仅用于开发者做初期测试。在真正的产品开发中，开发者应需要pid及强弱绑定关系，与在[小米IoT开发者平台](https://iot.mi.com/)上注册产品时的信息保持一致。强弱绑定的具体定义，可参考小米IoT开发者平台。

#### Q: 同时发现多个同类产品时(如多个蓝牙温湿度传感器)，用户如何确定绑定哪个产品？

A: 开发者需要在开发者平台选择蓝牙配对方式，目前支持三种配对选择方式：
* 通过 RSSI 判断：发现多个同类产品时，选择信号最强的产品进行绑定。
* 设备选择即配对：产品需具有按键。当用户按下按键后，产品发送带有 solicited bit 广播包，米家 App 收到该广播包后会进行绑定。
* App 选择即配对：产品需具有字符显示能力，能够显示 MAC 地址最后 2 字节。用户在米家 App 内选择相同 MAC 的产品进行绑定。

#### Q: 产品如何OTA？

A: 目前没有统一的米家OTA机制，请联系芯片厂商。
