# 桌面机械臂 STM32 项目

本工程基于 STM32F103C8，使用 PCA9685 作为 5 路舵机驱动，通过电位器和按键控制桌面机械臂各关节运动。

## 项目简介

- MCU: STM32F103C8
- 开发环境: Keil uVision (ARM-ADS / ARMCC)
- 模块: PCA9685 舵机驱动模块
- 控制方式:
  - 1-4 号舵机：电位器模拟输入控制
  - 5 号舵机：按键控制
- 指示灯: LED 运动状态指示

## 主要功能

- 初始化 I2C 总线，驱动 PCA9685 模块
- 读取电位器输入并实时控制 4 个舵机
- 读取按键输入控制第 5 个舵机（夹爪或腕部）
- 启动时执行舵机动作测试序列
- 运动指示 LED 提示动作状态

## 硬件连接

- PCA9685 SCL → STM32 PB8
- PCA9685 SDA → STM32 PB9
- 按键输入 → STM32 PB10
- LED 指示灯 → STM32 PB12
- 电位器输入 → STM32 PA4、PA5、PB0、PB1
- PCA9685 舵机通道分配:
  - OUT0: 舵机1（底部旋转关节）
  - OUT1: 舵机2（肩部俯仰）
  - OUT2: 舵机3（肘部俯仰）
  - OUT3: 舵机4（腕部旋转）
  - OUT4: 舵机5（腕部俯仰/夹爪）
- PCA9685 V+ / GND: 舵机电源

## 代码结构

- `User/main.c` - 程序入口与控制主循环
- `Hardware/servo.c`、`Hardware/servo.h` - 舵机控制逻辑
- `Hardware/button_servo.c`、`Hardware/button_servo.h` - 按键舵机控制
- `Hardware/i2c.c`、`Hardware/i2c.h` - I2C 通信驱动
- `Hardware/pca9685.c`、`Hardware/pca9685.h` - PCA9685 驱动
- `Hardware/LED.c`、`Hardware/LED.h` - LED 指示控制
- `Hardware/PWM.c`、`Hardware/PWM.h` - PWM 或电机输出控制
- `Lib/` - STM32 标准外设库文件

## 构建与烧录

1. 使用 Keil MDK 打开 `project.uvprojx`
2. 选择目标芯片 `STM32F103C8`
3. 编译项目
4. 生成的输出文件位于 `Objects/`，包括 `project.axf`、`project.hex`
5. 使用 Keil 或 ST-Link 工具将 `project.hex` 或 `project.axf` 烧录到 STM32F103C8

## 运行说明

1. 供电 STM32 和 PCA9685 舵机电源
2. PCA9685 与 STM32 通过 PB8/PB9 连接 I2C
3. 启动后，程序先执行舵机测试动作
4. 进入主循环后：
   - 电位器控制 1-4 号舵机
   - 按键控制 5 号舵机
   - LED 显示运动状态

## 注意事项

- PCA9685 舵机电源需要独立供电，避免通过 STM32 板子供电舵机电流过大
- 按键与电位器输入信号要做好防抖与滤波
- 若需要修改测试通道，可在 `User/main.c` 中调整 `TEST_SERVO_CHANNEL` 或 `Test_Servo_Full_Range()` 逻辑

## 免责声明

本工程为桌面机械臂控制参考代码，具体连接与电源配置请根据实际硬件调整。