# STM32F103 数码管秒表

四位数码管动态扫描，按键控制启停、继续和复位。引脚与电平设置在 firmware/include/board.h，计时与扫描分别在 firmware/src/stopwatch.c 和 display.c。安装 GCC 后可在电脑上测试计时逻辑。

## 运行

~~~powershell
gcc -std=c11 -Wall -Wextra -Werror -Itests/stub -Ifirmware/include tests/test_stopwatch.c firmware/src/stopwatch.c -o test_stopwatch.exe
./test_stopwatch.exe
~~~

## 固件构建

安装 Arm GNU Toolchain 和 GNU Make。从仓库根目录拉取 Arm 和 ST 官方 CMSIS 依赖。

~~~powershell
New-Item -ItemType Directory -Force vendor | Out-Null
git clone --depth 1 https://github.com/ARM-software/CMSIS_5.git vendor/CMSIS_5
git clone --depth 1 https://github.com/STMicroelectronics/cmsis_device_f1.git vendor/cmsis-device-f1
cd firmware
mingw32-make
~~~

Linux 可使用 make。输出位于 firmware/build/，可用支持 STM32F1 的调试器写入相应型号开发板。
