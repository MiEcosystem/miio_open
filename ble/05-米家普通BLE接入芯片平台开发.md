# 米家普通BLE接入芯片平台开发

***普通BLE接入将于2019年8月份升级，届时必须采用新的接入方式，本文档会废弃***

***新的接入资料即将发布***

*本文用于指导芯片厂商将芯片平台接入米家*

<br/>

## 接入米家

1. **芯片厂商**按照[API接口说明](https://miecosystem.github.io/mijia_ble_api/)，适配mijia_api并完成接口功能自测试。
2. **芯片厂商**参照GitHub上已有例程[米家标准认证示例demo](https://github.com/MiEcosystem/mijia_ble)，编写示例工程，主要在主函数中调用：
    - 调用认证库函数：`mible_std_auth_evt_register(std_authen_event_cb);`用于向米家认证流程注册回调函数，在回调函数中可接收事件，例如连接、断开、service初始化完成、绑定结果等。
    - 调用认证库函数：`mible_server_info_init(&dev_info, MODE_STANDARD);`完成产品信息的初始化配置。
    - 调用认证库函数：`mible_server_miservice_init();`初始化用于米家认证Service和characteristic（Service UUID 0xFE95）。
    - 自行实现：`advertising_init();`初始化广播包。符合MiBeacon定义。详见[米家BLE MiBeacon协议](https://github.com/MiEcosystem/miio_open/blob/master/ble/02-%E7%B1%B3%E5%AE%B6BLE%20MiBeacon%E5%8D%8F%E8%AE%AE.md)。
    - 自行实现：`advertising_start();`开始广播。
3. **芯片厂商**将适配好的API代码、示例工程以及相关硬件（开发板）发送到**小米**产品经理，并告知详细的编译、烧写等操作方法。
4. **小米**完善示例工程，进行认证库的适配和测试，此过程中遇到问题可邮件沟通或现场联调。
5. **小米**测试成功后将认证库发给**芯片厂商**进行自测试。测试方法如下：
    - **芯片厂商**向**小米**产品经理申请小米蓝牙开发板白名单
    - 使用米家APP与开发板反复进行绑定、登陆等操作（可对开发板上/下电、杀死米家APP进程等）
    - 多种操作条件下都能够绑定、登陆成功，即为测试通过
6. 双方测试无误后，**小米**将示例工程及适配的mijia_api上传GitHub，编写详细的使用说明文档，将认证库发到内部入库，供后续开发者申请和使用。
7. **产品开发者**按照Github上的说明下载代码，并申请认证库，开始应用开发工作。

## 参考文档

[米家BLE API介绍](https://miecosystem.github.io/mijia_ble_api/)

参考[米家普通BLE接入产品开发](https://github.com/MiEcosystem/miio_open/blob/master/ble/04-%E7%B1%B3%E5%AE%B6%E6%99%AE%E9%80%9ABLE%E6%8E%A5%E5%85%A5%E4%BA%A7%E5%93%81%E5%BC%80%E5%8F%91.md)了解米家蓝牙标准认证库的更多信息。
