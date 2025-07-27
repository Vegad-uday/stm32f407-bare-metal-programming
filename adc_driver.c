#include <stdint.h>

#define RCC_BASE        (0x40023800)
#define RCC_AHB1ENR     (*(volatile uint32_t*)(RCC_BASE + 0x30))
#define RCC_APB2ENR     (*(volatile uint32_t*)(RCC_BASE + 0x44))

#define GPIOA_BASE      (0x40020000)
#define GPIOA_MODER     (*(volatile uint32_t*)(GPIOA_BASE + 0x00))
#define GPIOA_PUPDR     (*(volatile uint32_t*)(GPIOA_BASE + 0x0C))

#define ADC2_BASE       (0x40012100)
#define ADC2_SR         (*(volatile uint32_t*)(ADC2_BASE + 0x00))
#define ADC2_CR1        (*(volatile uint32_t*)(ADC2_BASE + 0x04))
#define ADC2_CR2        (*(volatile uint32_t*)(ADC2_BASE + 0x08))
#define ADC2_SMPR2      (*(volatile uint32_t*)(ADC2_BASE + 0x10))
#define ADC2_SQR3       (*(volatile uint32_t*)(ADC2_BASE + 0x34))
#define ADC2_DR         (*(volatile uint32_t*)(ADC2_BASE + 0x4C))

int main(void)
{
    // enable GPIOA clock
    RCC_AHB1ENR |= (1 << 0);

    // set PA1 to analog mode
    GPIOA_MODER |= (3 << (1 * 2));
    GPIOA_PUPDR &= ~(3 << (1 * 2));

    // enable ADC2 clock
    RCC_APB2ENR |= (1 << 9);

    // disable ADC
    ADC2_CR2 &= ~(1 << 0);

    // set sampling time for channel 1 to maximum (480 cycles)
    ADC2_SMPR2 |= (7 << (1 * 3)); // Channel 1

    // select channel 1 as 1st conversion
    ADC2_SQR3 = 1;

    // enable ADC2
    ADC2_CR2 |= (1 << 0);

    // start conversion
    ADC2_CR2 |= (1 << 30);
    while(1){
        // wait until EOC  flag is set
        while (!(ADC2_SR & (1 << 1)));

        // read ADC result
        uint16_t adc_value = ADC2_DR;

        // start next conversion
        ADC2_CR2 |= (1 << 30);
        }
}

