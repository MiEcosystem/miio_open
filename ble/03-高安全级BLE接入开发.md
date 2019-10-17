# 高安全级BLE接入开发

*本文用于指导产品开发者实现高安全级BLE产品接入*

## 接入米家

高安全级BLE接入方式的软件SDK，由三部分组成：芯片原厂SDK，米家高安全级认证库，米家智能门锁示例Demo。同时必须外接安全芯片。具体步骤如下：

1. [米家高安全级认证库](https://github.com/MiEcosystem/mijia_ble_libs)是私有工程，产品开发者首先需联系小米产品经理，增加获取高安全级认证库的权限。**申请高安全级认证库请发送邮件，格式请参考常用信息->开发者反馈指引。**
2. 根据芯片选型选择对应的分支，下载芯片厂商提供的SDK，各分支下有对应的下载地址链接或下载方式。
3. 下载[米家智能门锁示例demo](https://github.com/MiEcosystem/mijia_ble_secure)
   - 按照各分支下的README说明，进入到芯片原厂SDK的指定目录
   - 命令行执行`git clone -b xxx(对应分支) --recursive https://github.com/MiEcosystem/mijia_ble_secure.git`。建议使用git clone --recursive命令，直接网站上下载会发现有些文件找不到，这是因为submodule没有下载成功
4. 联系小米产品经理申请开通“小米蓝牙安全开发板”白名单。
5. 按照`main.c`文件中I2C引脚定义连接开发板和安全芯片。
6. 按照各分支下README说明中的路径导入工程，编译固件，下载固件测试开发板。
7. 打开米家APP，开启蓝牙，发现附近的设备，点击“小米蓝牙安全开发板”。在连接过程中出现6位数字PIN码，在固件上输入这6个数字（小米蓝牙安全开发板需要在Segger JLink RTT Viewer环境下输入）。登陆成功后，APP会有相应提示。进入插件，点击“开关闪灯”可以模拟开锁和关锁，同时开发板会广播带有相应事件Object的MiBeacon。这时也可以看到开发板上LED灯有响应。同时RTT Viewer也可以打印出开发板log。
8. APP管理者可以分享钥匙给其他小米账号用户。小米蓝牙开发板暂时不支持此功能。
9. 重置设备，目前需要两步
    - APP删除设备
    - 固件上长按重置按钮

#### 已支持芯片平台列表

*其他分支仅供米家内部测试使用*

| 芯片品牌     | 芯片型号 | SDK                                                                |
| :----------- | :------- | :----------------------------------------------------------------- |
| Nordic       | 51       | https://github.com/MiEcosystem/mijia_ble_secure/tree/nordic_legacy |
| Nordic       | 52       | https://github.com/MiEcosystem/mijia_ble_secure/tree/nordic        |
| Silicon Labs | BG13     | https://github.com/MiEcosystem/mijia_ble_secure/tree/silabs        |

## 米家高安全级认证库介绍

开发者首先需要了解高安全级BLE接入开发流程。

#### 产品开发过程中需了解的宏定义

| 宏名                      | 作用                                        |
| :------------------------ | :------------------------------------------ |
| CUSTOMIZED_MI_CONFIG_FILE | 定义开发者自己的头文件                      |
| HAVE_MSC                  | 0，无安全芯片；1，MJSC安全芯片；2，MJA1安全芯片 |
| MI_BLE_ENABLED            | 高安全级BLE接入产品必须打开这个宏              |
| MI_LOG_ENABLED            | 打印调试log，调试时候可以打开此宏            |
| MI_ASSERT                 | MI_LOG_ENABLED打开时才能使用，打印error信息 |
| PRODUCT_ID                | 必须与在小米IoT开发者平台创建的产品ID相同    |
| DEVELOPER_VERSION         | 开发者固件版本号                           |

<注1> MJSC为第一代安全芯片，已停产。MJA1为第二代安全芯片，推荐使用。

<注2> 用户可以通过 CUSTOMIZED_MI_CONFIG_FILE 定义自己的配置头文件。如果在此头文件中具有与 mi_config.h 相同的宏定义，那么会采用自定义头文件中的定义。如果没有，会采用 mi_config.h 中默认值。例如，如果不定义 PRODUCT_ID，则默认为 0x01CF 小米蓝牙安全开发板。也可以直接修改 mi_config.h 。

可以根据需要修改PRODUCT_ID定义
```
#ifndef PRODUCT_ID
#ifdef MI_BLE_ENABLED
#define PRODUCT_ID  0x01CF
#else
#define PRODUCT_ID  0x0379
#endif
#endif

// pid = 0x01CF代表小米蓝牙安全开发板，仅用于测试。注意此产品为白名单用户可见。如需使用此 PID 进行功能验证，请申请白名单权限。在真正的产品开发中，pid必须与在小米IoT开发者平台上创建的产品ID相同。
```

`main.c`文件中定义了开发板的I2C引脚用来连接安全芯片，以Nordic芯片为例：

```
#define MSC_PWR_PIN                     23   // enable MSC chip
const iic_config_t iic_config = {
        .scl_pin  = 24,
        .sda_pin  = 25,
        .freq = IIC_100K
};
```

#### 产品开发过程中需要了解的API

| 函数名 | 功能 | 说明 |
| :----- | :--- | :--- |
| mi_scheduler_init        | 调度器初始化        | 工作间隔应小于 BLE 最小连接间隔 |
| mi_schd_process          | 调度器处理函数       |在main loop或者task执行 |
| mi_schd_event_handler    | 事件回调函数         |处理事件响应   |
| mi_session_encrypt       | 加密函数             |登陆成功后，设备和手机之间建立了加密通道，负责对传输数据加密   |
| mi_session_decrypt       | 解密函数             |负责对传输数据解密  |
| mibeacon_obj_enque       | 事件/属性信息上报    |  发送带有object的MiBeacon    |

<注> `mibeacon_obj_enque`的使用方法：搭配蓝牙网关使用，开发者调用mibeacon_obj_enque将要发送的object放入发送队列，之后libs自动发送带有object的MiBeacon。**不允许**开发者调用系统广播函数广播MiBeacon。**必须采用此接口**，小米在其中定义了重发次数和重发间隔，确保事件可以被网关收到

关于API的具体定义在mijia_ble_libs README中有更详细的描述。https://github.com/MiEcosystem/mijia_ble_libs/blob/master/readme.md

#### 安全芯片测试

调用 mi_scheduler_start(SYS_MSC_SELF_TEST)可以对安全芯片完成自测试，如果测试通过, mi_schd_event_handler会收到事件SCHD_EVT_MSC_SELF_TEST_PASS。

## 更多

米家高安全级BLE接入必须使用安全芯片，第二代安全芯片MJA1有两款供客户选择，主要参数区别如下：

| 参数                     | MJA1-HCIW  | MJA1-SXIW  |
|:------------------------ |:---------- |:---------- |
| 睡眠功耗(uA)             | <0.5       | <1         |
| 空闲功耗(uA)             | <10        | < 460      |
| 典型运行功耗（mA）       | 1          | 10         |
| 最大峰值电流（mA）       | 3          | 18         |
| 典型签名用时（ms)        | 38         | 78         |
| 典型验签用时（ms）       | 69         | 152        |
| 典型生成密钥对用时（ms） | 32         | 66         |
| 典型秘钥协商用时（ms）   | 41         | 130        |
| 工作温度范围（℃）      | -40 ~ +85  | -40 ~ +105 |
| ESD                   | 8KV（HBM） | 5KV（HBM） |
| 包装                  |  卷带包装  |    卷带包装   |
| 安全认证              | 国内EAL4+      | 国际CC EAL5+    |

两颗芯片pin-to-pin，主要应用区别是温度范围和认证不同。出国内市场产品建议量产使用MJA1-HCIW，海外市场建议使用MJA1-SXIW。请根据产品定义与销售区域选择合适产品。

请详细阅读MJA1规格书，按照参考电路设计使用。米家一次可免费提供20PCS芯片样品、2块开发板，供开发调试使用。更多需求请与小米供应链联系采购。

## 空白工程集成米家高安全级认证库

### 软件环境

- IDE/toolchains (根据芯片 SDK 要求进行安装，如遇问题可咨询芯片原厂 FAE)
- [JLink](https://www.segger.com/downloads/jlink/)
- [Git](https://git-scm.com/downloads)

### 硬件环境

硬件平台可以选用芯片原厂的开发板，或使用在研产品开发板。推荐使用原厂开发板环境进行功能验证。完成功能验证后再移植到在研产品开发板。[已支持的芯片平台](#已支持芯片平台列表)

### 代码集成

*如果开发者在 Demo Project 基础上进行开发，可以跳过本节 1~5 步*

Demo Project 代码结构：

![高安全级 BLE 产品软件框架](./pics/secure-auth.png)

如上图所示，高安全级 BLE 产品接入需要用到以下代码：
- SoC SDK & 协议栈
- [mijia ble api](https://github.com/MiEcosystem/mijia_ble_api)
- [mijia ble libs](https://github.com/MiEcosystem/mijia_ble_libs.git)

其中，SoC SDK & 协议栈 由芯片原厂提供，米家会持续更新已验证的 SDK 版本；mijia ble api & libs 由米家提供，以源码形式托管在 GitHub 上。推荐开发者通过 **git submodule** 方式将米家代码集成到自己仓库中。以便后续通过 submodule update 方式升级 api & libs。mijia ble api 为开源仓库，无需申请权限即可访问。mijia ble libs 为私有仓库，需申请访问权限。

*以 Nordic 为例*

1. 下载芯片原厂 SDK。(SDK 版本根据 mijia ble api 要求进行选择)
> 从 Nordic 官网下载 [SDK 15.2.0](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK/Download#infotabs)
2. 按照芯片原厂 SDK 说明文档，新建 BLE 空工程。(如果已创建工程，可以跳过此步)
> Nordic BLE 空工程在 `SDK\examples\ble_peripheral\ble_app_template` 目录下，复制一份 ble_app_template 并重命名，例如 secure_demo
3. 在新建的 BLE 工程目录下创建 [Git](https://git-scm.com/) 本地仓库。(如果已有本地仓库，可以跳过此步)
```
$ git init
```
4. 添加 mijia ble api 和 mijia ble libs 到本地仓库。添加 mijia ble api 时需选择对应芯片平台的分支。
```
$ git submodule add -b nordic https://github.com/MiEcosystem/mijia_ble_api.git
$ git submodule add -b master https://github.com/MiEcosystem/mijia_ble_libs.git
$ git commit -am "add submodules mijia ble api & libs"
```
5. 添加 mijia ble api 和 mijia ble libs 路径下源码到 BLE 工程中，并将路径加入工程的 `include path`。mijia ble api 会使用到 SoC SDK 中的 BLE，TIMER，IIC，FLASH 等功能，需要开发者将这些功能模块添加到工程中。（详细资源占用情况及配置方法，请参考芯片对应分支下的 README）。添加下列路径下代码。
```
mijia_ble_libs
├── common
├── cryptography
│   └── mja1
├── mijia_profiles
├── secure_auth
└── third_party
    ├── SEGGER_RTT
    ├── mbedtls
    ├── micro-ecc
    └── pt
```
**注意** libs 引用的 `./third_party/` 路径下的第三方开源代码（例如， mbedtls，RTT），可能在 SoC SDK 中已经包含。当工程编译出现重复定义时，开发者需要去掉 libs 中重复文件以解决冲突。（比如，原厂工程中已经包含 SEGGER RTT 文件，就无需再添加 `./third_party/SEGGER_RTT` 路径下源码）

1. 创建一个配置文件，在文件中添加宏定义，然后将 `CUSTOMIZED_MI_CONFIG_FILE=<your_config.h>` 添加到工程 `Preprocesser symbols`中，或者直接将以下宏定义添加到工程的 `Preprocesser symbols` 配置中。[所需配置的宏定义](#产品开发过程中需了解的宏定义)

### 米家认证

进行以下步骤前请**先阅读** mijia ble libs [**使用手册**](https://github.com/MiEcosystem/mijia_ble_libs/blob/master/readme.md)。

1. 芯片运行环境初始化（包括但不限于芯片电源，时钟，GPIO，TIMER，IIC，BLE stack）。
2. 使用 `mibeacon_data_set()` 生成MiBeacon广播数据，并开始广播。
[示例代码](https://github.com/MiEcosystem/mijia_ble_secure/blob/c8fdaf4daedc412c7af1fdfa39fbd59e9874d27e/main.c#L521-L561)
3. 实现 C 标准库 `time` 相关函数，初始化系统时间 。
[示例代码](https://github.com/MiEcosystem/mijia_ble_secure/blob/c8fdaf4daedc412c7af1fdfa39fbd59e9874d27e/time.c#L22-L40)
4. 调用 `mi_service()` 创建米家服务。米家服务提供认证数据传输通道，服务缺失将导致 App 认证失败。
5. 根据米家认证芯片硬件连线，定义 IIC 引脚配置 `iic_config`，实现电源管理函数 `mijia_secure_chip_power_manage()`，以及认证事件处理函数 `mi_schd_event_handler()`，进行 `mi_scheduler_init()` 初始化。（`mi_schd_event_handler()` 在收到 SCHD_EVT_OOB_REQUEST 事件的 30 秒内，产品端要根据自身 IO 能力获取 OOB，再调用 `mi_input_oob()` 向 `mi_schduler` 提供所需 OOB。）初始化后，调用 `mi_scheduler_start(SYS_KEY_RESTORE)` 尝试读取绑定信息，执行结果会通过 `mi_schd_event_handler()` 传参返回。如果已重置，将收到 SCHD_EVT_KEY_NOT_FOUND 事件。
[示例代码](https://github.com/MiEcosystem/mijia_ble_secure/blob/c8fdaf4daedc412c7af1fdfa39fbd59e9874d27e/main.c#L823-L830)
6. 将 `mi_schd_process()` 添加到 main loop 中。
7. 米家认证相关的事件，会通过 `mi_schd_event_handler()` 通知应用层：

| 事件名称                          | 含义                                         |
| :------------------------------- | :------------------------------------------ |
| SCHD_EVT_REG_SUCCESS             | 绑定注册成功                                  |
| SCHD_EVT_REG_FAILED              | 绑定注册失败                                  |
| SCHD_EVT_ADMIN_LOGIN_SUCCESS     | 管理员登录成功                                |
| SCHD_EVT_ADMIN_LOGIN_FAILED      | 管理员登录失败                                |
| SCHD_EVT_SHARE_LOGIN_SUCCESS     | 分享者登录成功                                |
| SCHD_EVT_SHARE_LOGIN_FAILED      | 分享者登录失败                                |
| SCHD_EVT_TIMEOUT                 | 操作超时                                     |
| SCHD_EVT_KEY_NOT_FOUND           | 未绑定                                       |
| SCHD_EVT_KEY_FOUND               | 已绑定                                       |
| SCHD_EVT_KEY_DEL_SUCC            | 重置成功                                     |
| SCHD_EVT_OOB_REQUEST             | 请求输入 OOB                                 |
| SCHD_EVT_MSC_SELF_TEST_PASS      | 认证芯片自检正常                              |
| SCHD_EVT_MSC_SELF_TEST_FAIL      | 认证芯片自检异常                              |

此外，开发者可以使用 `mi_scheduler_start()` 发送以下命令，进行本地系统管理：

| 命令名称                     | 含义                                        |
| :-------------------------- | :----------------------------------------- |
| SYS_KEY_RESTORE             | 读取绑定信息                                 |
| SYS_KEY_DELETE              | 删除绑定信息                                 |
| SYS_MSC_SELF_TEST           | 认证芯片自检                                 |

经过以上步骤，产品侧已支持米家安全认证功能，可以使用米家 App 进行功能验证。具体步骤可参考本文档米家接入。

### 应用层开发

认证完成后，产品便具备安全通信的能力。在安全通信的基础上，可实现各种应用功能，比如，门锁应用，透传应用。

米家定义了一套门锁应用层规范 `lock profile`，使得米家 App 能对锁类产品进行统一的蓝牙操作。`lock profile` 只提供门锁通用操作、状态、事件上报功能，故不能满足产品的其他功能需求(比如，指纹管理)。如需支持其他功能，开发者可创建自定义服务，并采用安全通信方式读写自定义服务特征值。具体实现请参考 [米家透传服务](#米家透传服务)。

**注意** 门锁类产品必须具备米家门锁服务，米家 App 才可以进行开关锁操作和锁事件上报。

1. 调用 `lock_service()` 创建米家门锁服务，注册锁操作回调函数 `ble_lock_ops_handler()`，门锁操作码会通过回调函数通知应用层。[示例代码](https://github.com/MiEcosystem/mijia_ble_secure/blob/c8fdaf4daedc412c7af1fdfa39fbd59e9874d27e/main.c#L834-L836)
2. 当 `ble_lock_ops_handler()` 收到锁操作码后，应立即执行对应操作 (比如，控制电机旋转开锁)，并使用 `mibeacon_obj_enque()` 向蓝牙网关广播锁事件，使用 `send_lock_log()` 向米家 App 回复锁事件。[示例代码](https://github.com/MiEcosystem/mijia_ble_secure/blob/c8fdaf4daedc412c7af1fdfa39fbd59e9874d27e/main.c#L754-L789)

### 米家透传服务

米家透传服务作为一个示例，演示如何在一个自定义服务上实现加密传输。

1.参考 `stdio_service_init()` 创建数据传输示例服务，注册回调函数，以接收 App 数据。(示例服务创建时使用了 mijia ble api, 开发者可直接使用原厂 SDK API)
[示例代码](https://github.com/MiEcosystem/mijia_ble_libs/blob/3b733870ca186662761f153f850537bae022fb5d/mijia_profiles/stdio_service_server.c#L168-L234)

2.发送数据前，先使用 `mi_session_encrypt()` 加密数据，再发送。参考 `stdio_tx()` 实现；
[示例代码](https://github.com/MiEcosystem/mijia_ble_libs/blob/3b733870ca186662761f153f850537bae022fb5d/mijia_profiles/stdio_service_server.c#L237-L261)

3.接收加密数据后，调用 `get_mi_authorization()` 判断设备当前登录状态，若已登录使用 `mi_session_decrypt()` 解密数据，再上报应用层处理。参考 stdio 服务 `on_write_permit()` 实现：
[示例代码](https://github.com/MiEcosystem/mijia_ble_libs/blob/3b733870ca186662761f153f850537bae022fb5d/mijia_profiles/stdio_service_server.c#L76-L96)

