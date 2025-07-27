#include <stdint.h>

#define RCC_BASE         (0x40023800)
#define RCC_AHB1ENR      *((volatile uint32_t*)(RCC_BASE + 0x30))
#define RCC_APB1ENR      *((volatile uint32_t*)(RCC_BASE + 0x40))

#define GPIOD_BASE       (0x40020C00)
#define GPIOD_MODER      *((volatile uint32_t*)(GPIOD_BASE + 0x00))
#define GPIOD_OTYPER     *((volatile uint32_t*)(GPIOD_BASE + 0x04))
#define GPIOD_OSPEEDR    *((volatile uint32_t*)(GPIOD_BASE + 0x08))
#define GPIOD_PUPDR      *((volatile uint32_t*)(GPIOD_BASE + 0x0C))
#define GPIOD_BSRR       *((volatile uint32_t*)(GPIOD_BASE + 0x18))

#define TIM2_BASE        (0x40000000)
#define TIM2_CR1         *((volatile uint32_t*)(TIM2_BASE + 0x00))
#define TIM2_SR          *((volatile uint32_t*)(TIM2_BASE + 0x10))
#define TIM2_CNT         *((volatile uint32_t*)(TIM2_BASE + 0x24))
#define TIM2_PSC         *((volatile uint32_t*)(TIM2_BASE + 0x28))
#define TIM2_ARR         *((volatile uint32_t*)(TIM2_BASE + 0x2C))

#define LED_PIN          15

void delay_ms(uint32_t ms)
{
    // enable TIM2 clock
    RCC_APB1ENR |= (1 << 0);

    TIM2_PSC = 16000 - 1;           // 16 MHz / 16000 = 1 KHz (1 ms tick)
    TIM2_ARR = ms - 1;              // auto-reload value
    TIM2_CNT = 0;                   // reset counter
    TIM2_SR &= ~(1 << 0);           // clear update flag
    TIM2_CR1 |= (1 << 0);           // enable counter

    while (!(TIM2_SR & (1 << 0)));  // wait for update event
    TIM2_SR &= ~(1 << 0);           // clear update flag
    TIM2_CR1 &= ~(1 << 0);          // stop timer
}

int main(void)
{
    // enable GPIOD clock
    RCC_AHB1ENR |= (1 << 3);

    GPIOD_MODER   &= ~(0x3 << (LED_PIN * 2));	//clear gpiod mode
    GPIOD_MODER   |=  (0x1 << (LED_PIN * 2));    // output mode
    GPIOD_OTYPER  &= ~(1 << LED_PIN);            // push-pull
    GPIOD_OSPEEDR |=  (0x3 << (LED_PIN * 2));    // very high speed
    GPIOD_PUPDR   &= ~(0x3 << (LED_PIN * 2));    // no pull-up/pull-down

    while (1)
    {
        GPIOD_BSRR = (1 << LED_PIN);             // led on
        delay_ms(1000);                          // 1 second delay

        GPIOD_BSRR = (1 << (LED_PIN + 16));      // led off
        delay_ms(1000);                          // 1 second delay
    }
}


