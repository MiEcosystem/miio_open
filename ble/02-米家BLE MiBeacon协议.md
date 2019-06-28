# 米家BLE MiBeacon协议

## 概述

MiBeacon协议规定了基于蓝牙4.0及以上设备的广播格式。MiBeacon有两个主要的作用：

- 标识设备自己的身份和类型，用以被米家App连接配置。
- 发送设备自己的事件或属性到网关，用来远程上报状态和进行更普遍的设备联动等。

只要含有如下指定信息的广播报文，即可认为是符合了MiBeacon广播格式

- advertising中«Service Data»(0x16)含有Mi Service(UUID：0xFE95)
- scan response中«Manufacturer Specific Data»(0xFF)含有小米公司识别码(ID：0x038F)

无论是在advertising中，还是scan response中，均采用统一格式定义。

注意事项

- 所有数据均为**小端格式**。
- v5版本的MiBeacon**禁止**在scan response中添加有效object数据。后续米家BLE网关只会开启passive scan，因此scan response不能被网关接收。
- 通过MiBeacon发送给网关的事件或属性，如果要确保网关成功接收，同一事件或属性至少重复10次。如果MiBeacon中Frame Counter位相同，米家BLE网关会认为这是同一个事件或属性。如果要广播不同的事件和属性，Frame Counter位必须不同。
- 两种发送MiBeacon的方式，短时间内多次重复发送(例如开门/关门事件, **要求间隔100ms重复10次或更多**)或较长时间间隔持续发送(例如温度数据，**要求间隔2s持续发送**)，按需选择。推荐事件选用第一种方式而属性选择第二种方式。事件或属性的定义参考[米家BLE Object协议](https://github.com/MiEcosystem/miio_open/blob/master/ble/03-%E7%B1%B3%E5%AE%B6BLE%20Object%E5%8D%8F%E8%AE%AE.md)。米家对于此项会验收。
- 采用高安全级接入的设备，必须调用**米家提供的API**来发送MiBeacon事件或属性。米家对于此项会验收。

## Service Data 或 Manu Data 格式

| Field | Name                    | 类型   | 长度(byte)      | 必备(M) / 可选(O) | 说明 |
| :--- | :---------------         | :----- | :------------- | :--------------- | :------------- |
|   1   | Frame Control           | Bitmap | 2               | M                 | 控制位，详见下表具体定义    |
|   2   | Product ID              | U16    | 2               | M                 | 产品ID，每类产品唯一，pid需要在[小米IoT开发者平台](https://iot.mi.com/new/index.html)申请 |
|   3   | Frame Counter           | U8     | 1               | M                 | 序号，用于去重，不同的事件或属性上报需要不同的Frame Counter      |
|   4   | MAC Address             | U8     | 6               | C.1               | 设备Mac地址         |
|   5   | Capability              | U8     | 1                 | C.1               | 设备能力，详见下表定义     |
|   6   | I/O capability          | U8     | 2                 | C.2               | I/O能力，目前只有高安全级BLE接入才会用到此字段，只支持MiBeacon v5版本。只有在绑定之前使用；当绑定完成，上报事件时(例如开门、关门)，此字段也不再需要。详见下表定义 |
|   7   | Object                  | U8     | n(根据实际需求)   | C.1               | 触发事件或广播属性，详见[米家BLE Object定义](https://github.com/MiEcosystem/miio_open/blob/master/ble/03-%E7%B1%B3%E5%AE%B6BLE%20Object%E5%8D%8F%E8%AE%AE.md) |
|   8   | Random Number           | U8     | 3                 | C.1               | 如果加密则为必选字段，随机数3字节，用于加密报文  |
|   9   | Message Integrity Check | U8     | 4                 | C.1               | 如果加密则为必选字段，MIC四字节   |

C.1 : 根据Frame Control字段定义确定是否包含

C.2 : 根据Capability字段确定是否包含

<注> 如果包含的数据过多，超出Beacon长度，建议分多个Beacon广播。例如第一包广播Object，第二包广播MAC。v5版本不允许使用Scan Response。

<注> 如果有多个Object，建议分多个Beacon广播。

## Frame Control 字段定义

|  Bit  | Name               | Description  |
| :---: | :----------------- | :----------------------------------- |
|   0   | Reserved           | 保留     |
|   1   | Reserved           | 保留     |
|   2   | Reserved           | 保留     |
|   3   | isEncrypted        | 0：该包未加密；1：该包已加密  |
|   4   | MAC Include        | 0：不包含MAC地址；1：包含固定的MAC地址 (包含MAC地址是为了是的iOS识别此设备并进行连接)      |
|   5   | Capability Include | 0：不包含Capability；1：包含Capability。设备未绑定前，这一位强制为1    |
|   6   | Object Include     | 0：不包含Object；1：包含Object   |
|   7   | Mesh               | 0：不包含Mesh；1：包含Mesh。普通BLE接入产品和高安全级接入，此项强制为0。Mesh接入此项强制为1。Mesh接入更多信息请参考Mesh相关文档 |
|   8   | registered         | 0：设备未绑定；1：设备已注册绑定。此项用于表示设备是否被重置  |
|   9   | solicited          | 0：无操作；1：请求 App 进行注册绑定。当用户在开发者平台选择设备确认配对时才有效，否则置0。此项原名称为bindingCfm，重命名为solicited “主动请求，招揽” App进行注册绑定  |
| 10~11 | Auth Mode          | 0：旧版本认证；1：安全认证；2：标准认证；3：保留 |
| 12~15 | version            | 版本号(当前为v5)    |

<注> Reserved位必须填0

<注> 相应的Bit位与MiBeacon的实际数据必须对应，否则APP或网关认为是不符合规范的MiBeacon直接丢弃。

<注> 当周边有多个相同设备时(如多个蓝牙温湿度传感器)，怎么指定哪个设备需要被绑定？开发者需要在开发者平台选择蓝牙配对方式，目前有三种配对方式：APP选择配对，RSSI符合配对，设备确认配对。当选择APP选择配对或RSSI符合配对时，solicited置0。当选择设备确认配对时，正常广播的MiBeacon中solicited位为0，当用户触发，如按键，MiBeacon中solicited位变为1，持续2~3秒后恢复为0。此时手机可以发现solicited为1的设备并连接开始认证流程。

## Capability 字段定义

|  Bit  | Name        | Description                               |
| :---: | :---------- | :---------------------------------------- |
|   0   | Connectable | 暂时不使用                                 |
|   1   | Centralable | 暂时不使用                                 |
|   2   | Encryptable | 暂时不使用                                 |
|  3~4  | BondAbility | 0，无绑定，1，前绑定，2，后绑定，3，Combo    |
|   5   | I/O         | 1，包含I/O Capability字段                  |
|  6~7  | Reserved    | 保留                                      |

<注> BondAbility字段表明当周边有多个相同设备时如何确定要绑定哪个设备。无绑定：APP选择配对，RSSI符合配对。前绑定：设备确认配对，即先扫描，设备发确认包后(solicited in Frame Control)进行连接。后绑定：扫描后直接连接，设备通过震动等方式确认。Combo：针对Combo芯片。此绑定方式需要在小米IoT开发者平台选择，并且与此处保持一致。

## I/O Capability 定义

| Byte  | Name                | Description                        |
| :---: | :------------------ | :--------------------------------- |
|   0   | Base I/O capability | 0-3:基础输入能力; 4-7:基础输出能力 |
|   1   | Reserved            | 保留                               |

Base I/O capability类型可分为Input/Output两类，细分类型具体表示如下：

|  Bit  | Description         |
| :---: | :------------------ |
|   0   | 设备可输入 6 位数字 |
|   1   | 设备可输入 6 位字母 |
|   2   | 设备可读取 NFC tag  |
|   3   | 设备可识别 QR Code  |
|   4   | 设备可输出 6 位数字 |
|   5   | 设备可输出 6 位字母 |
|   6   | 设备可生成 NFC tag  |
|   7   | 设备可生成 QR Code  |

<注> 此字段只有在高安全级接入中才会使用。

## Object 定义

请参考[米家BLE Object定义](https://github.com/MiEcosystem/miio_open/blob/master/ble/03-%E7%B1%B3%E5%AE%B6BLE%20Object%E5%8D%8F%E8%AE%AE.md)。
