踩坑：
野火的debian系统默认加载了一个mpu6050驱动，导致自己写的不能加载
```shell
inv_mpu6050_i2c        16384  0
inv_mpu6050            20480  2 inv_mpu6050_i2c
```


野火默认已经使用overlay的方式在i2c1上注册了mpu6050：

```shell
root@npi:/sys/bus/i2c/devices# cd 0-0068
root@npi:/sys/bus/i2c/devices/0-0068# ls
channel-0  iio:device1  name     power      trigger0
driver     modalias     of_node  subsystem  uevent
root@npi:/sys/bus/i2c/devices/0-0068# cat name
mpu6050
root@npi:/sys/bus/i2c/devices/0-0068#
```

imx-fire-mpu6050-overlay.dts：
```c
 /dts-v1/;
 /plugin/;
#include "../imx6ul-pinfunc.h"
#include "../imx6ull-pinfunc.h"
#include "dt-bindings/gpio/gpio.h"
 / {
     fragment@0 {
         target = <&i2c1>;
         __overlay__ { 
            #address-cells = <1>;
            #size-cells = <0>;    		
            mpu6050@68 {
            compatible = "invensense,mpu6050";
            reg = <0x68>;
            interrupt-parent = <&gpio5>;
            interrupts = <0 1>;
	        };          
         };
     };
 };
```