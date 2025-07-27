# stm32f407-bare-metal-programming
Bare-metal STM32F407 programming in C without HAL. Learn how to configure peripherals like UART, Timer, RTC, PWM, ADC, I2C, SPI, and Interrupts using direct register access. exploring low-level embedded systems.
# stm32f407-bare-metal-programming

This repository contains bare-metal drivers for STM32F407, written in C using STM32CubeIDE. Each module directly accesses registers without relying on HAL or STM32 Standard Peripheral Libraries. The primary goal is to provide learning register-level (bare-metal) embedded programming.

---

## Learning Focus

- Register-level programming without HAL
- Manual configuration of clocks, GPIOs, interrupts, and peripherals
- Developing peripheral drivers for real-world sensors and interfaces
  
---

## Prerequisites

- STM32CubeIDE installed
- STM32F407VG-based development board
- ST-Link debugger or equivalent
- Basic knowledge of ARM Cortex-M architecture

---

## Development Environment

- **IDE**: STM32CubeIDE
- **MCU**: STM32F407VG
- **Language**: C
- **Debugging**: ST-Link via STM32CubeIDE

---

## Directory Structure

All driver `.c` files are added in the `Src/` folder. For each module, include the corresponding initialization and usage in `main.c`.

---

## Modules and Description

Each file in this repository implements a peripheral driver or interrupt mechanism using direct register-level programming for the STM32F407 microcontroller. These drivers are added under the `Src/` directory of an STM32CubeIDE project.

| File Name                 | Description                                                                 |
|--------------------------|-----------------------------------------------------------------------------|
| `adc_driver.c`           | Implements Analog-to-Digital Converter configuration and reading functions. |
| `bmp280_spi_driver.c`    | Provides SPI communication support for the BMP280 pressure sensor.          |
| `interrupts_handler.c`   | Handles external and internal interrupt configurations using NVIC.          |
| `mpu6500_i2c_driver.c`   | Contains I2C-based communication with the MPU6500 IMU sensor.               |
| `pwm_driver.c`           | Generates PWM signals using timer-based configuration for motor or LED control. |
| `rtc_driver.c`           | Initializes the Real-Time Clock using internal LSI for basic timekeeping.   |
| `timer_driver.c`         | Configures general-purpose timers for delays, timebase, or periodic events. |
| `uart_driver.c`          | Sets up UART for serial communication through register-level programming.   |

---

You said:
## Example main.c Structure

```
#include "stm32f4xx.h"
#include "adc_driver.c"            // Uses PA1 (ADC channel 1)
#include "bmp280_spi_driver.c"     // Uses SPI1: PA5 (SCK), PA6 (MISO), PA7 (MOSI), PA4 (CS)
#include "interrupts_handler.c"    // Example: Uses PC13 (user button interrupt)
#include "mpu6500_i2c_driver.c"    // Uses I2C1: PB6 (SCL), PB7 (SDA)
#include "pwm_driver.c"            // Uses TIM3_CH1: PA6 (PWM output)
#include "rtc_driver.c"            // Uses LSI clock, no external pin needed
#include "timer_driver.c"          // Timer for internal timebase, no specific I/O
#include "uart_driver.c"           // Uses USART2: PA2 (TX), PA3 (RX)

int main(void) {
    // Initialize UART for debugging/output
    uart2_tx_init();               // PA2 (TX)
    uart2_write('H');

    // Initialize ADC
    adc_init();                    // PA1 as analog input
    uint16_t val = adc_read();

    // Initialize PWM on timer pin
    pwm_init();                    // PA6 as PWM output (TIM3_CH1)

    // Initialize Real-Time Clock (uses internal LSI)
    rtc_init();

    // Initialize general-purpose timer (no pin usage)
    timer2_init();

    // Initialize SPI for BMP280 sensor
    bmp280_init();                 // SPI1: PA5 (SCK), PA6 (MISO), PA7 (MOSI), PA4 (CS)

    // Initialize I2C for MPU6500 sensor
    mpu6500_init();                // I2C1: PB6 (SCL), PB7 (SDA)

    while (1) {
        // Application loop
    }
}
```

## How to Use in STM32CubeIDE

  1. How to Set Up a Bare-Metal Project in STM32CubeIDE (No HAL)

   - Create a New STM32 Project

      - Open STM32CubeIDE.

      - Go to File > New > STM32 Project.

      - Select your MCU (e.g., STM32F407VGTx) and click Next.

      - Give your project a name (e.g., bare_metal_f407) and click Finish.

      - When prompted for initialization, choose "Empty Project"

  2. Copy all *_driver.c files to the Src/ directory.

  3. Include the appropriate headers if needed (or paste code directly in main.c as learning exercise).

  4. Build and flash the firmware to your board.

## Notes

  1. All peripherals are manually initialized using CMSIS (stm32f4xx.h) register definitions.

  2. Clock settings and GPIO configurations must match your board layout.

  3. You can expand each module with your own abstraction layer.

## References

 - ## References

- [STM32F407 Reference Manual (RM0090)](https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405415-stm32f407417-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)  
  Detailed technical documentation for the STM32F407 microcontroller, including peripheral registers and memory maps.

- [STM32CubeIDE Documentation](https://www.st.com/en/development-tools/stm32cubeide.html)  
  Official documentation and resources for using STM32CubeIDE for project creation, build, debugging, and programming.

- [BMP280 Datasheet (Bosch)](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf)  
  Complete datasheet for the BMP280 digital pressure and temperature sensor, used in SPI communication.

- [MPU6500 Datasheet (TDK InvenSense)](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6500-Datasheet1.pdf)  
  Full datasheet for the MPU6500 6-axis accelerometer and gyroscope, interfaced via I2C.


##This project is part of my learning journey into bare-metal programming on STM32F407. I’ve written these drivers by directly working with registers, without using HAL or libraries, to better understand how microcontrollers work under the hood.

Since I'm still learning, I’d really appreciate any feedback, suggestions, or corrections from more experienced developers.

Please feel free to share your knowledge! You can open a GitHub issue, leave a comment, or suggest changes via pull request.

Your guidance will help me and others learn and grow in embedded systems development. Thank you in advance!
