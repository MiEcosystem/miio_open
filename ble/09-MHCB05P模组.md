# MHCB05P模组使用说明

## 模组介绍
MHCB05P 是基于 Realtek 的 RTL8762CMF 高性能的 BLE 模组， 内置 ARM Cortex-M4F 核， 高发射功率7.5dbm。
适于智能穿戴，智能家居等诸多应用场景。 该模组提供业界最高的集成度， 有显著的系统性能， 具备较低功耗和低成本等特点。

MHCB05P 分为两个型号， MHCB05P-B 为板载天线模组， MHCB05P-IB 为外接天线模组

## 模组特性
模组尺寸： 13 * 23.5 * 3.4 mm

高性能 32-bit 40 MHz ARM Cortex®-M4

512KB flash， 64KB RAM

最高发射功率 8dBm

最低接收灵敏度： -97dBm

### 功耗数据
7.3 mA current at RX

7.9mA current at TX power 0dBm

9.6mA current at TX power 4dBm

11.3mA current at TX power 8dBm

2.6uA current at DLSP (Wakeup by GPIO Timer)

450nA current at Power down （Wakeup by GPIO）

更多详细数据请参考**MHCB05P模组规格书**

## 软件开发

1、MHCB05P模组的SDK及mijia标准认证Demo请先发邮件至小米开放平台（ miot-openhome@xiaomi.com ）获取MiEcosystem/mijia_ble_mesh仓库realtek分支（ https://github.com/MiEcosystem/mijia_ble_mesh/tree/realtek ）访问权限

2、执行git命令 git clone -b realtek --recursive git@github.com:MiEcosystem/mijia_ble_mesh.git

**注意**要加--recursive选项，否则submodule内容无法下载，导致工程中缺少文件

3、mijia标准认证Demo工程位于Bee2_SDK_Mesh/board/evb/mijia_ble_standard目录下，更多信息请参考 [标准BLE接入开发](https://iot.mi.com/new/doc/embedded-development/ble/standard.html)

4、mesh接入工程可参考Bee2_SDK_Mesh/board/evb/目录下的mesh相关工程，更多信息请参考[蓝牙 Mesh接入开发](https://iot.mi.com/new/doc/embedded-development/ble/ble-mesh.html)

## 模组使用

### 工具

烧录、查看Log、打包固件都需要使用Realtek原厂提供的工具。

烧录固件需要使用Bee2MPTool工具

MP Tool支持两种烧录模式：量产模式和调试模式。量产模式用于产线生产，调试模式用于开发人员开发调试时使用。调试模式默认是关闭的，用户需要双击运行realtek提供的“Registry Set工具”打开调试模式。

**注意**为避免工厂量产时产生误操作，不建议在负责量产的电脑上运行“Registry Set工具”。

查看Log需要使用DebugAnalyzer工具。

打包固件需要使用MP PackTool工具。

### bin文件
使用MPTool可以烧录7个文件：
| 文件名 | 功能 | 默认位置 | 备注 |
| :----- | :--- | :------- | :-------- |
| flash_map.ini | flash分区表 | \Bee2_SDK_Mesh\tool\download\xiaomi | 必选 |
| OTAHeader_Bank0-xxx.bin | ota header | \Bee2_SDK_Mesh\tool\download\xiaomi | 可选 |
| fsbl_MP_master___1.1.2.0_xxx.bin | secure boot | \Bee2_SDK_Mesh\tool\download | 可选 |
| Patch_ext_MP_master##_1.0.366.0_xxx.bin | patch解压缩 | \Bee2_SDK_Mesh\tool\download\xiaomi | 可选 |
| Patch_MP_571#####_1.0.371.0_xxx.bin | patch | \Bee2_SDK_Mesh\tool\download\xiaomi | 必选 |
| app_MP_sdk#####_0.0.1.3_xxx.bin | app image | \Bee2_SDK_Mesh\board\evb\YOUR_PROJECT\bin | 必选 |
| configFile_xxx.bin | configFile | \Bee2_SDK_Mesh\tool\download\xiaomi | 可选 |

使用小米模组烧录与OTA打包只需要**flash分区表、app image、patch**三个文件。

**注意：configFile**不能随意烧写，会改变模组MAC地址与射频标定。

### 烧写模组

以模组开发板为例，需要在J4处加一个跳线帽进入**烧写模式**，烧写完成后要**拔掉**此跳线帽。

**注意**：换了跳线帽接法之后要将模组复位一下

![Module Development](http://cdn.cnbj0.fds.api.mi-img.com/miio.files/commonfile_png_a204848c1f9de2a428db26d89972b7a0.png)

一次可烧写多个模组（一次最多支持8个）

操作流程：Detect——>Open——>Download

![Module Development](http://cdn.cnbj0.fds.api.mi-img.com/miio.files/commonfile_png_90825fe0c921488481f518c0d02cfc40.png)

### 查看模组Log

查看Log也需要修改开发板跳线帽，跳线帽是套在J4的4个排针的中间两个上，如下图黄色跳线帽所示所示

**注意**：换了跳线帽接法之后要将模组复位一下

![Module Development](http://cdn.cnbj0.fds.api.mi-img.com/miio.files/commonfile_jpg_a8bbc6a7ed15a9ad86523194f19013d4.jpg)

查看Log需要使用DebugAnalyzer工具，并在setting中选择与app配套编译生成的trace文件

![Module Development](http://cdn.cnbj0.fds.api.mi-img.com/miio.files/commonfile_jpg_9784ba1930b40c488c377b41ae4921a7.jpg)
![Module Development](http://cdn.cnbj0.fds.api.mi-img.com/miio.files/commonfile_jpg_ed7f8d9f15111e3dac2d5e5aefb6e2f1.jpg)

### OTA打包
需要使用MPPackTool.exe工具

先点击Load Layout，选择flash_map.ini

添加新生成的app.bin和Patch文件

最后将打包生成选择OTA_RAW的bin文件上传开放平台即可，注意在开放平台需要对固件进行签名。

![Module Development](http://cdn.cnbj0.fds.api.mi-img.com/miio.files/commonfile_png_2544acf5bde44c8b94ea62ab5bb7e98e.png)

### 制作厂包
打开MP PackTool工具，在如下图各个字段填入相应的bin文件。

如果是没有烧过蓝牙地址和证书的芯片“System Config File”要填入bin文件，否则不需要填入。

之后点“Confirm”生成厂包
![Module Development](http://cdn.cnbj0.fds.api.mi-img.com/miio.files/commonfile_png_c9e2e3bee947adad86634d0f51d2d77b.png)

在打包工具文件夹下会生成相应的打包好的ImgPacketFile.bin文件，即是厂包
