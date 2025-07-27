
#include <stdint.h>
#include <stdio.h>

#define RCC 0x40023800
#define RCC_APB1ENR  (*(volatile uint32_t *) (RCC + 0x40))
#define RCC_AHB1ENR  (*(volatile uint32_t *) (RCC + 0x30))

#define GPIOB_BASE   0x40020400
#define GPIOB_MODER      (*(volatile uint32_t *) (GPIOB_BASE + 0x00))
#define GPIOB_OSPEEDR    (*(volatile uint32_t *) (GPIOB_BASE + 0x08))
#define GPIOB_AFRL       (*(volatile uint32_t *) (GPIOB_BASE + 0x20))
#define GPIOB_AFRH       (*(volatile uint32_t *) (GPIOB_BASE + 0x24))
#define GPIOB_OTYPER     (*(volatile uint32_t *) (GPIOB_BASE + 0x04))
#define GPIOB_PUPDR      (*(volatile uint32_t *) (GPIOB_BASE + 0x0C))
#define GPIOB_ODR        (*(volatile uint32_t *) (GPIOB_BASE + 0x14))

#define I2C1_BASE 0x40005400
#define I2C1_CR1        (*(volatile uint32_t *) (I2C1_BASE + 0x00))
#define I2C1_CR2        (*(volatile uint32_t *) (I2C1_BASE + 0x04))
#define I2C1_OAR1       (*(volatile uint32_t *) (I2C1_BASE + 0x08))
#define I2C1_OAR2       (*(volatile uint32_t *) (I2C1_BASE + 0x0C))
#define I2C1_DR         (*(volatile uint32_t *) (I2C1_BASE + 0x10))
#define I2C1_SR1        (*(volatile uint32_t *) (I2C1_BASE + 0x14))
#define I2C1_SR2        (*(volatile uint32_t *) (I2C1_BASE + 0x18))
#define I2C1_CCR        (*(volatile uint32_t *) (I2C1_BASE + 0x1C))
#define I2C1_TRISE      (*(volatile uint32_t *) (I2C1_BASE + 0x20))


#define MPU6500_ADD 0x68
#define DEVICE_ID 0x70
#define TEMP_L 0x42
#define TEMP_H 0x41
#define WHO_AM_I 0x75

#define ACC_X_L 0x3C
#define ACC_X_H 0x3B
#define ACC_Y_L 0x3E
#define ACC_Y_H 0x3D
#define ACC_Z_L 0x40
#define ACC_Z_H 0x3F

void gpio_init()
{
        RCC_AHB1ENR |= (1<<1); // portb clock enable

        GPIOB_MODER &=~ ((1<<12) | (1<<13) | (1<<14) | (1<<15)); // clear pin 6,7 mode
        GPIOB_MODER |= (1<<13) | (1<<15); // alt function at pb6 and pb7

        GPIOB_OTYPER |= ((1<<6) | (1<<7)); //set open drain
        GPIOB_OSPEEDR &=~ ((1<<12) | (1<<13) | (1<<14) | (1<<15)); // clear pin 6,7 speed
        GPIOB_OSPEEDR |= (1<<12) | (1<<13) | (1<<14) | (1<<15); // Very high speed
        GPIOB_PUPDR &=~ ((1<<12) | (1<<13) | (1<<14) | (1<<15)); // clear pin 6,7 pull mode
        GPIOB_PUPDR |= (1<<12) | (1<<14); // pull up on pb6 and pb7
        GPIOB_AFRL |= (4<<(4*6)) | (4<<(4*7)); ////alternat function 4 set for gpiob pin 6 i2c SCK and pin 7 i2c SDA
}

void i2c_reset()
{
        I2C1_CR1 |= (1<<15);
        I2C1_CR1 &=~ (1<<15);
}

void i2c_init()
{
        RCC_APB1ENR |= (1<<21); // I2C1 clock enable
        i2c_reset();
        I2C1_CR2 |= 0x10; // 16MHz clock
        I2C1_CCR = 0x19;//
        I2C1_TRISE = 0x5;//
        I2C1_CR1 |= (1<<0); // enable I2C
}

uint8_t i2c_read(uint8_t Address)
{
        I2C1_CR1 |= (1<<8) | (1<<10); // start and ack enable
        while(!(I2C1_SR1 & (1<<0)));// wait till start bit is generated
        I2C1_DR = ((MPU6500_ADD << 1) | 0);// mpu6500 address + write operation
        while(!(I2C1_SR1 & (1<<1))); // wait till address is sent
        uint16_t temp = (I2C1_SR1 | I2C1_SR2); // reset the Status registers
        while(!(I2C1_SR1 & (1<<7))); // wait till Tx buffer is empty
        I2C1_DR = Address;      // data write to data register
        while(!(I2C1_SR1 & (1<<2))); // wait till byte is transferred

        I2C1_CR1 |= (1<<8);     // again send the start
        while(!(I2C1_SR1 & (1<<0))); // wait till start bit is generated
        I2C1_DR = ((MPU6500_ADD << 1) | 1);//mpu6500 address+ read operation
        while(!(I2C1_SR1 & (1<<1))); // wait till address is sent
        temp = (I2C1_SR1 | I2C1_SR2); // clear Status registers 1 and 2

        I2C1_CR1 &=~ (1<<10); // disable ack
        I2C1_CR1 |= (1<<9); // stop bit generated
        while(!(I2C1_SR1 & (1<<6))); // wait till Rx is not empty
        return I2C1_DR; // send the received data
}

void delay(int n)
{
        for(int i = 0 ; i<(4000*n); i++);
}

int main(void)
{
        gpio_init();
        i2c_init();
        uint16_t accl[3];
        uint16_t temp;

        if(i2c_read(WHO_AM_I) == DEVICE_ID)
        {
                while(1)
                {
                        accl[0] = (i2c_read(ACC_X_H) << 8) | (i2c_read(ACC_X_L)); //x
                        accl[1] = (i2c_read(ACC_Y_H) << 8) | (i2c_read(ACC_Y_L)); // y
                        accl[2] = (i2c_read(ACC_Z_H) << 8) | (i2c_read(ACC_Z_L)); //z

                        temp = (i2c_read(TEMP_H) << 8) | (i2c_read(TEMP_L));
                        delay(1000);
                }
        }
}

