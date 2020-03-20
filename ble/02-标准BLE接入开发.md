# 标准BLE接入开发

*本文用于指导产品开发者实现标准BLE产品接入*

## 背景介绍

### 强弱绑定
强弱绑定关系是服务器端的逻辑，与固件端开发无关。

开发者可以根据自家的产品特点，开放平台创建产品后，在**基础功能配置**页面选择合适的绑定方式。
#### 强绑定
​设备绑定账号后，其他人再尝试绑定时，会提示设备已经被其它用户绑定。即**我不删除，谁都绑不走**。
#### 弱绑定
​设备绑定账号后，其他人再尝试绑定时，能够绑定成功，原账号刷新列表，设备消失。即**谁最后绑定就是谁的**。

### 配对方式

开发者同样需要在开发者平台**基础功能配置**页面选择蓝牙配对方式，目前有三种配对方式：APP确认即配对，RSSI符合即配对，设备确认即配对。

- APP确认即配对：当附近有多个设备时，由APP选择与哪个设备进行配对。
- RSSI符合即配对：要求设备靠近才能配对，选择此方式需要在开发者平台填写RSSI阈值，信号强度高于该阈值则可开始配对。
- 设备确认即配对：要求在设备端进行确认操作（如按键）才能开始配对。

开发固件端代码时请注意：

当选择APP确认配对或RSSI符合配对时，solicited位置0。

当选择设备确认配对时：
- 首先，正常广播的MiBeacon中solicited位为0。
- 当用户触发（如按键），将MiBeacon中solicited位修改为1，持续2~3秒后恢复为0。
- 当APP检测到设备的solicited位为1后，才进行连接，开始认证流程。

solicited 标志位请参考[米家BLE MiBeacon协议](https://iot.mi.com/new/doc/embedded-development/ble/ble-mibeacon.html)中的说明：0表示无操作；1表示请求 APP 进行注册绑定。当用户在开发者平台选择设备确认配对时才有效，否则置0。

**注意：开发者只需要调用advertising_init()接口，参数传入0/1即可修改蓝牙广播中的solicited标志位**，具体可参考Demo例程中advertising_init()接口的调用方法。

### 注册与登录

**注册**表示未绑定设备通过APP进行绑定。

**登录**表示用户进入APP插件与已绑定设备进行交互。

## 接入米家

### 已支持的芯片方案
米家提供了基于开发板实现的标准接入 Demo Project。

Demo Project GitHub 地址如下：（请按照各分支下 README 文档，完成工程导入）

| 芯片厂商      | 芯片平台       | Demo Project                                                       |
| :----------- | :-------       | :----------------------------------------------------------------- |
| Nordic       | 51 Series      | https://github.com/MiEcosystem/mijia_ble_standard/tree/nordic_legacy|
| Nordic       | 52 Series      | https://github.com/MiEcosystem/mijia_ble_standard/tree/nordic      |
| Silicon Labs | BG13           | https://github.com/MiEcosystem/mijia_ble_standard/tree/silabs      |
| Telink       | TLSR825x       | https://github.com/MiEcosystem/mijia_ble_standard/tree/telink      |
| Realtek      | RTL8762        | https://github.com/MiEcosystem/mijia_ble_standard/tree/realtek     |
| Dialog       | DA145xx        | https://github.com/MiEcosystem/mijia_ble_standard/tree/Dialog      |
| Dialog       | DA146xx        | https://github.com/MiEcosystem/mijia_ble_standard/tree/da146xx     |

目前米家只提供了对主流芯片的支持，如果产品厂商确认要选用**其它家**的芯片，可以向小米产品经理提起申请，说明需要选用其它芯片的原因，如果申请通过，米家会考虑支持并适配该芯片。

**注意** 厂商请勿自行适配芯片，出现问题小米不保证有人力配合联调。

### 接入步骤
标准BLE接入方式的软件SDK，由三部分组成：芯片原厂SDK，米家标准认证库，米家标准认证示例Demo。具体步骤如下：

- [米家标准认证库](https://github.com/MiEcosystem/mijia_ble_libs)是私有工程，产品开发者首先需联系小米产品经理，申请获取米家标准认证库的权限。**注意，每个新品的接入都要单独申请认证库的权限，一个产品申请一次**，如果发现未经申请的新品接入，米家有权利拒绝产品上线。
- **申请标准认证库请发送邮件至 小米IoT平台开放窗口 <miot-openhome@xiaomi.com>，格式请参考常用信息->开发者反馈指引。**
- 根据芯片选型，下载芯片厂商提供的SDK，各分支下有对应的下载地址链接或下载方法。
- 下载[米家标准认证示例Demo](https://github.com/MiEcosystem/mijia_ble_standard)
  - 按照各分支下的README说明，进入到芯片原厂SDK的指定目录
  - 命令行执行 `git clone -b xxx(对应分支) --recursive https://github.com/MiEcosystem/mijia_ble_standard.git`。建议使用git clone --recursive命令，直接网站上下载会发现有些文件找不到，这是因为submodule没有下载成功
- 联系小米产品经理申请开通“小米蓝牙开发板”白名单。
- 按照各分支下README说明中的路径导入工程，编译固件，下载固件到硬件板卡。
- 打开米家APP，开启蓝牙，发现附近的设备，点击“小米蓝牙开发板”，可成功连接登陆使用，表示已完成米家认证流程。
- 后续的产品开发中，可修改Demo工程中的PRODUCT_ID宏定义（定义在custom_mi_config.h文件中）为自己实际的产品id（Demo例程中PRODUCT_ID定义为156，表示小米蓝牙开发板），进行实际产品的开发。
- **注意事项**
  - 向网关发送事件需要频繁读写flash，请在开发过程中务必测试flash的可靠性，请测试flash key-value读写达到100000次以上
  - 向网关发送事件请直接调用mibeacon_obj_enque()接口，不允许自行拼包发送

### 应用层开发

应用开发需要关注custom_mi_config.h文件，用户可自定义文件中的配置，Demo工程中已经将 `CUSTOMIZED_MI_CONFIG_FILE=<custom_mi_config.h>` 添加到了工程的 `Preprocesser symbols`中。

custom_mi_config.h文件中的宏定义如下表所示

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

*<注> 如果缺省宏定义，会采用 mi_config.h 中默认值。例如，不定义 PRODUCT_ID，则默认为 156 小米蓝牙开发板，此产品为白名单用户可见。（如需使用此 PID 进行功能验证，请申请白名单权限）*

如需查看 log 信息，则需要将下列宏定义添加到工程 `Preprocesser symbols` 中。

| 宏名                       | 作用                                         |
| :------------------------ | :------------------------------------------- |
| MI_LOG_ENABLED            | 打开调试log，仅在调试阶段使用，量产时应去除        |
| MI_ASSERT                 | MI_LOG_ENABLED 打开时才有效，进行 assert 检查   |

烧写固件到产品中，产品侧已支持米家标准认证功能，可以使用米家 APP 进行功能验证。具体操作如下：
* 开启蓝牙，打开米家 APP，点击右上角 ➕ 发现附近的设备；（如果没有出现待测产品，请检查 MiBeacon 广播格式以及产品 PID 是否正确。）
* 点击待测产品，开始注册绑定。（产品侧 log 会提示连接状态等信息）
* 注册绑定成功后，即进入产品插件页（如果 APP 插件暂未上传，可以跳过此步骤。）
* 退出插件，再次进入，米家 APP 将进行登录操作。
* 重置方法，目前需要两步
    - 米家 APP 里删除设备
    - 设备上长按重置按钮

APP发现页面无法发现设备存在以下几种情况：
* 设备端已重置，但APP端未删除设备，APP的发现页面不会显示还在本机设备列表中的设备，请先在APP端也删除设备再重试。
* APP端已删除设备，设备端未重置，对于目前最新的V5版本MiBeacon协议，APP会判断设备发出MiBeacon中的“已注册”标志位，扫到“已注册”的设备，APP的发现页面不会显示，这种情况下请重置设备再重试。对于无法重置的设备（如蓝牙温湿度计），请修改工程中的宏定义**HAVE_RESET_BUTTON**为0，这样设备端发出的MiBeacon的“已注册”标志位就一直为0。
* APP端已删除设备，设备端也已经重置，请确认设备是否已上线或者用户的小米ID是否在设备的白名单中。调试Demo工程请联系小米产品经理申请开通“小米蓝牙开发板”白名单。

认证完成后，产品便具备安全通信的能力。产品可以通过发送广播或建立连接进行加密数据传输。

#### 广播上报
BLE 是一种近场通信方式，如果要实现消息的远程上报，则需要搭配一个能够连接互联网的桥接设备，这个设备称为蓝牙网关。

产品如果需要通过蓝牙网关向服务器上报事件或属性，开发者请先联系对接的产品经理，确认子设备要上报哪些事件和属性，然后在米家服务器端进行配置。目前申请Object ID流程已规划线上化，敬请期待。

米家定义了两类广播消息：事件和状态。**事件**上报不存在频率限制，具有高实时性，可以用来触发自动化场景，比如开门事件触发开灯；**状态**上报存在频率限制，具有高延迟，适用于周期或缓慢变化的数据，比如环境温度。

请直接使用 `mibeacon_obj_enque()` 接口，将待发送消息放入发送队列，设备将按照设置的间隔进行多次广播。每条消息的发送间隔和总时长可以通过宏定义修改，具体参数参考custom_mi_config.h文件中的宏定义。
[示例代码](https://github.com/MiEcosystem/mijia_ble_standard/blob/65f0093ebaa19a58a0e43f108be321f675a8ce99/main.c#L478-L484)

**不允许**开发者调用系统广播函数广播MiBeacon。**必须采用此接口**，小米在其中定义了重发次数和重发间隔，确保事件可以被网关收到。

#### 米家透传服务
建立连接后，双方可以通过 GATT 服务进行数据传输。米家透传服务作为一个示例，演示如何在一个自定义服务上实现加密传输。

1.参考 `stdio_service_init()` 创建数据传输示例服务，注册回调函数，以接收 APP 数据。(示例服务创建时使用了 mijia ble api, 开发者可直接使用原厂 SDK API)
[示例代码](https://github.com/MiEcosystem/mijia_ble_libs/blob/3b733870ca186662761f153f850537bae022fb5d/mijia_profiles/stdio_service_server.c#L168-L234)

2.建立连接并完成米家登陆后，双方会生成会话密钥。在发送数据前，先使用 `mi_session_encrypt()` 加密数据，再发送。参考 `stdio_tx()` 实现：
[示例代码](https://github.com/MiEcosystem/mijia_ble_libs/blob/3b733870ca186662761f153f850537bae022fb5d/mijia_profiles/stdio_service_server.c#L237-L261)

3.接收加密数据后，调用 `get_mi_authorization()` 判断设备当前登录状态，若已登录使用 `mi_session_decrypt()` 解密数据，再上报应用层处理。参考 stdio 服务 `on_write_permit()` 实现：
[示例代码](https://github.com/MiEcosystem/mijia_ble_libs/blob/3b733870ca186662761f153f850537bae022fb5d/mijia_profiles/stdio_service_server.c#L76-L96)

#### OTA实现
mijia统一OTA目前已经支持Nordic、Silabs、realtek、telink芯片平台，请check最新Demo工程，并保证在固件工程中将USE_MIBLE_OTA宏定义打开即可使用。

开发者也可以自行实现OTA的蓝牙通路，同样也可完成APP端从开放平台下载固件，再通过蓝牙连接将固件包传输到设备端。

对于带有外部MCU设备（例如门锁设备）的OTA方案，OTA只是实现将整个的固件包下载到设备端，设备端可以再自行拆分，蓝牙和MCU分别进行升级。

#### 开发者需要关注的API
| 函数名 | 功能 | 说明 |
| :----- | :--- | :--- |
| mi_session_encrypt       | 加密函数             |登陆成功后，设备和手机之间建立了加密通道，负责对传输数据加密   |
| mi_session_decrypt       | 解密函数             |负责对传输数据解密  |
| mibeacon_obj_enque       | 事件/属性信息上报    |  发送带有object的MiBeacon    |

<注> `mibeacon_obj_enque`的使用方法：搭配蓝牙网关使用，开发者调用mibeacon_obj_enque将要发送的object放入发送队列，之后libs自动发送带有object的MiBeacon。**不允许**开发者调用系统广播函数广播MiBeacon。**必须采用此接口**，小米在其中定义了重发次数和重发间隔，确保事件可以被网关收到。

## 空白工程集成米家标准认证库

*如果开发者在 Demo Project 基础上进行开发，可以略过本章*

### 软件环境
* IDE/toolchains (根据芯片 SDK 要求进行安装，如遇问题可咨询芯片原厂 FAE)
* [JLink](https://www.segger.com/downloads/jlink/)
* [Git](https://git-scm.com/downloads)

### 硬件环境
硬件平台可以选用芯片原厂的开发板，或使用在研产品 PCBA。推荐使用原厂开发板环境进行功能验证。完成功能验证后再移植到在研产品 PCBA。

### 代码集成

Demo Project 代码结构：

![高安全级 BLE 产品软件框架](./pics/secure-auth.png)

如上图所示，标准 BLE 产品接入需要用到以下代码：
* SoC SDK & 协议栈
* [mijia ble api](https://github.com/MiEcosystem/mijia_ble_api)
* [mijia ble libs](https://github.com/MiEcosystem/mijia_ble_libs.git)

其中，SoC stack & SDK 由芯片原厂提供，米家会推荐已验证的 SDK 版本；
mijia ble api & libs 由米家提供，以源码形式托管在 GitHub 上。推荐使用 git submodule 方式将米家代码集成到自己仓库中，以便后续通过 submodule update 方式进行更新。mijia ble api 为开源仓库，无需申请权限即可访问。mijia ble libs 为私有仓库，需申请访问权限。（请联系产品接口人）

*以 nordic 为例*

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

*<注> libs 引用的 `./third_party/` 路径下的第三方开源代码（例如， mbedtls，RTT），可能在 SoC SDK 中已经包含。当工程编译出现重复定义冲突时，开发者需要去掉 libs 中重复的文件。（比如，原厂工程中已经包含 SEGGER RTT 文件，就无需再添加 `./third_party/SEGGER_RTT` 路径下源码）*

### 米家认证
*进行以下步骤前请**先阅读** mijia ble libs [**使用手册**](https://github.com/MiEcosystem/mijia_ble_libs/blob/master/readme.md)*

标准 BLE 接入设备需支持 **米家标准认证协议** `standard auth`，该功能已在 `mijia ble libs` 中集成，使用方法如下：

1.芯片运行环境初始化（包括芯片电源，时钟，GPIO，TIMER，BLE stack 等功能模块）。

2.实现认证事件处理函数 `mi_schd_event_handler()`，进行 `mi_scheduler_init()` 初始化。（`mi_schd_event_handler()` 在收到 SCHD_EVT_OOB_REQUEST 事件的 30 秒内，产品端根据自身 IO 能力获取 OOB 数据，再调用 `mi_schd_oob_rsp()` 向 `mi_schduler` 提供所需 OOB 数据。）
[示例代码](https://github.com/MiEcosystem/mijia_ble_standard/blob/65f0093ebaa19a58a0e43f108be321f675a8ce99/main.c#L644-L670)

3.初始化后，调用 `mi_scheduler_start(SYS_KEY_RESTORE)` 尝试读取绑定信息，执行结果会通过 `mi_schd_event_handler()` 传参返回。如果已重置，将收到 SCHD_EVT_KEY_NOT_FOUND 事件。
[示例代码](https://github.com/MiEcosystem/mijia_ble_standard/blob/65f0093ebaa19a58a0e43f108be321f675a8ce99/main.c#L703-L705)

4.调用 `mi_service_init()` 创建米家服务。米家服务提供认证数据传输通道，服务缺失将导致认证失败。

5.使用 `mibeacon_adv_data_set()` 设置广播数据：第一个参数 is_solicited 用于请求手机进行绑定，比如，当特定按键按下后 5 秒内设置为 1，请求手机进行绑定。第二个参数 采用 BLE 接入无须填写。如果用户要发送自定义数据，需先按照标准蓝牙广播格式编码，再通过 user data 参数传入。
**注意 请勿使用 SDK 中 ADV API 直接修改广播数据。以免 MiBeacon 广播数据被覆盖，造成产品无法被米家 APP 发现。**
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
