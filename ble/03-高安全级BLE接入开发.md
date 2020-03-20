# 高安全级BLE接入开发

*本文用于指导产品开发者实现高安全级BLE产品接入*

**开发前请先阅读[标准BLE接入开发](https://iot.mi.com/new/doc/embedded-development/ble/standard.html)**

## 米家安全芯片介绍

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

### 安全芯片测试

调用 mi_scheduler_start(SYS_MSC_SELF_TEST)可以对安全芯片完成自测试，如果测试通过, mi_schd_event_handler会收到事件SCHD_EVT_MSC_SELF_TEST_PASS。

## 接入米家

### 已支持的芯片方案

*其他分支仅供米家内部测试使用*

| 芯片品牌     | 芯片型号 | SDK                                                                |
| :----------- | :------- | :----------------------------------------------------------------- |
| Nordic       | 51       | https://github.com/MiEcosystem/mijia_ble_secure/tree/nordic_legacy |
| Nordic       | 52       | https://github.com/MiEcosystem/mijia_ble_secure/tree/nordic        |
| Silicon Labs | BG13     | https://github.com/MiEcosystem/mijia_ble_secure/tree/silabs        |

### 接入步骤

与标准BLE接入开发类似，高安全级BLE接入方式的软件SDK，也是由三部分组成：芯片原厂SDK，米家高安全级认证库，米家智能门锁示例Demo。同时必须外接安全芯片。具体步骤如下：

1. [米家高安全级认证库](https://github.com/MiEcosystem/mijia_ble_libs)是私有工程，产品开发者首先需联系小米产品经理，增加获取高安全级认证库的权限。**注意，每个新品的接入都要单独申请认证库的权限，一个产品申请一次**，如果发现未经申请的新品接入，米家有权利拒绝产品上线。 **申请高安全级认证库请发送邮件至 小米IoT平台开放窗口 <miot-openhome@xiaomi.com>，格式请参考常用信息->开发者反馈指引。**
2. 根据芯片选型选择对应的分支，下载芯片厂商提供的SDK，各分支下有对应的下载地址链接或下载方式。
3. 下载[米家智能门锁示例Demo](https://github.com/MiEcosystem/mijia_ble_secure)
   - 按照各分支下的README说明，进入到芯片原厂SDK的指定目录
   - 命令行执行`git clone -b xxx(对应分支) --recursive https://github.com/MiEcosystem/mijia_ble_secure.git`。建议使用git clone --recursive命令，直接网站上下载会发现有些文件找不到，这是因为submodule没有下载成功
4. 联系小米产品经理申请开通“小米蓝牙安全开发板”白名单。
5. 按照`main.c`文件中I2C引脚定义连接开发板和安全芯片。
6. 按照各分支下README说明中的路径导入工程，编译固件，下载固件测试开发板。
7. 打开米家APP，开启蓝牙，发现附近的设备，点击“小米蓝牙安全开发板”。在连接过程中出现6位数字PIN码，在固件上输入这6个数字（小米蓝牙安全开发板需要在Segger JLink RTT Viewer环境下输入）。登陆成功后，APP会有相应提示。进入插件，点击“开关闪灯”可以模拟开锁和关锁，同时开发板会广播带有相应事件Object的MiBeacon。这时也可以看到开发板上LED灯有响应。同时RTT Viewer也可以打印出开发板Log。
8. APP管理者可以分享钥匙给其他小米账号用户。小米蓝牙开发板暂时不支持此功能。
9. 重置设备，目前需要两步
    - APP删除设备
    - 固件上长按重置按钮

### 应用层开发

产品开发过程中需了解的宏定义参照[标准BLE接入开发](https://iot.mi.com/new/doc/embedded-development/ble/standard.html) 应用层开发章节。

请注意 **HAVE_MSC宏定义**： 0表示无安全芯片；1表示MJSC安全芯片；2表示MJA1安全芯片。其中，MJSC为第一代安全芯片，已停产。MJA1为第二代安全芯片，推荐使用。

**PRODUCT_ID宏定义**：Demo工程中默认为 0x01CF 表示小米蓝牙安全开发板。仅用于测试。注意此产品为白名单用户可见。如需使用此 PID 进行功能验证，请申请白名单权限。在真正的产品开发中，pid必须与在小米IoT开发者平台上创建的产品ID相同。

`main.c`文件中定义了开发板的I2C引脚用来连接安全芯片，以Nordic芯片为例：

```
#define MSC_PWR_PIN                     23   // enable MSC chip
const iic_config_t iic_config = {
        .scl_pin  = 24,
        .sda_pin  = 25,
        .freq = IIC_100K
};
```

认证完成后，产品便具备安全通信的能力。在安全通信的基础上，可实现各种应用功能，比如，门锁应用，透传应用。

米家定义了一套门锁应用层规范 `lock profile`，使得米家 APP 能对锁类产品进行统一的蓝牙操作。`lock profile` 只提供门锁通用操作、状态、事件上报功能，故不能满足产品的其他功能需求(比如，指纹管理)。如需支持其他功能，开发者可创建自定义服务，并采用安全通信方式读写自定义服务特征值。具体实现请参考米家透传服务。

**注意** 门锁类产品必须具备米家门锁服务，米家 APP 才可以进行开关锁操作和锁事件上报。

1. 调用 `lock_service()` 创建米家门锁服务，注册锁操作回调函数 `ble_lock_ops_handler()`，门锁操作码会通过回调函数通知应用层。[示例代码](https://github.com/MiEcosystem/mijia_ble_secure/blob/c8fdaf4daedc412c7af1fdfa39fbd59e9874d27e/main.c#L834-L836)
2. 当 `ble_lock_ops_handler()` 收到锁操作码后，应立即执行对应操作 (比如，控制电机旋转开锁)，并使用 `mibeacon_obj_enque()` 向蓝牙网关广播锁事件，使用 `send_lock_log()` 向米家 APP 回复锁事件。[示例代码](https://github.com/MiEcosystem/mijia_ble_secure/blob/c8fdaf4daedc412c7af1fdfa39fbd59e9874d27e/main.c#L754-L789)


**米家透传服务**请参考[标准BLE接入开发](https://iot.mi.com/new/doc/embedded-development/ble/standard.html) 应用层开发章节。


## 空白工程集成米家高安全级认证库

请参考[标准BLE接入开发](https://iot.mi.com/new/doc/embedded-development/ble/standard.html) 空白工程集成米家标准认证库章节。

不同点在于：

1. 根据米家认证芯片硬件连线，定义 IIC 引脚配置 `iic_config`，实现电源管理函数 `mijia_secure_chip_power_manage()`，以及认证事件处理函数 `mi_schd_event_handler()`，进行 `mi_scheduler_init()` 初始化。（`mi_schd_event_handler()` 在收到 SCHD_EVT_OOB_REQUEST 事件的 30 秒内，产品端要根据自身 IO 能力获取 OOB，再调用 `mi_input_oob()` 向 `mi_schduler` 提供所需 OOB。）初始化后，调用 `mi_scheduler_start(SYS_KEY_RESTORE)` 尝试读取绑定信息，执行结果会通过 `mi_schd_event_handler()` 传参返回。如果已重置，将收到 SCHD_EVT_KEY_NOT_FOUND 事件。
[示例代码](https://github.com/MiEcosystem/mijia_ble_secure/blob/c8fdaf4daedc412c7af1fdfa39fbd59e9874d27e/main.c#L823-L830)

2. 米家认证相关的事件，会通过 `mi_schd_event_handler()` 通知应用层：

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

经过以上步骤，产品侧已支持米家安全认证功能，可以使用米家 APP 进行功能验证。
